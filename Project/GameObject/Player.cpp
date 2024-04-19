#include "Player.h"
#include "TextureManager.h"
#include "Collision/CollisionConfig.h"

Player::Player() {}
Player::~Player() {
	//delete model_;
}

void Player::Initialize(Camera* camera) {
	// colliderの設定
	SetCollisionPrimitive(kCollisionOBB);

	model_ = std::make_unique<Object3D>();
	model_->Initialize();
	model_->SetModel("", "block.obj");
	model_->SetCamera(camera);
}

void Player::Update() {
	//if (Input::GetInstance()->PressKey(DIK_A)) {
	//	model_->worldTransform.translation_.x -= 0.1f;
	//}
	//if (Input::GetInstance()->PressKey(DIK_D)) {
	//	model_->worldTransform.translation_.x += 0.1f;
	//}
	//if (Input::GetInstance()->PressKey(DIK_S)) {
	//	model_->worldTransform.translation_.z -= 0.1f;
	//}
	//if (Input::GetInstance()->PressKey(DIK_W)) {
	//	model_->worldTransform.translation_.z += 0.1f;
	//}

	//if (Input::GetInstance()->PressKey(DIK_LEFT)) {
	//	model_->worldTransform.rotation_.y -= 0.01f;
	//}
	//if (Input::GetInstance()->PressKey(DIK_RIGHT)) {
	//	model_->worldTransform.rotation_.y += 0.01f;
	//}
}

void Player::Draw(uint32_t textureHandle) {
	// player
	model_->Draw(textureHandle);
}

void Player::OnCollision(Collider* collider) {

}

Vector3 Player::GetRotation() {
	Vector3 rotate = model_->worldTransform.transform.rotate;
	return rotate;
}

Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる変数
	Vector3 worldPos = model_->worldTransform.transform.translate;
	//// ワールド行列の平行移動成分を取得
	//worldPos.x = model_->worldTransform.matWorld_.m[3][0];
	//worldPos.y = model_->worldTransform.matWorld_.m[3][1];
	//worldPos.z = model_->worldTransform.matWorld_.m[3][2];

	return worldPos;
}