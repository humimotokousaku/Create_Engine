#include "PostEffect.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "WinApp.h" 
#include "SrvManager.h"

PostEffect::PostEffect()
{

}

void PostEffect::Initialize() {
	// 基底クラスの初期化
	IPostEffect::Initialize();
#pragma region シェーダ内のパラメータを調整するための準備
	//// ブラーの情報を書き込む
	//blurResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(BlurData)).Get();
	//// データを書き込む
	//blurData_ = nullptr;
	//// 書き込むためのアドレスを取得
	//blurResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&blurData_));

	//// 高輝度テクスチャの情報を書き込む
	//highIntensityResource_ = CreateBufferResource(DirectXCommon::GetInstance()->GetDevice(), sizeof(HighIntensityData)).Get();
	//// データを書き込む
	//highIntensityData_ = nullptr;
	//// 書き込むためのアドレスを取得
	//highIntensityResource_.Get()->Map(0, nullptr, reinterpret_cast<void**>(&highIntensityData_));
#pragma endregion
}

void PostEffect::Draw(uint32_t psoNum) {
	IPostEffect::Draw(psoNum);
}

void PostEffect::PreDrawScene() {
	IPostEffect::PreDrawScene();
}

void PostEffect::PostDrawScene() {
	IPostEffect::PostDrawScene();
}