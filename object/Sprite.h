#pragma once
#include "../math/Matrix4x4.h"
#include "../VertexData.h"
#include "../Transform.h"
#include "../TransformationMatrix.h"
#include "../base/DirectXCommon.h"
#include "../base/WinApp.h"
#include "../base/ViewProjection.h"
#include "../base/WorldTransform.h"
#include "../Material.h"
#include <d3d12.h>
#include <wrl.h>

class Sprite
{
public:
	Sprite() = default;
	~Sprite() = default;

	/// <summary>
	/// インスタンスの生成
	/// </summary>
	/// <param name="size">画像の縦幅、横幅</param>
	static Sprite* Create(Vector2 size, int textureNum);

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="size">画像の縦幅、横幅</param>
	void Initialize(Vector2 size,int textureNum);

	//void update();

	/// <summary>
	/// 描画処理
	/// </summary>
	/// <param name="size">縦幅、横幅を入力</param>
	/// <param name="textureNum">textureManagerで登録したenum型の番号を入れる</param>
	void Draw(int textureNum);

	// 解放処理
	void Release();

	/// 
	/// User Method
	/// 
	// 調整項目
	void ImGuiAdjustParameter();

	/// Getter
	// 縦幅、横幅
	Vector2 GetSize() { return size_; }
	// 座標
	Vector2 GetPos() { return { worldTransform_.translation_.x, worldTransform_.translation_.y }; }
	// Spriteのアンカーポイント
	Vector2 GetAnchorPoint() { return anchorPoint_; }
	// 画像の切り出しサイズ
	Vector2 GetTextureSize() { return textureSize_; }

	/// Setter
	// 縦幅、横幅
	void SetSize(Vector2 size) { size_ = size; }
	// 座標
	void SetPos(Vector2 pos) {
		worldTransform_.translation_.x = pos.x;
		worldTransform_.translation_.y = pos.y;
	}
	// Spriteのアンカーポイント
	void SetAnchorPoint(Vector2 anchorPoint) { anchorPoint_ = anchorPoint; }
	// 画像の切り出しサイズ
	void SetTextureSize(Vector2 textureSize) { textureSize_ = textureSize; }

	// 画像の切りだしサイズのリセット
	void ResetTextureSize() { textureSize_ = { 512.0f,512.0f }; }

	// テクスチャのサイズをスプライトに合わせる
	void AdjustTextureSize();

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes);

	void CreateVertexResource();

	void CreateVertexBufferView();

	void CreateIndexResource();

	void CreateIndexBufferView();

	void CreateMaterialResource();

private:
	TextureManager* textureManager_;

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
	WorldTransform worldTransform_;
	ViewProjection viewProjection_;

	// 画像の中心点
	Vector2 anchorPoint_ = { 0.5f,0.5f };

	// スプライトの縦幅、横幅
	Vector2 size_;

	// 画像の左上の座標
	Vector2 textureLeftTop_ = { 0.0f,0.0f };
	// テクスチャの切り出しサイズ
	Vector2 textureSize_ = { 512.0f,512.0f };

	uint32_t textureindex_;
};

