#pragma once
#include "DirectXCommon.h"
#include "WinApp.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include <dxcapi.h>

/// <summary>
/// 使用できるポストエフェクトの種類
/// </summary>
enum PostEffectType {
	NORMAL,
	HIGHINTENSITY,
	BLUR,
	COUNT
};

// 構造体
// ブラー
struct BlurData {
	uint32_t isActive;  // 使用するか
	float strength;  // ブラーの強さ
};
// 高輝度
struct HighIntensityData {
	uint32_t isActive;	 // 使用するか
	float threshold;  // 閾値
};

/// <summary>
/// ポストエフェクトのPSO
/// </summary>
class PostEffectPSO
{
public:
	///
	/// Default Method
	///

	static PostEffectPSO* GetInstance();

	~PostEffectPSO() = default;

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

	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_[COUNT];
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[COUNT][2];
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_[COUNT];
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_[COUNT];
	ID3DBlob* signatureBlob_[COUNT];

	D3D12_RASTERIZER_DESC rasterizerDesc_[COUNT];
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDescs_[COUNT];
	D3D12_BLEND_DESC blendDesc_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_[COUNT];
	D3D12_ROOT_PARAMETER rootParameters_[COUNT][4];
	D3D12_DESCRIPTOR_RANGE descriptorRange_[COUNT][1];

	D3D12_STATIC_SAMPLER_DESC staticSamplers_[COUNT][1];

	// 通常
	IDxcBlob* vertexShaderBlob_[COUNT];
	IDxcBlob* pixelShaderBlob_[COUNT];

	D3D12_VIEWPORT viewport_;
	D3D12_RECT scissorRect_;
};