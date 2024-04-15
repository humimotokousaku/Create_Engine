#pragma once
#include "MathStructs.h"
#include <map>
#include <optional>
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
struct EulerTransform {
	Vector3 scale;
	Vector3 rotate;
	Vector3 translate;
};
struct QuaternionTransform {
	Vector3 scale;
	Quaternion rotate;
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

#pragma region ノード
template<typename tValue>
struct Keyframe {
	float time;
	tValue value;
};
using KeyframeVector3 = Keyframe<Vector3>;
using KeyframeQuaternion = Keyframe<Quaternion>;

template<typename tValue>
// キーフレーム
struct AnimationCurve {
	std::vector<Keyframe<tValue>> keyframes;
};
// ノードアニメーション
struct NodeAnimation {
	AnimationCurve<Vector3> translate;
	AnimationCurve<Quaternion> rotate;
	AnimationCurve<Vector3> scale;
};
// アニメーション
struct Motion {
	float duration;	// アニメーション全体の尺(秒)
	std::map<std::string, NodeAnimation> nodeAnimations;
};
// アニメーションの読み込み
Motion LoadAnimationFile(const std::string& directoryPath, const std::string& filename);
// 任意の時刻の値を取得
Vector3 CalculateValue(const std::vector<KeyframeVector3>& keyframes, float time);
Quaternion CalculateValue(const std::vector<KeyframeQuaternion>& keyframes, float time);

// ノード
struct Node {
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	std::string name;
	std::vector<Node> children;
};
// ノードの読み込み
Node ReadNode(aiNode* node);
#pragma endregion

#pragma region SkeletonとJoint
struct Joint {
	QuaternionTransform transform;
	Matrix4x4 localMatrix;
	Matrix4x4 skeletonSpaceMatrix;
	std::string name;
	std::vector<int32_t> children;
	int32_t index;
	std::optional<int32_t> parent;
};
// Jointを作成
int32_t CreateJoint(const Node& node, const std::optional<int32_t>& parent, std::vector<Joint>& joints);

struct Skeleton {
	int32_t root;
	std::map<std::string, int32_t> jointMap;
	std::vector<Joint> joints;
};
// スケルトンを作成
Skeleton CreateSkeleton(const Node& rootNode);
// スケルトンの更新処理
void SkeletonUpdate(Skeleton& skeleton);
#pragma endregion
// スケルトンに対してアニメーションを適用
void ApplyAnimation(Skeleton& skeleton, const Motion& animation, float animationTime);

struct ModelData {
	std::vector<VertexData> vertices;
	MaterialData material;
	Node rootNode;
};