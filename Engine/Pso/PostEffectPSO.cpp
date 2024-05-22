#include "PostEffectPSO.h"
#include "ConvertString.h"
#include <format>
#include <cassert>

PostEffectPSO* PostEffectPSO::GetInstance() {
	static PostEffectPSO instance;

	return &instance;
}

void PostEffectPSO::Initialize() {
	// DXCの初期化
	DXCInitialize();
	// PSOを生成
	PSO();
	// ビューポートの生成
	CreateViewport();
	// シザー矩形の生成
	CreateScissor();
}

void PostEffectPSO::PreDraw() {
	DirectXCommon::GetInstance()->GetCommandList()->RSSetViewports(1, &viewport_); // Viewportを設定
	DirectXCommon::GetInstance()->GetCommandList()->RSSetScissorRects(1, &scissorRect_); // Scirssorを設定
}

void PostEffectPSO::PostDraw() {}

void PostEffectPSO::DXCInitialize() {
	HRESULT hr;
	// dxCompilerの初期化
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));

	// 現時点でincludeはしないが、includeに対応するために設定を行っておく
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}

IDxcBlob* PostEffectPSO::CompileShader(
	// CompilerするShaderファイルへのパス
	const std::wstring& filePath,
	// Compilerに使用するProfile
	const wchar_t* profile,
	// 初期化で生成したものを3つ
	IDxcUtils* dxcUtils,
	IDxcCompiler3* dxcCompiler,
	IDxcIncludeHandler* includeHandler) {

#pragma region hlslファイルを読む

	// これからシェーダーをコンパイラする旨をログに出す
	WinApp::Log(ConvertString(std::format(L"Begin CompileShader, path:{}, profile:{}\n", filePath, profile)));
	// hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	HRESULT hr = dxcUtils->LoadFile(filePath.c_str(), nullptr, &shaderSource);
	// 読めなかったら止める
	assert(SUCCEEDED(hr));
	// 読み込んだファイルの内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; // UTF8の文字コードであることを通知

#pragma endregion

#pragma region Compileする

	LPCWSTR arguments[] = {
		filePath.c_str(), // コンパイル対象のhlslファイル名
		L"-E", L"main", // エントリーポイントの指定。基本的にmain以外にはしない
		L"-T", profile, // ShaderProfileの設定
		L"-Zi", L"-Qembed_debug", // デバッグ用の情報を埋め込む
		L"-Od", // 最適化を外しておく
		L"-Zpr", // メモリレイアウトは行優先
	};
	// 実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr = dxcCompiler->Compile(
		&shaderSourceBuffer,	// 読み込んだファイル
		arguments,				// コンパイルオプション
		_countof(arguments),	// コンパイルオプションの数
		includeHandler,			// includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult) // コンパイル結果
	);
	// コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr));

#pragma endregion

#pragma region 警告・エラーが出てないか確認する

	// 警告・エラーが出てきたらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		WinApp::Log(shaderError->GetStringPointer());
		assert(false);
	}

#pragma endregion

#pragma region Compile結果を受け取って渡す

	// コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr));
	// 成功したログを出す
	WinApp::Log(ConvertString(std::format(L"Compile Succeeded, path:{}, profile:{}\n", filePath, profile)));
	// もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	// 実行用のバイナリを変換
	return shaderBlob;

#pragma endregion

}

void PostEffectPSO::CreateDescriptorRange() {
	for (int i = 0; i < COUNT; i++) {
		descriptorRange_[i][0].BaseShaderRegister = 0;
		descriptorRange_[i][0].NumDescriptors = 1;
		descriptorRange_[i][0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;
		descriptorRange_[i][0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;
	}
}

void PostEffectPSO::SettingSampler() {
	for (int i = 0; i < COUNT; i++) {
		staticSamplers_[i][0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;
		staticSamplers_[i][0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers_[i][0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers_[i][0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
		staticSamplers_[i][0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
		staticSamplers_[i][0].MaxLOD = D3D12_FLOAT32_MAX;
		staticSamplers_[i][0].ShaderRegister = 0;
		staticSamplers_[i][0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		descriptionRootSignature_[i].pStaticSamplers = staticSamplers_[i];
		descriptionRootSignature_[i].NumStaticSamplers = _countof(staticSamplers_[i]);
	}
}

void PostEffectPSO::CreateRootParameter() {
	for (int i = 0; i < COUNT; i++) {
		// material
		rootParameters_[i][0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters_[i][0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters_[i][0].Descriptor.ShaderRegister = 0;
		// texture
		rootParameters_[i][1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
		rootParameters_[i][1].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;
		rootParameters_[i][1].DescriptorTable.pDescriptorRanges = descriptorRange_[i];
		rootParameters_[i][1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange_[i]);
		// worldTransform
		rootParameters_[i][2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters_[i][2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		rootParameters_[i][2].Descriptor.ShaderRegister = 0;
		// viewProjection
		rootParameters_[i][3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
		rootParameters_[i][3].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
		rootParameters_[i][3].Descriptor.ShaderRegister = 1;

		CreateDescriptorRange();
		descriptionRootSignature_[i].pParameters = rootParameters_[i];
		descriptionRootSignature_[i].NumParameters = _countof(rootParameters_[i]);
	}
}

void PostEffectPSO::CreateRootSignature() {
	HRESULT hr;
	for (int i = 0; i < COUNT; i++) {
		descriptionRootSignature_[i].Flags =
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
		// rootParameter生成
		CreateRootParameter();
		// samplerの設定
		SettingSampler();
		// シリアライズしてバイナリにする
		hr = D3D12SerializeRootSignature(&descriptionRootSignature_[i],
			D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob_[i], &errorBlob_);
		if (FAILED(hr)) {
			WinApp::Log(reinterpret_cast<char*>(errorBlob_->GetBufferPointer()));
			assert(false);
		}
		// バイナリをもとに生成
		hr = DirectXCommon::GetInstance()->GetDevice()->CreateRootSignature(0, signatureBlob_[i]->GetBufferPointer(),
			signatureBlob_[i]->GetBufferSize(), IID_PPV_ARGS(&rootSignature_[i]));
		assert(SUCCEEDED(hr));
	}
}

void PostEffectPSO::SettingInputLayout() {
	for (int i = 0; i < COUNT; i++) {
		inputElementDescs_[i][0].SemanticName = "POSITION";
		inputElementDescs_[i][0].SemanticIndex = 0;
		inputElementDescs_[i][0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputElementDescs_[i][0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputElementDescs_[i][1].SemanticName = "TEXCOORD";
		inputElementDescs_[i][1].SemanticIndex = 0;
		inputElementDescs_[i][1].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputElementDescs_[i][1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
		inputLayoutDesc_[i].pInputElementDescs = inputElementDescs_[i];
		inputLayoutDesc_[i].NumElements = _countof(inputElementDescs_[i]);
	}
}

void PostEffectPSO::SettingBlendState() {
	blendDesc_.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	// すべての色要素を書き込む
	blendDesc_.RenderTarget[0].RenderTargetWriteMask =
		D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc_.RenderTarget[0].BlendEnable = true;
	blendDesc_.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc_.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	// ブレンドモードの設定
	// ノーマル
	blendDesc_.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc_.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc_.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
}

void PostEffectPSO::SettingRasterizerState() {
	for (int i = 0; i < COUNT; i++) {
		// 裏面(時計回り)を表示しない
		rasterizerDesc_[i].CullMode = D3D12_CULL_MODE_BACK;
		// 三角形の中を塗りつぶす
		rasterizerDesc_[i].FillMode = D3D12_FILL_MODE_SOLID;
	}
}

void PostEffectPSO::PixelSharder() {
	// 通常
	pixelShaderBlob_[NORMAL] = CompileShader(L"engine/resources/sharder/PostEffectTestPS.hlsl",
		L"ps_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(pixelShaderBlob_ != nullptr);
	// smoothing
	pixelShaderBlob_[SMOOTHING] = CompileShader(L"engine/resources/sharder/Smoothing.PS.hlsl",
		L"ps_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	assert(pixelShaderBlob_ != nullptr);
	
	//// 高輝度
	//pixelShaderBlob_[1] = CompileShader(L"engine/resources/sharder/HighIntensityPS.hlsl",
	//	L"ps_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	//assert(pixelShaderBlob_ != nullptr);
	//// ブラー
	//pixelShaderBlob_[2] = CompileShader(L"engine/resources/sharder/BlurPS.hlsl",
	//	L"ps_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	//assert(pixelShaderBlob_ != nullptr);
	//// ブルーム
	//pixelShaderBlob_[3] = CompileShader(L"engine/resources/sharder/BloomPS.hlsl",
	//	L"ps_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
	//assert(pixelShaderBlob_ != nullptr);
}

void PostEffectPSO::VertexSharder() {
	for (int i = 0; i < COUNT; i++) {
		// Shaderをコンパイルする
		vertexShaderBlob_[i] = CompileShader(L"engine/resources/sharder/PostEffectTestVS.hlsl",
			L"vs_6_0", dxcUtils_, dxcCompiler_, includeHandler_);
		assert(vertexShaderBlob_ != nullptr);
	}
}

void PostEffectPSO::CreatePSO() {
	for (int i = 0; i < COUNT; i++) {
		graphicsPipelineStateDescs_[i].pRootSignature = rootSignature_[i].Get(); // rootSignature
		graphicsPipelineStateDescs_[i].InputLayout = inputLayoutDesc_[i]; // InputLayout

		graphicsPipelineStateDescs_[i].VS = { vertexShaderBlob_[i]->GetBufferPointer(),
		vertexShaderBlob_[i]->GetBufferSize() }; // vertexShader
		graphicsPipelineStateDescs_[i].PS = { pixelShaderBlob_[i]->GetBufferPointer(),
		pixelShaderBlob_[i]->GetBufferSize() }; // pixelShader

		// DepthStencilの設定
		D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
		// Depthの機能を有効化する
		depthStencilDesc.DepthEnable = true;
		// 書き込みをします
		depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
		// 比較関数はLessEqual。つまり、近ければ描画される
		depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
		graphicsPipelineStateDescs_[i].DepthStencilState = depthStencilDesc;
		graphicsPipelineStateDescs_[i].DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

		graphicsPipelineStateDescs_[i].BlendState = blendDesc_; // blendState
		graphicsPipelineStateDescs_[i].RasterizerState = rasterizerDesc_[i]; // rasterizerState

		// 書き込むRTVの情報
		graphicsPipelineStateDescs_[i].NumRenderTargets = 1;
		graphicsPipelineStateDescs_[i].RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

		// 利用するトポロ時（形状）のタイプ。三角形
		graphicsPipelineStateDescs_[i].PrimitiveTopologyType =
			D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;

		// どのように画面に色を打ち込むかの設定（気にしなくてよい）
		graphicsPipelineStateDescs_[i].SampleDesc.Count = 1;
		graphicsPipelineStateDescs_[i].SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

		// 実際に生成
		HRESULT hr;
		hr = DirectXCommon::GetInstance()->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDescs_[i],
			IID_PPV_ARGS(&graphicsPipelineState_[i]));
		assert(SUCCEEDED(hr));
	}
}

void PostEffectPSO::PSO() {

	CreateRootSignature();

	CreateRootParameter();

	SettingInputLayout();

	SettingBlendState();

	SettingRasterizerState();

	VertexSharder();

	PixelSharder();

	CreatePSO();
}

void PostEffectPSO::CreateViewport() {
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = (float)WinApp::kClientWidth_;
	viewport_.Height = (float)WinApp::kClientHeight_;
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
}

void PostEffectPSO::CreateScissor() {
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right = WinApp::kClientWidth_;
	scissorRect_.top = 0;
	scissorRect_.bottom = WinApp::kClientHeight_;
}
