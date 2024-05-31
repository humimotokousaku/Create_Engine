#include "PostEffectTest.hlsli"

struct Material {
	float32_t4 color;
	int32_t enableLighting;
	float32_t4x4 uvTransform;
};

ConstantBuffer<Material> gMaterial : register(b0);

Texture2D<float32_t4> gTexture : register(t0);
SamplerState gSampler : register(s0);

struct PixelShaderOutput {
	float32_t4 color : SV_TARGET0;
};

PixelShaderOutput main(VertexShaderOutput input) {
	const float32_t2 kCenter = float32_t2(0.5f, 0.5f);
	const int32_t kNumSamples = 10;
	const float32_t kBlurWidth = 0.01f;
	float32_t2 direction = input.texcoord - kCenter;
	float32_t4 outputColor = float32_t4(0.0f, 0.0f, 0.0f,0.0f);
	for (int32_t sampleIndex = 0; sampleIndex < kNumSamples; ++sampleIndex) {
		float32_t2 texcoord = input.texcoord + direction * kBlurWidth * float32_t(sampleIndex);
		outputColor += gTexture.Sample(gSampler, texcoord);
	}
	// 平均化
	outputColor *= rcp(kNumSamples);

	PixelShaderOutput output;
	output.color = outputColor;
	output.color.a = 1.0f;

	return output;
}