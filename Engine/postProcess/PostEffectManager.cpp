#include "PostEffectManager.h"

PostEffectManager::~PostEffectManager() {
	for (int i = 0; i < COUNT; i++) {
		delete postEffect_[i];
	}
}

void PostEffectManager::Initialize() {
	for (int i = 0; i < PostEffectType::COUNT; i++) {
		postEffect_[i] = new PostEffect();
		postEffect_[i]->Initialize();
	}
}

void PostEffectManager::PreDraw() {

	postEffect_[NORMAL]->PreDrawScene();
}

void PostEffectManager::PostDraw() {
	postEffect_[NORMAL]->PostDrawScene();

	for (int i = 0; i < COUNT - 1; i++) {
		postEffect_[i + 1]->PreDrawScene();
		postEffect_[i]->Draw(i);
		postEffect_[i + 1]->PostDrawScene();
	}
}

void PostEffectManager::Draw() {
	postEffect_[COUNT - 1]->Draw(COUNT - 1);
}