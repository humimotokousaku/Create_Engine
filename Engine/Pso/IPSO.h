#pragma once
#include "DirectXCommon.h"
#include "WinApp.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include <dxcapi.h>

/// <summary>
/// PSOの基底クラス(PSOManager内で作成することを想定)
/// </summary>
class IPSO {
public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~IPSO() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	virtual void Init(IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler,IDxcIncludeHandler* includeHandler);

	/// <summary>
	/// rootSignatureの作成
	/// </summary>
	virtual void CreateRootSignature() = 0;

	/// <summary>
	/// PSOの作成
	/// </summary>
	/// <param name="VS_SharderName">VSシェーダの名前</param>
	/// <param name="PS_SharderName">PSシェーダの名前</param>
	virtual void CreatePSO() = 0;

	// シェーダのコンパイル
	IDxcBlob* CompileShader(
		// CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		// Compilerに使用するProfile
		const wchar_t* profile,
		// 初期化で生成したものを3つ
		IDxcUtils* dxcUtils,
		IDxcCompiler3* dxcCompiler,
		IDxcIncludeHandler* includeHandler);

protected:
	// directXCommonのインスタンス
	DirectXCommon* dxCommon_;

	// シェーダ読み込みに使用する
	IDxcUtils* dxcUtils_;
	IDxcCompiler3* dxcCompiler_;
	IDxcIncludeHandler* includeHandler_;
	ID3DBlob* errorBlob_;
	ID3DBlob* signatureBlob_;
	IDxcBlob* vertexShaderBlob_;
	IDxcBlob* pixelShaderBlob_;

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_;

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_;
	D3D12_RASTERIZER_DESC rasterizerDesc_;
	D3D12_BLEND_DESC blendDesc_;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDescs_;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;
};