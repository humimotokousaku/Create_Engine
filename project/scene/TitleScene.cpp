#include "TitleScene.h"
#include "ImGuiManager.h"
#include "ModelManager.h"
#include "GlobalVariables.h"
#include "Lerp.h"

void TitleScene::Initialize() {
	sceneNum = TITLE_SCENE;
	input_ = Input::GetInstance();

	// カメラの初期化
	camera_ = std::make_unique<Camera>();
	camera_->Initialize();
	camera_->SetTranslate(Vector3{ 0,5,-20 });

	// テクスチャの読み込み
	TextureManager::GetInstance()->LoadTexture("","uvChecker.png");
	TextureManager::GetInstance()->LoadTexture("","monsterBall.png");
	TextureManager::GetInstance()->LoadTexture("","circle.png");
	// srvの番号取得
	uvcheckerTexture_ = TextureManager::GetInstance()->GetSrvIndex(  "","uvChecker.png");
	monsterBallTexture_ = TextureManager::GetInstance()->GetSrvIndex("","monsterBall.png");
	particleTexture_ = TextureManager::GetInstance()->GetSrvIndex(   "","circle.png");
	// objモデル
	ModelManager::GetInstance()->LoadModel("SimpleSkin", "simpleSkin.gltf");
	ModelManager::GetInstance()->LoadModel("", "testPlane.gltf");
	ModelManager::GetInstance()->LoadModel("Human","walk.gltf");
	ModelManager::GetInstance()->LoadModel("Human", "sneakWalk.gltf");
	ModelManager::GetInstance()->LoadModel("AnimatedCube", "AnimatedCube.gltf");

	ModelManager::GetInstance()->LoadModel("", "axis.obj");
	ModelManager::GetInstance()->LoadModel("", "block.obj");

	// 平面(スケルトンなしのgltf)
	plane_ = std::make_unique<Object3D>();
	plane_->Initialize();
	plane_->SetModel("AnimatedCube", "AnimatedCube.gltf");
	plane_->SetCamera(camera_.get());
	plane_->worldTransform.transform.translate = { -10,0,0 };

	// 人間(スケルトンありのgltf)
	human_ = std::make_unique<Object3D>();
	human_->Initialize();
	human_->SetModel("Human", "walk.gltf");
	//human_->SetModel("AnimatedCube/AnimatedCube.gltf");
	human_->SetCamera(camera_.get());
	human_->worldTransform.transform.translate = { 0,0,0 };
	//human_->model_->materialData_->color = { 1,1,1,0.0f };

	// 3D線(obj読み込み)
	axis_ = std::make_unique<Object3D>();
	axis_->Initialize();
	axis_->SetModel("Human", "sneakWalk.gltf");
	axis_->SetCamera(camera_.get());
	axis_->worldTransform.transform.translate = { 10,0,0 };

	// アニメーション
	//anim_ = std::make_unique<Animation>();
	//anim_->SetAnimData(&plane_->worldTransform.transform.translate, Vector3{ 0,0,0 }, Vector3{ 10,0,0 }, 60, "PlaneAnim0", Easings::EaseOutBack);
}

void TitleScene::Update() {
#pragma region パーティクル以外の処理
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		sceneNum = GAME_SCENE;
	}

#ifdef _DEBUG
	ImGui::Begin("plane");
	ImGui::DragFloat3("translation", &plane_->worldTransform.transform.translate.x, 0.01f, -100, 100);
	ImGui::DragFloat3("scale", &plane_->worldTransform.transform.scale.x, 0.01f, -100, 100);
	ImGui::DragFloat3("rotate", &plane_->worldTransform.transform.rotate.x, 0.01f, -6.28f, 6.28f);
	ImGui::End();
	ImGui::Begin("axis");
	ImGui::DragFloat3("translation", &axis_->worldTransform.transform.translate.x, 0.01f, -100, 100);
	ImGui::DragFloat3("scale", &axis_->worldTransform.transform.scale.x, 0.01f, -100, 100);
	ImGui::DragFloat3("rotate", &axis_->worldTransform.transform.rotate.x, 0.01f, -6.28f, 6.28f);
	ImGui::End();
	ImGui::Begin("human");
	ImGui::DragFloat3("translation", &human_->worldTransform.transform.translate.x, 0.01f, -100, 100);
	ImGui::DragFloat3("scale", &human_->worldTransform.transform.scale.x, 0.01f, -100, 100);
	ImGui::DragFloat3("rotate", &human_->worldTransform.transform.rotate.x, 0.01f, -6.28f, 6.28f);
	ImGui::End();

	ImGui::Begin("Current Scene");
	ImGui::Text("TITLE");
	ImGui::Text("SPACE:scene change");
	ImGui::End();
#endif


#pragma endregion
}

void TitleScene::Draw() {
	plane_->Draw();
	axis_->Draw();
	human_->Draw();
}

void TitleScene::Finalize() {

}