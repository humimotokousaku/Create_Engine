#pragma once
#include "Sprite.h"
#include "DirectXCommon.h"
#include "Camera.h"
#include "PostEffectPSO.h"
#include <Windows.h>

class PostEffect {
private:// 構造体
	struct RenderingTextureData {
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		uint32_t srvIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;
	};

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	PostEffect();
	
	//static PostEffect* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新処理
	/// </summary>
	//void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(uint32_t psoNum);

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDrawScene();

	/// <summary>
	/// 描画後の処理
	/// </summary>
	void PostDrawScene();

	/// <summary>
	/// スプライトの初期化(現状のSpriteクラスだとテクスチャの読み込みをしなくてはいけないので新たに作る)
	/// </summary>
	void SpriteInitialize(RenderingTextureData texData);

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);

#pragma region Getter
	// ブラーの情報を取得
	BlurData* GetBlurData() { return blurData_; }
	// 高輝度の情報を取得
	HighIntensityData* GetHighIntensityData() { return highIntensityData_; }
#pragma endregion

#pragma region Setter
	// ブラーの情報を設定
	void SetBlurData(BlurData* blurData) { blurData_ = blurData; }
	// 高輝度の情報を設定
	void SetHighIntensityData(HighIntensityData* highIntensityData) { highIntensityData_ = highIntensityData; }
#pragma endregion

private:// プライベートなメンバ関数
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes);

	void CreateVertexResource();

	void CreateVertexBufferView();

	void CreateIndexResource();

	void CreateIndexBufferView();

	void CreateMaterialResource();

	// テクスチャを作成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureBufferResource();

	// SRVの作成
	void CreateSRV();

	// RTVの作成
	void CreateRTV();

	// バリアを張る
	void SetBarrier(Microsoft::WRL::ComPtr<ID3D12Resource> texBuff, D3D12_RESOURCE_STATES beforeState, D3D12_RESOURCE_STATES afterState);

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateRTVDescriptorHeap(const Microsoft::WRL::ComPtr<ID3D12Device>& device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors, bool shaderVisible);




	// RenderTextureの生成
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor);


public:// 定数
	// 画面クリアカラー
	static const float clearColor_[4];

	// 現在は赤色に設定
	const Vector4 kRenderTargetClearValue = { 1.0f,0.0f,0.0f,1.0f };

public:// プライベートな変数
	WorldTransform worldTransform_;

private:// パブリックな変数
	// 基本機能
	DirectXCommon* directXCommon_;
	PostEffectPSO* postEffectPSO_;
	SrvManager* srvManager_;

	// テクスチャバッファ
	RenderingTextureData texBuff_;
	// 高輝度テクスチャ
	//RenderingTextureData highIntensityTexBuff_;

	// 深度バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuff_;
	// RTV用のデスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapRTV_;
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_;
	// DSV用のデスクリプタヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapDSV_;

	// ブラーの情報
	BlurData* blurData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> blurResource_;
	// 高輝度の情報
	HighIntensityData* highIntensityData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> highIntensityResource_;

#pragma region スプライト
	// カメラ
	std::unique_ptr<Camera> camera_;
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraPosResource_;
	Vector3 cameraPosData_;

	// Material
	Material* materialData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Transform uvTransform_;
	Matrix4x4 uvTransformMatrix_;
	// Vertex
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	VertexData* vertexData_;
	// index
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	D3D12_INDEX_BUFFER_VIEW indexBufferView_;
	uint32_t* indexData_ = nullptr;
	// Sprite

	ViewProjection viewProjection_;

	// 画像の中心点
	Vector2 anchorPoint_ = { 0.5f,0.5f };

	// スプライトの縦幅、横幅
	Vector2 size_;

	// 画像の左上の座標
	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	// テクスチャの切り出しサイズ
	Vector2 textureSize_;

	uint32_t textureIndex_;
#pragma endregion
};

