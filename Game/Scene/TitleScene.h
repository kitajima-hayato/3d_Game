#pragma once
#include "Audio.h"
#include "SpriteCommon.h"
#include "engine/InsideScene/BaseScene.h"
#include "Sprite.h"
#include "engine/InsideScene/SceneManager.h"
#include "Game/Particle/ParticleManager.h"
#include "Game/Particle/ParticleEmitter.h"
#include "Game/Loader/LevelLoader.h"
#include "SceneTransition/SceneTransition.h"
#include "Game/Application/BackGround.h"


#ifdef USE_IMGUI
#include "ImGuiManager.h"
#endif


/// <summary>
/// タイトルシーン
/// タイトルシーンを管理する
/// </summary>
class Camera;
class Object3D;
class TitleScene :public BaseScene
{
public:
	TitleScene();
	~TitleScene();
	/// <summary>
	/// 初期化処理
	/// </summary>
	/// <param name="dxCommon"></param>
	void Initialize(DirectXCommon* dxCommon)override;
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update()override;
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw()override;
	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize()override;
	/// <summary>
	///	ロード処理（オーディオ、スプライト）
	/// </summary>
	void LoadAudio();
	void LoadSprite();
	/// <summary>
	/// Imgui一括管理 / 描画関数
	/// </summary>
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

private: // ゲーム要素
	Vector3 emitterScale = { 0.0f, 0.0f, 0.0f };
	Vector3 emitterRotate = { 0.0f, 0.0f, 0.0f };
	Vector3 emitterTranslate = { 0.0f, 0.0f, 10.0f };

	Transform effectTransform;

	Transform cylinderTransform;

	Vector3 speed = { 0.0f,0.0f,0.0f };


	bool isStart = false;



	/// level loader
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

	 // タイトルスプライト
	 std::unique_ptr<Sprite> titleSprite;
	 std::unique_ptr<Sprite> pressStartSprite;
	 // 点滅
	 const int blinkInterval = 60;
	 const int blinkDuration = 30;

	 /// カメラ
	 Camera* camera = nullptr;

	//std::unique_ptr<SceneTransition> sceneTransition;

};

