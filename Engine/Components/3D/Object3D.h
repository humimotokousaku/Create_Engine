#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include "Camera.h"
#include "ModelManager.h"

class Object3D
{
public:
	///
	/// Default Method
	/// 
	
	// コンストラクタ
	Object3D() = default;
	// デストラクタ
	~Object3D();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	//void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="viewProjection">カメラ</param>
	/// <param name="textureNum">テクスチャ番号</param>
	void Draw(uint32_t textureNum);
	/// <summary>
	/// 描画処理(モデルで使われているテクスチャを使用する)
	/// </summary>
	void Draw();

	///
	/// Default Method
	/// 

	void ImGuiParameter(const char* name);

	/// Getter


	/// Setter
	// カメラ
	void SetCamera(Camera* camera) { camera_ = camera; }

	// モデルのセット
	void SetModel(const std::string& directoryPath, const std::string& filePath) { 
		model_ = ModelManager::GetInstance()->FindModel(directoryPath, filePath);
		// アニメーション
		animation_ = model_->animation_;
		// スケルトン
		skeleton_ = model_->skeleton_;
		// スキンクラスタ
		skinCluster_ = model_->skinCluster_;
	}
	void SetModel(Model* model) {
		model_ = model;
		// アニメーション
		animation_ = model_->animation_;
		// スケルトン
		skeleton_ = model_->skeleton_;
		// スキンクラスタ
		skinCluster_ = model_->skinCluster_;
	}

public:
	WorldTransform worldTransform;
	Camera* camera_;
	Model* model_;
	// アニメーション
	Motion animation_;
	float animationTime_ = 0.0f;
private:
	// スケルトン
	Skeleton skeleton_;
	// スキンクラスタ
	SkinCluster skinCluster_;
};