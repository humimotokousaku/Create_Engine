#include "Model.h"
#include "ImGuiManager.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "TextureManager.h"
#include "PipelineManager.h"
#include  "ModelManager.h"
#include <cassert>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

void Model::Initialize(const std::string& directoryPath, const std::string& filename) {
	// エンジン機能のインスタンスを入れる
	dxCommon_ = DirectXCommon::GetInstance();
	texManager_ = TextureManager::GetInstance();

	// モデルの読み込み
	modelData_ = LoadModelFile(directoryPath, filename);
	// アニメーションデータを読み込む
	animation_ = LoadAnimationFile(directoryPath, filename);
	// スケルトンデータを作成
	skeleton_ = CreateSkeleton(modelData_.rootNode);

	// モデルファイルと同じ階層にテクスチャがない場合、デフォルトのテクスチャが入るようにする
	texHandle_ = 1;
	// テクスチャ読み込み 
	texManager_->LoadTexture(modelData_.material.textureFilePath);
	texHandle_ = texManager_->GetSrvIndex(modelData_.material.textureFilePath);

	// 頂点データのリソース作成
	CreateVertexResource();
	CreateVertexBufferView();
	// 書き込むためのアドレスを取得
	vertexResource_->Map(0, nullptr, reinterpret_cast<void**>(&vertexData_));
	std::memcpy(vertexData_, modelData_.vertices.data(), sizeof(VertexData) * modelData_.vertices.size());

	// インデックスのリソース作成
	indexResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(uint32_t) * modelData_.indices.size());
	indexBufferView_.BufferLocation = indexResource_.Get()->GetGPUVirtualAddress();
	indexBufferView_.SizeInBytes = sizeof(uint32_t) * (uint32_t)modelData_.indices.size();
	indexBufferView_.Format = DXGI_FORMAT_R32_UINT;
	// 書き込むためのアドレスを取得
	indexResource_->Map(0, nullptr, reinterpret_cast<void**>(&mappedIndex_));
	std::memcpy(mappedIndex_, modelData_.indices.data(), sizeof(uint32_t) * (uint32_t)modelData_.indices.size());
	*mappedIndex_ = modelData_.indices.size();

	// マテリアルデータのリソース作成
	CreateMaterialResource();

	// カメラ
	// 1つ分のサイズを用意する
	cameraPosResource_ = CreateBufferResource(dxCommon_->GetDevice(), sizeof(Vector3)).Get();
	// 書き込むためのアドレスを取得
	cameraPosResource_->Map(0, nullptr, reinterpret_cast<void**>(&cameraPosData_));

	// Lightingするか
	materialData_->enableLighting = false;
	materialData_->color = { 1.0f,1.0f,1.0f,1.0f };
	// uvTransform行列の初期化
	materialData_->uvTransform = MakeIdentity4x4();
	uvTransform_ = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,0.0f}
	};
}

void Model::Draw(const ViewProjection& viewProjection, uint32_t textureHandle) {
#pragma region アニメーション
	animationTime_ += 1.0f / 60.0f;
	animationTime_ = std::fmod(animationTime_, animation_.duration);
	//NodeAnimation& rootNodeAnimation = animation_.nodeAnimations[modelData_.rootNode.name];
	//Vector3 translate = CalculateTranslateValue(rootNodeAnimation.translate.keyframes, animationTime_);
	//Quaternion rotate = CalculateQuaternionValue(rootNodeAnimation.rotate.keyframes, animationTime_);
	//Vector3 scale = { 1,1,1 };
	//modelData_.rootNode.localMatrix = MakeAffineMatrix(scale, rotate, translate);

	// スケルトンに対してアニメーションを適用
	ApplyAnimation(skeleton_, animation_, animationTime_);
	// 骨の更新処理
	SkeletonUpdate(skeleton_);

#pragma endregion

	// 形状を設定
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);

	/// CBVの設定

	// viewProjection
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, cameraPosResource_.Get()->GetGPUVirtualAddress());

	/// DescriptorTableの設定
	// texture
	SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(2, textureHandle);
	// material
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());
	// ライティング
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, DirectionalLight::GetInstance()->GetDirectionalLightResource()->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, PointLight::GetInstance()->GetPointLightResource()->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(7, SpotLight::GetInstance()->GetSpotLightResource()->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1, 0, 0, 0);
}

void Model::Draw(const ViewProjection& viewProjection) {
#pragma region アニメーション
	animationTime_ += 1.0f / 60.0f;
	animationTime_ = std::fmod(animationTime_, animation_.duration);
	//NodeAnimation& rootNodeAnimation = animation_.nodeAnimations[modelData_.rootNode.name];
	//Vector3 translate = CalculateTranslateValue(rootNodeAnimation.translate.keyframes, animationTime_);
	//Quaternion rotate = CalculateQuaternionValue(rootNodeAnimation.rotate.keyframes, animationTime_);
	//Vector3 scale = { 1,1,1 };
	//modelData_.rootNode.localMatrix = MakeAffineMatrix(scale, rotate, translate);

	// スケルトンに対してアニメーションを適用
	ApplyAnimation(skeleton_, animation_, animationTime_);
	// 骨の更新処理
	SkeletonUpdate(skeleton_);

#pragma endregion

	// 形状を設定
	DirectXCommon::GetInstance()->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView_); // VBVを設定
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView_);

	/// CBVの設定

	// viewProjection
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(4, viewProjection.constBuff_->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(5, cameraPosResource_.Get()->GetGPUVirtualAddress());

	/// DescriptorTableの設定
	// texture
	SrvManager::GetInstance()->SetGraphicsRootDesctiptorTable(2, texHandle_);
	// material
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource_.Get()->GetGPUVirtualAddress());
	// ライティング
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, DirectionalLight::GetInstance()->GetDirectionalLightResource()->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(6, PointLight::GetInstance()->GetPointLightResource()->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(7, SpotLight::GetInstance()->GetSpotLightResource()->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->DrawIndexedInstanced(UINT(modelData_.indices.size()), 1, 0, 0, 0);
}

void Model::AdjustParameter() {
	ImGui::Begin("Model");
	//ImGui::DragFloat3("translation", worldtrans)
	ImGui::End();
}

#pragma region プライベートな関数

Microsoft::WRL::ComPtr<ID3D12Resource> Model::CreateBufferResource(const Microsoft::WRL::ComPtr<ID3D12Device>& device, size_t sizeInBytes) {
	HRESULT hr;
	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // UploadHeapを使う
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	// バッファソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes; // リソースのサイズ。今回はVector4を3頂点分
	// バッファの場合はこれからは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	// 実際に頂点リソースを作る
	hr = device.Get()->CreateCommittedResource(&uploadHeapProperties, D3D12_HEAP_FLAG_NONE,
		&vertexResourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));

	return vertexResource;
}

void Model::CreateVertexResource() {
	vertexResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(VertexData) * modelData_.vertices.size()).Get();
}

void Model::CreateVertexBufferView() {
	// リソースの先頭のアドレスから使う
	vertexBufferView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vertexBufferView_.SizeInBytes = sizeof(VertexData) * UINT(modelData_.vertices.size());
	// 1頂点当たりのサイズ
	vertexBufferView_.StrideInBytes = sizeof(VertexData);
}

void Model::CreateMaterialResource() {
	materialResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(Material)).Get();
	// マテリアルにデータを書き込む
	materialData_ = nullptr;
	// 書き込むためのアドレスを取得
	materialResource_->Map(0, nullptr, reinterpret_cast<void**>(&materialData_));
}

ModelData Model::LoadModelFile(const std::string& directoryPath, const std::string& filename) {
	ModelData modelData;
	Assimp::Importer importer;
	std::string filePath = "Engine/resources/" + directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), aiProcess_FlipWindingOrder | aiProcess_FlipUVs);
	assert(scene->HasMeshes());

	for (uint32_t meshIndex = 0; meshIndex < scene->mNumMeshes; ++meshIndex) {
		aiMesh* mesh = scene->mMeshes[meshIndex];
		assert(mesh->HasNormals());
		assert(mesh->HasTextureCoords(0));
		// 頂点数分のメモリを確保
		//modelData.vertices.resize(mesh->mNumVertices);
		for (uint32_t vertexIndex = 0; vertexIndex < mesh->mNumVertices; ++vertexIndex) {
			aiVector3D& position = mesh->mVertices[vertexIndex];
			aiVector3D& normal = mesh->mNormals[vertexIndex];
			aiVector3D& texcoord = mesh->mTextureCoords[0][vertexIndex];
			VertexData vertex;
			vertex.position = { -position.x, position.y, position.z, 1.0f };
			vertex.normal = { -normal.x, normal.y, normal.z };
			vertex.texcoord = { texcoord.x, texcoord.y };

			//modelData.vertices[vertexIndex].position = { -position.x, position.y, position.z, 1.0f };
			//modelData.vertices[vertexIndex].normal = { -normal.x, normal.y, normal.z };
			//modelData.vertices[vertexIndex].texcoord = { texcoord.x, texcoord.y };


			modelData.vertices.push_back(vertex);
		}
		// 面からindexの解析
		for (uint32_t faceIndex = 0; faceIndex < mesh->mNumFaces; ++faceIndex) {
			aiFace& face = mesh->mFaces[faceIndex];
			assert(face.mNumIndices == 3);
			// 頂点を解析
			for (uint32_t element = 0; element < face.mNumIndices; ++element) {
				uint32_t vertexIndex = face.mIndices[element];
				modelData.indices.push_back(vertexIndex);
			}
		}
	}
	modelData;
	// マテリアルの解析
	for (uint32_t materialIndex = 0; materialIndex < scene->mNumMaterials; ++materialIndex) {
		aiMaterial* material = scene->mMaterials[materialIndex];
		if (material->GetTextureCount(aiTextureType_DIFFUSE) != 0) {
			aiString textureFilePath;
			material->GetTexture(aiTextureType_DIFFUSE, 0, &textureFilePath);
			modelData.material.textureFilePath = "Engine/resources/" + directoryPath + "/" + textureFilePath.C_Str();
		}
	}

	// シーンの階層構造を作成
	modelData.rootNode = ReadNode(scene->mRootNode);

	return modelData;
}

#pragma  endregion