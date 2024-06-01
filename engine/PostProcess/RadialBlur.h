#pragma once
#include "Sprite.h"
#include "DirectXCommon.h"
#include "Camera.h"
#include "PostEffectPSO.h"
#include "IPostEffect.h"
#include <Windows.h>

class RadialBlur : public IPostEffect {
private:// 構造体
	// ブラー
	struct RadialBlurData {
		uint32_t isActive;  // 使用するか
		float strength;  // ブラーの強さ
	};

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	RadialBlur();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize()override;

	/// <summary>
	/// 更新処理
	/// </summary>
	//void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw(uint32_t psoNum)override;

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDrawScene()override;

	/// <summary>
	/// 描画後の処理
	/// </summary>
	void PostDrawScene()override;

private:
	RadialBlurData* radialBlurData_;
};