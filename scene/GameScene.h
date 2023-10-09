#pragma once
#include "IScene.h"
// base
#include "../base/Model.h"
#include "../base/ViewProjection.h"
// components
#include "../components/Input.h"
#include "../components/camera/FollowCamera.h"
// GameObject
#include "../gameObject/player/Player.h"
#include "../gameObject/ground/Ground.h"
#include "../gameObject/skydome/Skydome.h"
#include "../gameObject/enemy/Enemy.h"
// c
#include <memory>

class GameManager;
class GameScene : public IScene
{
public:
	// 初期化
	void Initialize()override;

	// 更新処理
	void Update()override;

	// 描画
	void Draw()override;

	void Finalize()override;
private:
	// 入力
	Input* input_;

	// 3Dモデルデータ

	// 自機の3Dモデル
	std::unique_ptr<Model> playerModel_;
	std::unique_ptr<Model> modelFighterBody_;
	std::unique_ptr<Model> modelFighterHead_;
	std::unique_ptr<Model> modelFighterL_arm_;
	std::unique_ptr<Model> modelFighterR_arm_;
	// 自機の武器
	std::unique_ptr<Model> modelFighterHammer_;

	// 敵の3Dモデル
	std::unique_ptr<Model> modelEnemy_;

	std::unique_ptr<Model> groundModel_;
	std::unique_ptr<Model> skydomeModel_;

	// ビュープロジェクション
	ViewProjection viewProjection_;

	// 追従カメラ
	std::unique_ptr<FollowCamera> followCamera_;
	// 自キャラ
	std::unique_ptr<Player> player_;
	// 敵
	std::unique_ptr<Enemy> enemy_;
	// 地面
	std::unique_ptr<Ground> ground_;
	// 天球
	std::unique_ptr<Skydome> skydome_;
};

