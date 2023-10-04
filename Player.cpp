#include "Player.h"
#include <cassert>


Player::Player() {}
Player::~Player() {worldTransform_.constBuff_.ReleaseAndGetAddressOf();}

// Initializeの関数定義
void Player::Initialize(Model* model, uint32_t textureHandle) {
	// NULLポインタチェック
	assert(model);

	// 引数として受け取ったデータをメンバ変数に記録する
	model_ = model;
	playerTexture_ = textureHandle;

	// ワールド変換の初期化
	worldTransform_.Initialize();
}

// Updateの関数定義
void Player::Update() {
	// 行列を定数バッファに転送
	worldTransform_.TransferMatrix();
}

// Drawの関数定義
void Player::Draw(ViewProjection& viewProjection) {
	model_->Draw(worldTransform_, viewProjection, playerTexture_);
}
