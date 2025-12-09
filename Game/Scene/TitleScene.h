#pragma once
#include "Audio.h"
#include "SpriteCommon.h"
#include "InsideScene/BaseScene.h"
#include "Sprite.h"
#include "InsideScene/SceneManager.h"
#include "Game/Particle/ParticleManager.h"
#include "Game/Particle/ParticleEmitter.h"
#include "Game/Particle/EffectManager.h"
#include "Game/Particle/EffectEmitter.h"
#include "Game/Loader/LevelLoader.h"
#include "SceneTransition/SceneTransition.h"
#include "Game/Application/BackGround.h"


#ifdef USE_IMGUI
#include "ImGuiManager.h"
#endif

/// <summary>
/// タイトルシーン
/// </summary>
class Camera;
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
	std::unique_ptr<ParticleEmitter> particleEmitter2;

	// リングエフェクト
	std::unique_ptr<Object3D> object3D;
	// 背景
	std::unique_ptr<Object3D> backgroundObject;


	// effect
	std::unique_ptr<EffectEmitter>effectEmitter;
	std::unique_ptr<EffectEmitter>cylinder;

private: // ゲーム要素
	Vector3 emitterScale = { 0.0f, 0.0f, 0.0f };
	Vector3 emitterRotate = { 0.0f, 0.0f, 0.0f };
	Vector3 emitterTranslate = { 0.0f, 0.0f, 10.0f };

	Transform effectTransform;

	Transform cylinderTransform;

	Vector3 speed = { 0.0f,0.0f,0.0f };


	bool isStart = false;



	/// levelloader
	std::unique_ptr<LevelLoader>levelData;

	std::unique_ptr<Object3D> playerObject;
	Transform playerTransform;

	std::unique_ptr<Object3D> Rainbow;
	Transform rainbowTransform;

	/// タイトルロゴ
	std::unique_ptr<Object3D> titleLogo;
	Transform titleLogoTransform;

	// バックグラウンド
	 std::unique_ptr<BackGround> background;

	 std::unique_ptr<Sprite> quxSprite_;

	

	//std::unique_ptr<SceneTransition> sceneTransition;

};

