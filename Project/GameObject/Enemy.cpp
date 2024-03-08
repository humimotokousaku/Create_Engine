#include "Enemy.h"
#include "TextureManager.h"
#include "Collision/CollisionConfig.h"

Enemy::~Enemy() {
	//delete model_;
}

void Enemy::Initialize(Camera* camera) {
	// collider�̐ݒ�
	SetCollisionPrimitive(kCollisionOBB);

	model_ = std::make_unique<Object3D>();
	model_->Initialize();
	model_->SetModel("block.obj");
	model_->SetCamera(camera);
}

void Enemy::Update() {

}

void Enemy::Draw() {
	// Enemy
	model_->Draw(UVCHEKER);
}

void Enemy::OnCollision(Collider* collider) {

}

Vector3 Enemy::GetRotation() {
	Vector3 rotate = model_->worldTransform.rotation_;
	return rotate;
}

Vector3 Enemy::GetWorldPosition() {
	// ���[���h���W������ϐ�
	Vector3 worldPos = model_->worldTransform.translation_;
	//// ���[���h�s��̕��s�ړ��������擾
	//worldPos.x = model_->worldTransform.matWorld_.m[3][0];
	//worldPos.y = model_->worldTransform.matWorld_.m[3][1];
	//worldPos.z = model_->worldTransform.matWorld_.m[3][2];

	return worldPos;
}