#include "PipelineManager.h"
#include "ConvertString.h"
#include <format>
#include <cassert>

PipelineManager* PipelineManager::GetInstance() {
	static PipelineManager instance;

	return &instance;
}

void PipelineManager::Initialize() {
	// DXCの初期化
	DXCInitialize();

	// 3Dのオブジェクトに使用するPSO
	object3dPSO_ = Object3dPSO::GetInstance();
	object3dPSO_->Init(dxcUtils_, dxcCompiler_, includeHandler_,"Object3d.VS.hlsl","Object3d.PS.hlsl");
	object3dPSO_->CreatePSO();
	// 3Dの線に使用するPSO
	linePSO_ = LinePSO::GetInstance();
	linePSO_->Init(dxcUtils_, dxcCompiler_, includeHandler_,"LineVS.hlsl", "LinePS.hlsl");
	linePSO_->CreatePSO();
	// particleに使用するPSO
	particlePSO_ = ParticlePSO::GetInstance();
	particlePSO_->Init(dxcUtils_, dxcCompiler_, includeHandler_,"Particle.VS.hlsl","Particle.PS.hlsl");
	particlePSO_->CreatePSO();
	// postEffectに使用するPSO
	postEffectPSO_ = PostEffectPSO::GetInstance();
	postEffectPSO_->Initialize();

	// ビューポートの生成
	CreateViewport();
	// シザー矩形の生成
	CreateScissor();
}

void PipelineManager::PreDraw() {
	postEffectPSO_->PreDraw();
	DirectXCommon::GetInstance()->GetCommandList()->RSSetViewports(1, &viewport_); // Viewportを設定
	DirectXCommon::GetInstance()->GetCommandList()->RSSetScissorRects(1, &scissorRect_); // Scirssorを設定
}

void PipelineManager::DXCInitialize() {
	HRESULT hr;
	// dxCompilerの初期化
	hr = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr));
	hr = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr));

	// 現時点でincludeはしないが、includeに対応するために設定を行っておく
	hr = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr));
}

void PipelineManager::CreateViewport() {
	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport_.Width = (float)WinApp::kClientWidth_;
	viewport_.Height = (float)WinApp::kClientHeight_;
	viewport_.TopLeftX = 0;
	viewport_.TopLeftY = 0;
	viewport_.MinDepth = 0.0f;
	viewport_.MaxDepth = 1.0f;
}

void PipelineManager::CreateScissor() {
	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect_.left = 0;
	scissorRect_.right = WinApp::kClientWidth_;
	scissorRect_.top = 0;
	scissorRect_.bottom = WinApp::kClientHeight_;
}
