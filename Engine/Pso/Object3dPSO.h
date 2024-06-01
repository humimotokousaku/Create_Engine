#pragma once
#include "IPSO.h"

class Object3dPSO : public IPSO {
public:
	enum class PSOTypes {
		OffAnim, // アニメーションなし
		OnAnim,	 // アニメーションあり
		Count
	};

public:// メンバ関数
	/// <summary>
	/// シングルトン
	/// </summary>
	static Object3dPSO* GetInstance();

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
		dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}

private:// プライベートなメンバ変数

};