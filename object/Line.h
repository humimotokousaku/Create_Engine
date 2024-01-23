#pragma once
#include "../math/Matrix4x4.h"
#include "../VertexData.h"
#include "../Transform.h"
#include "../TransformationMatrix.h"
#include "../base/DirectXCommon.h"
#include "../base/WorldTransform.h"
#include "../base/ViewProjection.h"
#include <d3d12.h>
#include <wrl.h>

class Line
{
public:
	/// 
	/// Default Method
	/// 

	// コンストラクタ
	Line() = default;
	// デストラクタ
	~Line() = default;
	// 初期化
	void Initialize();
	// 描画処理
	void Draw(Vector3 start, Vector3 end, const ViewProjection& viewProjection);
	// 解放処理
	//void Release();

	///
	///	User Method
	/// 

	// ImGuiをまとめた関数
	void ImGuiAdjustParameter();

	/// Getter


	/// Setter
	void SetRGBA(Vector4 rgba) { *materialData_ = rgba; }

private:// プライベートなメンバ関数
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes);

	void CreateVertexResource();

	void CreateVertexBufferView();

	void CreateMaterialResource();

private:
	// Material
	Vector4* materialData_;
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	// 頂点情報
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	VertexData* vertexData_;
public:
	// ワールド座標
	WorldTransform worldTransform;
};
