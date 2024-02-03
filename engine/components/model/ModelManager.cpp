#include "ModelManager.h"
#include "WinApp.h"

ModelManager* ModelManager::GetInstance() {
	static ModelManager instance;

	return &instance;
}

void ModelManager::Initialize() {

}

void ModelManager::LoadModel(const std::string& filePath) {
	// 読み込み済みモデルを検索
	if (models_.contains(filePath)) {
		// 読み込み済みなら
		return;
	}

	// モデル生成とファイル読み込み
	std::unique_ptr<Model> model;
	model.reset(Model::CreateModelFromObj("engine/resources", filePath));
	//model->Initialize("engine/resources", filePath);

	// モデルをmapコンテナに格納
	models_.insert(std::make_pair(filePath, std::move(model)));
}

void ModelManager::Finalize() {
	//delete instance;
}

Model* ModelManager::FindModel(const std::string& filePath) {
	// 読み込み済みモデルを検索
	if (models_.contains(filePath)) {
		// 読み込み済みモデルを返す
		return models_.at(filePath).get();
	}

	// 該当ファイルなし
	WinApp::Log("Not ModelFile\n");
	return nullptr;
}

Model* ModelManager::SetModel(const std::string& filePath) {
	//Model* model = new Model();
	//model = FindModel(filePath);
	return ModelManager::GetInstance()->FindModel(filePath);
}