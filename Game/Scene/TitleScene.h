#pragma once
#include "Audio.h"
#include "SpriteCommon.h"
#include "InsideScene/BaseScene.h"
#include "Sprite.h"
#include "InsideScene/SceneManager.h"
#include "Game/Particle/ParticleManager.h"
#include "Game/Particle/ParticleEmitter.h"

#ifdef _DEBUG
#include "ImGuiManager.h"
#endif
class Object3D;
class TitleScene :public BaseScene
{
public:
	TitleScene();
	~TitleScene();
	// 初期化
	void Initialize(DirectXCommon* dxCommon)override;
	// 更新
	void Update()override;
	// 描画
	void Draw()override;
	// 終了処理
	void Finalize()override;
	// スレッド用の関数
	void LoadAudio();
	void LoadSprite();

	// Imgui一括管理 / 描画関数
	void DrawImgui();
private:

	// オーディオ
	// サウンドデータ
	//SoundData soundData;
	//IXAudio2* xaudio2_;

	// スプライト
	std::unique_ptr<Sprite> sprite_;

	// パーティクルエミッター
	std::unique_ptr<ParticleEmitter> particleEmitter;

	// リングエフェクト
	std::unique_ptr<Object3D> object3D;


private: // ゲーム要素
	// TitleScene.h
	Vector3 emitterScale = { 0.0f, 0.0f, 0.0f };
	Vector3 emitterRotate = { 0.0f, 0.0f, 0.0f };
	Vector3 emitterTranslate = { 0.0f, 0.0f, 10.0f };


};

