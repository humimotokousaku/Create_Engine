#include "ModelStructs.h"
#include <cassert>

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