#pragma once
#include "Audio.h"
#include "SpriteCommon.h"
#include "InsideScene/BaseScene.h"
#include "engine/math/MyMath.h"
#include "Game/Application/Enemy/EnemyFactory.h"
#include "SceneTransition/SceneTransition.h"
class Map;
class Player;
class Object3D;
class CollisionManager;
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

	/// imgui
	void DrawImgui();

	/// エネミーの初期化
	void InitializeEnemy();

	/// 当たり判定
	void CheckCollision();


private:
	// マップ
	std::unique_ptr<Map> map;
	//
	// オーディオ
	// サウンドデータ
	SoundData soundData;
	IXAudio2* xaudio2_;

	// Player
	std::unique_ptr<Player> player;

	
	/// エネミーファクトリー
	std::vector<std::unique_ptr<EnemyBace>> enemies;

	std::unique_ptr<EnemyBace>normalEnemy;
	std::unique_ptr<EnemyBace> flyingEnemy;

	/// コリジョンマネージャー
	std::unique_ptr<CollisionManager> collision_;


	/// カメラ
	std::unique_ptr<Camera> camera;
	Transform cameraTransform;

	// シーン遷移
	//std::unique_ptr<SceneTransition> sceneTransition;

};

