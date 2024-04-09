#pragma once
#include "MathStructs.h"
#include <vector>
#include <fstream>
#include <string>
#include <sstream>
#include <stdint.h>
#include <assimp/scene.h>

struct TransformationMatrix {
	Matrix4x4 matWorld;
	Matrix4x4 WorldInverseTrasnpose;
};
struct Transform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};

struct VertexData {
	Vector4 position;
	Vector2 texcoord;
	Vector3 normal;
};

struct Material {
	Vector4 color;
	int32_t enableLighting;
	float padding[3];
	Matrix4x4 uvTransform;
	float shininess;
};
struct MaterialData {
	std::string textureFilePath;
};

MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);

struct Node {
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};

Node ReadNode(aiNode* node);

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
	Node rootNode;
};