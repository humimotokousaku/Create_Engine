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
	worldTransform.matWorld_ = Multiply(model_->GetAnimationMatrix(), Multiply(model_->GetModelData().rootNode.localMatrix, worldTransform.matWorld_));
	worldTransform.TransferMatrix();

	/// コマンドを積む
	// 使用するPSO
	Object3dPSO::GetInstance()->SetCommand();

	// worldTransform
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldTransform.constBuff_->GetGPUVirtualAddress());
	
	// 見た目を描画
	model_->Draw(camera_->GetViewProjection(), textureNum);
}