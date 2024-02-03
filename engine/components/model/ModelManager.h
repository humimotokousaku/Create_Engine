#pragma once
#include "Model.h"
#include <memory>
#include <map>

class ModelManager {
public:
	// シングルトンインスタンスの取得
	static ModelManager* GetInstance();

	// 初期化
	void Initialize();

	void LoadModel(const std::string& filePath);

	// 終了
	void Finalize();

	/// 
	/// User Method
	/// 

	/// Getter

	/// <summary>
	/// モデルの検索
	/// </summary>
	/// <param name="filePath">モデルのファイルパス</param>
	/// <returns>モデル</returns>
	Model* FindModel(const std::string& filePath);

	/// Setter

	// モデルをセット
	Model* SetModel(const std::string& filePath);

private:
	ModelManager() = default;
	~ModelManager() = default ;
	ModelManager(const ModelManager& obj) = default;
	ModelManager& operator = (const ModelManager& obj) = default;

private:
	// モデルデータ
	std::map<std::string, std::unique_ptr<Model>> models_;
};

