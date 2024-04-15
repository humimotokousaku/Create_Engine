#include "ModelStructs.h"
#include <cassert>
#include <assimp/Importer.hpp>

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename) {
	MaterialData materialData;
	std::string line;
	std::ifstream file(directoryPath + "/" + filename);
	assert(file.is_open());

	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier;

		// identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// 連結してファイルパスをする
			materialData.textureFilePath = directoryPath + "/" + textureFilename;
		}
	}

	return materialData;
}

Motion LoadAnimationFile(const std::string& directoryPath, const std::string& filename) {
	Motion animation;
	Assimp::Importer importer;
	std::string filePath = directoryPath + "/" + filename;
	const aiScene* scene = importer.ReadFile(filePath.c_str(), 0);
	assert(scene->mAnimations != 0);

	aiAnimation* animationAssimp = scene->mAnimations[0];
	animation.duration = float(animationAssimp->mDuration / animationAssimp->mTicksPerSecond); // 時間の単位を秒に変換

	// それぞれのNodeAnimationの情報を取得
	for (uint32_t channelIndex = 0; channelIndex < animationAssimp->mNumMeshChannels; ++channelIndex) {
		aiNodeAnim* nodeAnimationAssimp = animationAssimp->mChannels[channelIndex];
		NodeAnimation& nodeAnimation = animation.nodeAnimations[nodeAnimationAssimp->mNodeName.C_Str()];
		// Translateの解析
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumPositionKeys; keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mPositionKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { -keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z }; // 左手座標から右手座標に変換
			nodeAnimation.translate.keyframes.push_back(keyframe);
		}
		// Rotateの解析
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumRotationKeys; keyIndex) {
			aiQuatKey& keyAssimp = nodeAnimationAssimp->mRotationKeys[keyIndex];
			KeyframeQuaternion keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x, -keyAssimp.mValue.y, -keyAssimp.mValue.z, keyAssimp.mValue.w }; // 左手座標から右手座標に変換
			nodeAnimation.rotate.keyframes.push_back(keyframe);
		}
		// Scaleの解析
		for (uint32_t keyIndex = 0; keyIndex < nodeAnimationAssimp->mNumScalingKeys; keyIndex) {
			aiVectorKey& keyAssimp = nodeAnimationAssimp->mScalingKeys[keyIndex];
			KeyframeVector3 keyframe;
			keyframe.time = float(keyAssimp.mTime / animationAssimp->mTicksPerSecond);
			keyframe.value = { keyAssimp.mValue.x, keyAssimp.mValue.y, keyAssimp.mValue.z };
			nodeAnimation.translate.keyframes.push_back(keyframe);
		}
	}

	return animation;
}

Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time) {
	assert(!keyframes.empty());
	// キーが一つか、時刻がキーフレーム前なら最初の値にする
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexを比較して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Lerps::Lerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}

	return (*keyframes.rbegin()).value;
}
Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time) {
	assert(!keyframes.empty());
	// キーが一つか、時刻がキーフレーム前なら最初の値にする
	if (keyframes.size() == 1 || time <= keyframes[0].time) {
		return keyframes[0].value;
	}

	for (size_t index = 0; index < keyframes.size() - 1; ++index) {
		size_t nextIndex = index + 1;
		// indexとnextIndexを比較して範囲内に時刻があるかを判定
		if (keyframes[index].time <= time && time <= keyframes[nextIndex].time) {
			// 範囲内を補間
			float t = (time - keyframes[index].time) / (keyframes[nextIndex].time - keyframes[index].time);
			return Slerp(keyframes[index].value, keyframes[nextIndex].value, t);
		}
	}

	return (*keyframes.rbegin()).value;
}

Node ReadNode(aiNode* node) {
	Node result;
	aiMatrix4x4 aiLoclMatrix = node->mTransformation;
	aiLoclMatrix.Transpose();
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++) {
			result.localMatrix.m[i][j] = aiLoclMatrix[i][j];
		}
	}
	result.name = node->mName.C_Str();
	result.children.resize(node->mNumChildren);
	for (uint32_t childIndex = 0; childIndex < node->mNumChildren; ++childIndex) {
		// 再帰的に読む
		result.children[childIndex] = ReadNode(node->mChildren[childIndex]);
	}

	return result;
}