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

	// アニメーション
	model_->animation_ = animation_;
	// スケルトン
	model_->skeleton_ = skeleton_;
	// スキンクラスタ
	model_->skinCluster_ = skinCluster_;

	/// コマンドを積む
	if (model_->GetModelData().isSkinClusterData) {
		// 使用するPSO
		SkinningPSO::GetInstance()->SetCommand();
		// アニメーション
		if (model_->animation_.isActive) {
			AnimationUpdate(skinCluster_, skeleton_, animation_, animationTime_);
		}
	}
	else {
		// 使用するPSO
		Object3dPSO::GetInstance()->SetCommand();
		// スキンクラスターはないがアニメーションがある
		if (model_->animation_.isActive) {
			NodeAnimation& rootNodeAnimation = animation_.nodeAnimations[model_->GetModelData().rootNode.name];
			Vector3 translate = CalculateTranslateValue(rootNodeAnimation.translate.keyframes, animationTime_);
			Quaternion rotate = CalculateQuaternionValue(rootNodeAnimation.rotate.keyframes, animationTime_);
			Vector3 scale = CalculateScaleValue(rootNodeAnimation.scale.keyframes, animationTime_);
			Matrix4x4 localMatrix = MakeAffineMatrix(scale, rotate, translate);
			worldTransform.matWorld_ = Multiply(localMatrix, worldTransform.matWorld_);
			animationTime_ += 1.0f / 60.0f * animation_.playBackSpeed;

			// ループ再生の場合
			if (animation_.isLoop) {
				// 通常
				if (animation_.playBackSpeed >= 0.001f) {
					animationTime_ = std::fmod(animationTime_, animation_.duration);
				}
				// 逆再生
				else if (animation_.playBackSpeed <= 0.0f) {
					animationTime_ = Custom_fmod(animationTime_, 0, animation_.duration);
				}
			}
		}
		worldTransform.TransferMatrix();
	}

	// worldTransform
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldTransform.constBuff_->GetGPUVirtualAddress());


	// 見た目を描画
	model_->Draw(camera_->GetViewProjection(), textureNum);
}

void Object3D::Draw() {
	// カメラ
	if (camera_) {
		camera_->Update();
	}

	// ワールド座標の更新
	worldTransform.UpdateMatrix();

	/// コマンドを積む
	if (model_->GetModelData().isSkinClusterData) {
		// 使用するPSO
		SkinningPSO::GetInstance()->SetCommand();
		// アニメーション
		if (model_->animation_.isActive) {
			AnimationUpdate(skinCluster_, skeleton_, animation_, animationTime_);
		}
	}
	else {
		// 使用するPSO
		Object3dPSO::GetInstance()->SetCommand();
		// スキンクラスターはないがアニメーションがある
		if (model_->animation_.isActive) {
			NodeAnimation& rootNodeAnimation = animation_.nodeAnimations[model_->GetModelData().rootNode.name];
			Vector3 translate = CalculateTranslateValue(rootNodeAnimation.translate.keyframes, animationTime_);
			Quaternion rotate = CalculateQuaternionValue(rootNodeAnimation.rotate.keyframes, animationTime_);
			Vector3 scale = CalculateScaleValue(rootNodeAnimation.scale.keyframes, animationTime_);
			Matrix4x4 localMatrix = MakeAffineMatrix(scale, rotate, translate);
			worldTransform.matWorld_ = Multiply(localMatrix, worldTransform.matWorld_);
			animationTime_ += 1.0f / 60.0f;
			animationTime_ = std::fmod(animationTime_, animation_.duration);
		}
		worldTransform.TransferMatrix();
	}

	// worldTransform
	DirectXCommon::GetInstance()->GetCommandList()->SetGraphicsRootConstantBufferView(1, worldTransform.constBuff_->GetGPUVirtualAddress());

	// アニメーション
	if (model_->animation_.isActive == model_->GetModelData().isSkinClusterData) {
		AnimationUpdate(skinCluster_, skeleton_, animation_, animationTime_);
	}

	// 見た目を描画
	model_->Draw(camera_->GetViewProjection());
}

void Object3D::ImGuiParameter(const char* name) {
	ImGui::Begin(name);
	ImGui::DragFloat3("translation", &worldTransform.transform.translate.x, 0.01f, -100, 100);
	ImGui::DragFloat3("scale", &worldTransform.transform.scale.x, 0.01f, -100, 100);
	ImGui::DragFloat3("rotate", &worldTransform.transform.rotate.x, 0.01f, -6.28f, 6.28f);
	ImGui::DragFloat("playBackSpeed", &animation_.playBackSpeed, 0.01f, -10.0f, 10.0f);
	ImGui::Checkbox("isAnimation", &animation_.isActive);
	ImGui::DragFloat("duration", &animation_.duration, 0,-10.10);
	ImGui::DragFloat("animTime", &animationTime_,0,-100,100);
	ImGui::End();
}
