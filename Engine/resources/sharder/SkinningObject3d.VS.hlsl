#include "Object3d.hlsli"
struct TransformationMatrix {
    float32_t4x4 matWorld;
    float32_t4x4 WorldInverseTranspose;
};
struct ViewProjectionMatrix {
    float32_t4x4 view;
    float32_t4x4 projection;
    float32_t3 cameraPos;
};
ConstantBuffer<TransformationMatrix> gTransformationMatrix : register(b0);
ConstantBuffer<ViewProjectionMatrix> gViewProjectionMatrix : register(b1);

struct Well {
    float32_t4x4 skeletonSpaceMatrix;
    float32_t4x4 skeletonSpaceInverseTransposeMatrix;
};

StructuredBuffer<Well> gMatrixPalette : register(t0);

struct VertexShaderInput {
    float32_t4 position : POSITION0;
    float32_t2 texcoord : TEXCOORD0;
    float32_t3 normal : NORMAL0;
    float32_t4 weight : WEIGHT0;
    int32_t4 index : INDEX0;
};

struct Skinned {
    float32_t4 position;
    float32_t3 normal;
};

Skinned Skinning(VertexShaderInput input) {
    Skinned skinned;
    // 位置の変換
    skinned.position = mul(input.position, gMatrixPalette[input.index.x].skeletonSpaceMatrix) * input.weight.x;
    skinned.position += mul(input.position, gMatrixPalette[input.index.x].skeletonSpaceMatrix) * input.weight.y;
    skinned.position += mul(input.position, gMatrixPalette[input.index.x].skeletonSpaceMatrix) * input.weight.z;
    skinned.position += mul(input.position, gMatrixPalette[input.index.x].skeletonSpaceMatrix) * input.weight.w;
    skinned.position.w = 1.0f;

    // 法線の変換
    skinned.normal = mul(input.normal, (float32_t3x3)gMatrixPalette[input.index.x].skeletonSpaceInverseTransposeMatrix) * input.weight.x;
    skinned.normal += mul(input.normal, (float32_t3x3)gMatrixPalette[input.index.x].skeletonSpaceInverseTransposeMatrix) * input.weight.y;
    skinned.normal += mul(input.normal, (float32_t3x3)gMatrixPalette[input.index.x].skeletonSpaceInverseTransposeMatrix) * input.weight.z;
    skinned.normal += mul(input.normal, (float32_t3x3)gMatrixPalette[input.index.x].skeletonSpaceInverseTransposeMatrix) * input.weight.w;
    skinned.normal = normalize(skinned.normal);

    return skinned;
}

VertexShaderOutput main(VertexShaderInput input) {
    VertexShaderOutput output;
    Skinned skinned = Skinning(input);

    float32_t4x4 WorldViewProjectionMatrix = mul(gViewProjectionMatrix.view, gViewProjectionMatrix.projection);
    output.position = mul(skinned.position, mul(gTransformationMatrix.matWorld, WorldViewProjectionMatrix));
    output.worldPosition = mul(skinned.position, gTransformationMatrix.matWorld).xyz;
    output.texcoord = input.texcoord;
    output.normal = normalize(mul(skinned.normal, (float32_t3x3)gTransformationMatrix.WorldInverseTranspose));

    return output;
}