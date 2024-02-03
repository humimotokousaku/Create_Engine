#include "TitleScene.h"
#include "ImGuiManager.h"
#include "ModelManager.h"

void TitleScene::Initialize() {
	sceneNum = TITLE_SCENE;
	input_ = Input::GetInstance();
	viewProjection_.Initialize();

	// objモデル
	//ModelManager::GetInstance()->LoadModel("plane.obj");
	//plane_ = new Object3D();
	//plane_->Initialize("plane.obj");

	//ModelManager::GetInstance()->LoadModel("teapot.obj");
	//axis_ = new Object3D();
	//axis_->Initialize("teapot.obj");

	// Sprite
	//sprite_ = Sprite::Create(UVCHEKER);

	// particle
	//particles_ = new Particles();
	//particles_->Initialize();
}

void TitleScene::Update() {
	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		sceneNum = GAME_SCENE;
	}

	viewProjection_.UpdateViewMatrix();
	viewProjection_.TransferMatrix();

	// パーティクルの発生位置を指定
	//particles_->SetEmitterPos(Vector3{ 5,5,0 });
	// パーティクルの更新処理
	//particles_->Update();

	ImGui::Begin("Camera");
	ImGui::DragFloat3("translation", &viewProjection_.translation_.x, 0.1f, -100, 100);
	ImGui::DragFloat3("rotation", &viewProjection_.rotation_.x, 0.01f, -6.28f, 6.28f);
	ImGui::End();
	//ImGui::Begin("plane");
	//ImGui::DragFloat3("translation", &plane_->worldTransform.translation_.x, 0.01f, -100, 100);
	//ImGui::End();
	//ImGui::Begin("axis");
	//ImGui::DragFloat3("translation", &axis_->worldTransform.translation_.x, 0.01f, -100, 100);
	//ImGui::End();

	//sprite_->ImGuiAdjustParameter();

	ImGui::Begin("Current Scene");
	ImGui::Text("TITLE");
	ImGui::Text("SPACE:scene change");
	ImGui::End();
}

void TitleScene::Draw() {
	//axis_->Draw(viewProjection_, UVCHEKER);
	//plane_->Draw(viewProjection_, UVCHEKER);
	//particles_->Draw(viewProjection_, PARTICLE);
	//sprite_->Draw();
}

void TitleScene::Finalize() {
	//delete particles_;
	//delete sprite_;
	//delete axis_;
	//delete plane_;
	viewProjection_.constBuff_.ReleaseAndGetAddressOf();
}