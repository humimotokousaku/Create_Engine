#include "GameScene.h"
#include "../Manager/GameManager.h"
#include "../Manager/ImGuiManager.h"
#include "../Manager/TextureManager.h"
#include <cassert>

GameScene::GameScene() {}

GameScene::~GameScene() {

}

void GameScene::Initialize() {
	sceneNum = GAME_SCENE;

	input_ = Input::GetInstance();
	audio_ = Audio::GetInstance();
	LoadEnemyPopData();

	// ファイル名を指定してテクスチャを読み込む
	playerTexture_ = PLAYER;
	// 3Dモデルの生成
	model_ = Model::CreateModelFromObj("engine/resources","block.obj");

	modelSkydome_ = Model::CreateModelFromObj("engine/resources/Skydome", "skydome.obj");

	// ワールドトランスフォームの初期化
	worldTransform_.Initialize();
	// ビュープロジェクションの初期化
	viewProjection_.Initialize();
	//viewProjection_.translation_ = { 0,0,-40 };
	worldTransform_.translation_.z = -10;

	// カメラレールの生成
	railCamera_ = new RailCamera();
	// カメラレールの初期化
	railCamera_->Initialize(worldTransform_, worldTransform_.rotation_);

	// 自キャラの生成
	player_ = new Player();

	// 初期位置
	Vector3 playerPosition(0, -2, 10);
	// 自キャラの初期化
	player_->Initialize(model_, playerTexture_, playerPosition);
	player_->SetGameScene(this);
	// 自キャラとレールカメラの親子関係を結ぶ
	player_->SetParent(&railCamera_->GetWorldTransform());

	// 天球
	skydome_ = new Skydome();
	skydome_->Initialize(modelSkydome_, { 0, 0, 0 });

	// 衝突マネージャーの生成
	collisionManager_ = new CollisionManager();
}

void GameScene::Update() {
	viewProjection_.UpdateMatrix();

	// デバッグカメラの更新
	railCamera_->Update();
	viewProjection_.matView = railCamera_->GetViewProjection().matView;
	viewProjection_.matProjection = railCamera_->GetViewProjection().matProjection;

	// 敵の出現するタイミングと座標
	UpdateEnemyPopCommands();
	// 敵の削除
	enemy_.remove_if([](Enemy* enemy) {
		if (enemy->IsDead()) {
			delete enemy;
			return true;
		}
		return false;
		});
	// enemyの更新
	for (Enemy* enemy : enemy_) {
		enemy->Update();
	}

	// 弾の更新
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Update();
	}
	// 終了した弾を削除
	enemyBullets_.remove_if([](EnemyBullet* bullet) {
		if (bullet->isDead()) {
			delete bullet;
			return true;
		}
		return false;
		});
	// 自キャラの更新
	player_->Update(viewProjection_);
	// 終了した弾を削除
	playerBullets_.remove_if([](PlayerBullet* bullet) {
		if (bullet->IsDead()) {
			delete bullet;
			return true;
		}
		return false;
		});
	// 弾の更新
	for (PlayerBullet* bullet : playerBullets_) {
		bullet->Update();
	}

	// 天球
	skydome_->Update();

	// 当たり判定を必要とするObjectをまとめてセットする
	collisionManager_->SetGameObject(player_, enemy_, enemyBullets_, playerBullets_);
	// 衝突マネージャー(当たり判定)
	collisionManager_->CheckAllCollisions(this, player_);

	// ビュープロジェクション行列の転送
	viewProjection_.TransferMatrix();
}

void GameScene::Draw() {
	// 自機
	player_->Draw(viewProjection_);
	// 自弾
	for (PlayerBullet* bullet : playerBullets_) {
		bullet->Draw(viewProjection_);
	}

	// 敵
	for (Enemy* enemy : enemy_) {
		enemy->Draw(viewProjection_);
	}
	// 敵弾
	for (EnemyBullet* bullet : enemyBullets_) {
		bullet->Draw(viewProjection_);
	}

	// 天球
	skydome_->Draw(viewProjection_);

	// 2Dレティクル
	player_->DrawUI();
}

void GameScene::Finalize() {
	// 3Dモデル
	delete model_;
	// 自キャラの解放
	delete player_;
	for (PlayerBullet* bullet : playerBullets_) {
		delete bullet;
	}
	// enemyの解放
	for (Enemy* enemy : enemy_) {
		delete enemy;
	}
	for (EnemyBullet* bullet : enemyBullets_) {
		delete bullet;
	}
	// 衝突マネージャーの解放
	delete collisionManager_;
	// 3Dモデル
	delete modelSkydome_;
	// 天球
	delete skydome_;
	// カメラレール
	delete railCamera_;
	// ビュー(カメラ)の解放
	viewProjection_.constBuff_.ReleaseAndGetAddressOf();

	worldTransform_.constBuff_.ReleaseAndGetAddressOf();
}

void GameScene::SpawnEnemy(Vector3 pos) {
	Enemy* enemy = new Enemy();
	// 自機の位置をもらう
	enemy->SetPlayer(player_);
	// 初期化
	enemy->SetGameScene(this);
	enemy->Initialize(model_, pos);

	// リストに登録
	enemy_.push_back(enemy);
}

void GameScene::LoadEnemyPopData() {
	// ファイルを開く
	std::ifstream file;
	file.open("resources/csv/enemyPop.csv");
	assert(file.is_open());

	// ファイルの内容を文字列ストリームにコピー
	enemyPopCommands_ << file.rdbuf();

	// ファイルを閉じる
	file.close();
}

void GameScene::UpdateEnemyPopCommands() {
	// 待機処理
	if (isWait_) {
		waitTime_--;
		if (waitTime_ <= 0) {
			// 待機完了
			isWait_ = false;
		}
		return;
	}

	// 1桁分の文字列を入れる変数
	std::string line;

	// コマンド実行ループ
	while (getline(enemyPopCommands_, line)) {
		// 1桁の文字列をストリームに変換して解析しやすくする
		std::istringstream line_stream(line);

		std::string word;
		// ,区切りで行の先頭文字列を取得
		getline(line_stream, word, ',');

		// "//"から始まる行はコメント
		if (word.find("//") == 0) {
			// コメント行を飛ばす
			continue;
		}

		// POPコマンド
		if (word.find("POP") == 0) {
			// x座標
			getline(line_stream, word, ',');
			float x = (float)std::atof(word.c_str());

			// y座標
			getline(line_stream, word, ',');
			float y = (float)std::atof(word.c_str());

			// z座標
			getline(line_stream, word, ',');
			float z = (float)std::atof(word.c_str());

			// 敵を発生させる
			SpawnEnemy(Vector3(x, y, z));
		}
		// WAITコマンド
		else if (word.find("WAIT") == 0) {
			getline(line_stream, word, ',');

			// 待ち時間
			int32_t waitTime = atoi(word.c_str());

			// 待ち時間
			isWait_ = true;
			waitTime_ = waitTime;

			// コマンドループを抜ける
			break;
		}
	}
}

void GameScene::AddPlayerBullet(PlayerBullet* playerBullet) {
	// リストに登録する
	playerBullets_.push_back(playerBullet);
}

void GameScene::AddEnemyBullet(EnemyBullet* enemyBullet) {
	// リストに登録する
	enemyBullets_.push_back(enemyBullet);
}