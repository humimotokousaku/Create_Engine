#pragma once
#include "MathStructs.h"
#include "ModelStructs.h"
#include "WorldTransform.h"
#include "ViewProjection.h"
#include <d3d12.h>

class Model
{
public:
	Model() = default;
	~Model() = default;

	void Initialize(const std::string& directoryPath, const std::string& filename);

	void Draw(const ViewProjection& viewProjection, uint32_t textureHandle);

	void AdjustParameter();

	///
	/// User Method
	/// 

	/// Getter
	// モデルデータ
	ModelData GetModelData() { return modelData_; }

	/// Setter
	void SetIsLighting(bool isActive) { materialData_->enableLighting = isActive; }

private:
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes);

	void CreateVertexResource();

	void CreateVertexBufferView();

	void CreateMaterialResource();

	ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

public:
	// ワールド座標
	//WorldTransform worldTransform;
	// Material
	Material* materialData_;
private:

	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource_;
	Transform uvTransform_;
	Matrix4x4 uvTransformMatrix_;
	// Vertex
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView_;
	VertexData* vertexData_;

	ModelData modelData_;

	// カメラ
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraPosResource_;
	Vector3 cameraPosData_;
};

