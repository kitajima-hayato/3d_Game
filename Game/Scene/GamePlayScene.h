#pragma once
#include "Audio.h"
#include "SpriteCommon.h"
#include "InsideScene/BaseScene.h"
#include "engine/math/MyMath.h"
#include "Game/Camera/camera.h"
#include "Game/Application/Enemy/EnemyFactory.h"
class Map;
class Camera;
class Player;
class Object3D;
class GamePlayScene :public BaseScene
{
public:
	GamePlayScene();
	~GamePlayScene();
	// 初期化
	void Initialize(DirectXCommon* dxCommon)override;
	// 更新
	void Update()override;
	// 描画
	void Draw()override;
	// 終了処理
	void Finalize()override;

	/// エネミーの初期化
	void InitializeEnemy();


private:
	// マップ
	std::unique_ptr<Map> map;

	// オーディオ
	// サウンドデータ
	SoundData soundData;
	IXAudio2* xaudio2_;

	// Player
	std::unique_ptr<Player> player;

	std::unique_ptr<Object3D>enemy;
	Transform enemyTransform = {
		// Scale
		{1.0f, 1.0f, 1.0f},
		// Rotate
		{0.0f, 0.0f, 0.0f},
		// Translate
		{10.0f, -7.0f, 20.0f}
	};

	/// エネミーファクトリー
	std::unique_ptr<EnemyBace>normalEnemy;
	std::unique_ptr<EnemyBace> flyingEnemy;
};

