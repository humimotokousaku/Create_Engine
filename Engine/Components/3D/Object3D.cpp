#include "Object3D.h"
#include "PipelineManager.h"
#include "ModelManager.h"

Object3D::~Object3D() {
	worldTransform.constBuff_.ReleaseAndGetAddressOf();
}

void Object3D::Initialize() {
	worldTransform.Initialize();
}

void Object3D::Draw(uint32_t textureNum) {
	// カメラ
	if (camera_) {
		camera_->Update();
	}

	// ワールド座標の更新
	worldTransform.UpdateMatrix();
	// nodeを含み計算
	worldTransform.matWorld_ = Multiply(model_->GetModelData().rootNode.localMatrix, Multiply(model_->GetModelData().rootNode.localMatrix, worldTransform.matWorld_));
	worldTransform.TransferMatrix();

	/// コマンドを積む
	// RootSignatureを設定。PSOに設定しているけど別途設定が必要
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootSignature(PipelineManager::GetInstance()->GetRootSignature()[1].Get());
	DirectXCommon::GetInstance()->GetCommandList()->SetPipelineState(PipelineManager::GetInstance()->GetGraphicsPipelineState()[1].Get()); // PSOを設定

	// 形状を設定
	DirectXCommon::GetInstance()->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	// worldTransform
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldTransform.constBuff_->GetGPUVirtualAddress());
	
	// 見た目を描画
	model_->Draw(camera_->GetViewProjection(), textureNum);
}