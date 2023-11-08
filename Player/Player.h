#pragma once
#include "PlayerBullet.h"
#include "../components/Input.h"
#include "../Collision/Collider.h"
#include "../base/Model.h"
#include "../base/WorldTransform.h"
#include "../math/Vector4.h"
#include <list>

class Player : public Collider {
public:
	Player();

	/// <summary>
	/// デストラクタ
	/// </summary>
	~Player();

	/// <summary>
	/// 初期化
	/// <summary>
	void Initialize(Model* model, uint32_t textureHandle);

	/// <summary>
	/// 更新
	/// <summary>
	void Update();

	/// <summary>
	/// 描画
	/// <summary>
	void Draw(ViewProjection& viewProjection);

	///
	/// User Method
	/// 

	// playerの回転
	void Rotate();

	/// <summary>
	/// 攻撃
	/// </summary>
	void Attack();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision() override;

	// ワールド行列の平行移動成分を取得
	Vector3 GetWorldPosition() override;

	// 弾リストを取得
	const std::list<PlayerBullet*>& GetBullets() const { return bullets_; }

public:
	// キーボード入力
	Input* input_ = nullptr;

	// ワールド変換データ
	WorldTransform worldTransform_;

	// モデル
	Model* model_ = nullptr;
	// テクスチャハンドル
	uint32_t playerTexture_ = 0u;

	// 弾
	std::list<PlayerBullet*> bullets_;
};