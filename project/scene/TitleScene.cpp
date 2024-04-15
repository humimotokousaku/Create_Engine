#include "TitleScene.h"
#include "ImGuiManager.h"
#include "ModelManager.h"
#include "GlobalVariables.h"
#include "Lerp.h"

void TitleScene::Initialize() {
	sceneNum = TITLE_SCENE;
	input_ = Input::GetInstance();

	// テクスチャの読み込み
	TextureManager::GetInstance()->LoadTexture("Engine/resources/uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("Engine/resources/monsterBall.png");
	TextureManager::GetInstance()->LoadTexture("Engine/resources/circle.png");
	// srvの番号取得
	uvcheckerTexture_ = TextureManager::GetInstance()->GetSrvIndex("Engine/resources/uvChecker.png");
	monsterBallTexture_ = TextureManager::GetInstance()->GetSrvIndex("Engine/resources/monsterBall.png");
	particleTexture_ = TextureManager::GetInstance()->GetSrvIndex("Engine/resources/circle.png");
	// objモデル
	//ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("Human/walk.gltf");
	//ModelManager::GetInstance()->LoadModel("testPlane.gltf");
	//ModelManager::GetInstance()->LoadModel("axis.obj");
	//ModelManager::GetInstance()->LoadModel("block.obj");

	// カメラの初期化
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->SetTranslate(Vector3{ 0,5,-20 });

	/// particle

	particles_ = std::make_unique<Particles>();
	particles_->Initialize();
	particles_->SetCamera(camera_.get());
	// パーティクルの発生位置を指定
	particles_->SetEmitterPos(Vector3{ 5,5,0 });

	particles_1 = std::make_unique<Particles>();
	particles_1->Initialize();
	particles_1->SetCamera(camera_.get());
	// パーティクルの発生位置を指定
	particles_1->SetEmitterPos(Vector3{ 0,0,0 });

#pragma region パーティクル以外の処理
	// Sprite
	sprite_.reset(Sprite::Create("Engine/resources/uvChecker.png"));
	sprite_->SetIsBackGround();
	Vector4 color = { 0,0,0,1 };
	sprite_->SetColor(color);
	sprite_->SetPos(Vector2{ 640,360 });
	sprite_->SetSize(Vector2{ 1280,720 });

	sprite_2.reset(Sprite::Create("Engine/resources/monsterBall.png"));




	// 平面
	plane_ = std::make_unique<Object3D>();
	plane_->Initialize();
	plane_->SetModel("Human/walk.gltf");
	plane_->SetCamera(camera_.get());
	plane_->model_->materialData_->color = { 1,1,1,1 };

	// アニメーション
	anim_ = std::make_unique<Animation>();
	anim_->SetAnimData(&plane_->worldTransform.transform.translate, Vector3{ 0,0,0 }, Vector3{ 10,0,0 }, 60, "PlaneAnim0", Easings::EaseOutBack);

#pragma endregion
}

void TitleScene::Update() {
	// パーティクルの更新処理
	particles_->Update();
	// パーティクルの更新処理
	particles_1->Update();

#pragma region パーティクル以外の処理
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		sceneNum = GAME_SCENE;
		//anim_->SetIsStart(true);
	}

	//anim_->Update();

	//player_->Update();

	for (int i = 0; i < 2; i++) {
		//enemy_[i]->Update();
		// 狙う対象に身体を向ける
		//float radian = atan2(player_->model_->worldTransform.transform.translate.x - enemy_[i]->model_->worldTransform.transform.translate.x, player_->model_->worldTransform.transform.translate.z - enemy_[i]->model_->worldTransform.transform.translate.z);
		//enemy_[i]->model_->worldTransform.transform.rotate.y = radian;
	}

	// 当たり判定
	//collisionManager_->CheckAllCollisions();

	ImGui::Begin("axis");
	//ImGui::ColorEdit4("color", &axis_->model_->materialData_->color.x);
	ImGui::End();

#ifdef _DEBUG
	ImGui::Begin("Animation");
	//ImGui::Text("isStart:%d", anim_->GetIsStart());
	ImGui::End();

	//ImGui::Begin("Camera");
	//ImGui::DragFloat3("translation", &camera_->GetViewProjection().transform.translate.x, 0.1f, -100, 100);
	//ImGui::DragFloat3("rotation", &camera_->viewProjection_.rotation_.x, 0.01f, -6.28f, 6.28f);
	//ImGui::End();
	ImGui::Begin("plane");
	ImGui::DragFloat3("translation", &plane_->worldTransform.transform.translate.x, 0.01f, -100, 100);
	ImGui::DragFloat3("scale", &plane_->worldTransform.transform.scale.x, 0.01f, -100, 100);
	ImGui::DragFloat3("rotate", &plane_->worldTransform.transform.rotate.x, 0.01f, -6.28f, 6.28f);
	ImGui::End();
	//ImGui::Begin("axis");
	//ImGui::DragFloat3("translation", &axis_->worldTransform.transform.translate.x, 0.01f, -100, 100);
	//ImGui::DragFloat3("scale", &axis_->worldTransform.transform.scale.x, 0.01f, -100, 100);
	//ImGui::DragFloat3("rotate", &axis_->worldTransform.transform.rotate.x, 0.01f, -6.28f, 6.28f);
	//ImGui::End();

	//sprite_->ImGuiAdjustParameter();

	ImGui::Begin("Particles");
	//if (ImGui::TreeNode("Particle")) {
	//	particles_->ImGuiAdjustParameter();
	//	ImGui::TreePop();
	//}
	//if (ImGui::TreeNode("Particle1")) {
	//	particles_1->ImGuiAdjustParameter();
	//	ImGui::TreePop();
	//}


	ImGui::End();

	ImGui::Begin("Current Scene");
	ImGui::Text("TITLE");
	ImGui::Text("SPACE:scene change");
	ImGui::End();
#endif


#pragma endregion
}

void TitleScene::Draw() {

#pragma region パーティクル以外の処理
	//axis_->Draw(uvcheckerTexture_);
	plane_->Draw(uvcheckerTexture_);
	//testWater_->Draw(world_, viewProjection_);
	//player_->Draw(uvcheckerTexture_);
	for (int i = 0; i < 2; i++) {
		//enemy_[i]->Draw(monsterBallTexture_);
	}
	sprite_->Draw();
	//sprite_2->Draw();
#pragma endregion

	//particles_->Draw(particleTexture_);
	//particles_1->Draw(particleTexture_);
}

void TitleScene::Finalize() {

}