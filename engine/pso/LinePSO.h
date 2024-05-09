#pragma once
#include "DirectXCommon.h"
#include "WinApp.h"
#include "ImGuiManager.h"
#include "TextureManager.h"
#include "IPSO.h"

#include <dxcapi.h>
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dxcompiler.lib")

class LinePSO : public IPSO
{
public:
	/// <summary>
	/// シングルトン
	/// </summary>
	static LinePSO* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Init(IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler, const std::string& VS_fileName, const std::string& PS_fileName) override;

	/// <summary>
	/// rootSignatureの作成
	/// </summary>
	void CreateRootSignature() override;

	/// <summary>
	/// PSOの作成
	/// </summary>
	void CreatePSO() override;

	///** Getter **///


	///** Setter **///

	/// <summary>
	/// 描画前に積むコマンド
	/// </summary>
	void SetCommand() {
		// シグネチャの設定
		dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature_.Get());
		// PSOを設定
		dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState_.Get());
		// 形状を設定
		dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_LINELIST);
	}
private:
	//IDxcUtils* dxcUtils_;
	//IDxcCompiler3* dxcCompiler_;
	//IDxcIncludeHandler* includeHandler_;

	//ID3DBlob* errorBlob_;

	//Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	D3D12_INPUT_ELEMENT_DESC inputElementDescs_[1];
	/*D3D12_INPUT_LAYOUT_DESC inputLayoutDesc_;
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature_;
	ID3DBlob* signatureBlob_;

	D3D12_RASTERIZER_DESC rasterizerDesc_;
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDescs_;
	D3D12_BLEND_DESC blendDesc_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState_;*/
	D3D12_ROOT_PARAMETER rootParameters_[2];
	D3D12_DESCRIPTOR_RANGE descriptorRange_[1];

	D3D12_STATIC_SAMPLER_DESC staticSamplers_[1];

	//// 通常
	//IDxcBlob* vertexShaderBlob_;
	//IDxcBlob* pixelShaderBlob_;

	//D3D12_VIEWPORT viewport_;
	//D3D12_RECT scissorRect_;
};