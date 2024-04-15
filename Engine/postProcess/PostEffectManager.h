#pragma once
#include "PostEffect.h"
#include "PostEffectPSO.h"

class PostEffectManager
{
public:
	PostEffectManager() = default;
	~PostEffectManager();
	void Initialize();
	void PreDraw();
	void PostDraw();
	void Draw();

public:
	PostEffect* postEffect_[PostEffectType::COUNT];
};