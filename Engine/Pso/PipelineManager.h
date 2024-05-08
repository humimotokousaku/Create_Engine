#pragma once
#include "DirectXCommon.h"
#include "WinApp.h"
#include "ImGuiManager.h"
#include "TextureManager.h"

#include <dxcapi.h>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

enum BlendMode {
	// ブレンドなし
	kBlendModeNone,
	// 通常αブレンド
	kBlendModeNormal,
	// 加算
	kBlendModeAdd,
	// 減算
	kBlendModeSubtract,
	// 乗算
	kBlendModeMultiply,
	// スクリーン
	kBlendModeScreen,
	// 利用してはいけない
	kCountOfBlendMode,
};

class PipelineManager {
public:
	///
	/// Default Method
	///
	
	static PipelineManager* GetInstance();

	~PipelineManager() = default;

	// エンジンの初期化
	void Initialize();

	// 描画前の処理
	void PreDraw();

	// 描画後の処理
	void PostDraw();

	///
	/// User Method
	/// 

	// Getter
	Microsoft::WRL::ComPtr<ID3D12PipelineState>* GetGraphicsPipelineState() { return graphicsPipelineState_; }
	Microsoft::WRL::ComPtr<ID3D12RootSignature>* GetRootSignature() { return rootSignature_; }

private:// プライベートな関数
	// DXCの初期化
	void DXCInitialize();

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

	// DescriptorRangeの生成
	void CreateDescriptorRange();

	// Samplerの設定
	void SettingSampler();

	// RootSignatureの生成
	void CreateRootSignature();

	// RootParameter生成
	void CreateRootParameter();

	// InputLayerの設定
	void SettingInputLayout();

	// BlendStateの設定
	void SettingBlendState();

	// RasterizerStateの設定
	void SettingRasterizerState();

	// ピクセルシェーダー
	void PixelSharder();

	// 頂点シェーダー
	void VertexSharder();

	// PSOの生成
	void CreatePSO();

	// PSO
	void PSO();

	// ビューポート
	void CreateViewport();

	// シザー矩形
	void CreateScissor();

private:
	IDxcUtils* dxcUtils_;
	IDxcCompiler3* dxcCompiler_;
	IDxcIncludeHandler* includeHandler_;

	ID3DBlob* errorBlob_;

	static const int kMaxPSO = 7;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_[kMaxPSO];
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[kMaxPSO][5];
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_[kMaxPSO];
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_[kMaxPSO];
	ID3DBlob* signatureBlob_[kMaxPSO];

	D3D12_RASTERIZER_DESC rasterizerDesc_[kMaxPSO];
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDescs_[kMaxPSO];
	D3D12_BLEND_DESC blendDesc_[kMaxPSO];
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_[kMaxPSO];
	D3D12_ROOT_PARAMETER rootParameters_[kMaxPSO][9];
	D3D12_DESCRIPTOR_RANGE descriptorRange_[kMaxPSO][1];
	D3D12_STATIC_SAMPLER_DESC staticSamplers_[kMaxPSO][1];

	// 通常
	IDxcBlob* vertexShaderBlob_;
	IDxcBlob* pixelShaderBlob_;
	// particle用
	IDxcBlob* particleVertexShaderBlob_;
	IDxcBlob* particlePixelShaderBlob_;
	// ポストエフェクト用
	IDxcBlob* PostEffectVertexShaderBlob_;
	IDxcBlob* PostEffectPixelShaderBlob_;
	IDxcBlob* scanLinePixelShaderBlob_;

	D3D12_VIEWPORT viewport_;
	D3D12_RECT scissorRect_;
};
