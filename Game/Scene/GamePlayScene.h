#pragma once
#include "Audio.h"
#include "SpriteCommon.h"
#include "engine/InsideScene/BaseScene.h"
#include "engine/math/MyMath.h"
#include "Game/Application/Enemy/EnemyFactory.h"
#include "Game/Application/BackGround.h"
#include "Game/Camera/CameraController.h"
#include "Game/Particle/ParticleEmitter.h"
#include "SceneTransition/SceneTransition.h"
#include "Game/Scene/Pause/PauseSystem.h"
#include "Game/Application/UI/GamePlayHUD.h"
#include "Game/Camera/StartCamPhase.h"
#include "Game/Application/UI/DamageFeedBack.h"
/// <summary>
/// ゲームプレイシーン
/// ゲームプレイ中のシーンを管理する
/// </summary>

// 前方宣言
class Map;
class Player;
class Object3D;
class CollisionManager;
/// <summary>
/// ゲームプレイシーン : 基底シーンクラスを継承
/// </summary>
class GamePlayScene :public BaseScene
{
public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	GamePlayScene();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~GamePlayScene();
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
	/// Imgui一括管理 / 描画関数
	/// </summary>
	void DrawImgui();

	/// <summary>
	/// エネミーの初期化
	/// </summary>
	void InitializeEnemy();

	/// <summary>
	/// エネミーの生成
	/// </summary>
	void GenerateEnemy();

	/// <summary>\
	/// 当たり判定
	/// </summary>
	void CheckCollision();

	/// <summary>
	/// スタートカメラの更新
	/// </summary>
	void UpdateStartCamera(float dt);

	/// <summary>
	/// エネミーヒット時のカメラシェイク
	/// </summary>
	void EnemyHitShake(float dt);

	/// <summary>
	/// UI/ダメージ/スプライト類の初期化 / 更新 / 描画
	/// </summary>
	void SpritesInitialize();
	void SpritesUpdate();
	void SpritesDraw();


	

private:
	// マップ
	std::unique_ptr<Map> map;
	
	// オーディオ
	// サウンドデータ
	SoundData soundData;
	IXAudio2* xaudio2_;

	// Player
	std::unique_ptr<Player> player;


	/// エネミーファクトリー
	std::vector<std::unique_ptr<EnemyBase>> enemies;

	/// コリジョンマネージャー
	std::unique_ptr<CollisionManager> collision_;


	/// カメラ
	Camera* camera = nullptr;
	Transform cameraTransform;

	

	// 被弾フィードバック
	std::unique_ptr<DamageFeedBack> damageFeedBack_;


	
	float shakeAmp_ = 0.1f;

	


	/// タイトルロゴ
	std::unique_ptr<Object3D> titleLogoObject;
	Transform titleLogoTransform;

	/// バックグラウンド
	std::unique_ptr<BackGround> backGround;


	// シーン遷移
	std::unique_ptr<SceneTransition> sceneTransition;

	// フォローカメラ
	std::unique_ptr<CameraController> cameraController_;

	// ポーズシステム
	std::unique_ptr<PauseSystem> pauseSystem_;

	// ステージ開始演出フラグ
	bool stageStartEventFlag_ = true;
	// 固定フレームレート用のデルタタイム
	const float dt = 1.0f / 60.0f;

	// エネミー配置のオフセット
	float enemySpawnOffset_ = 0.5f;

	// エネミーヒットのシェイク時間
	float enemyHitShakeTime_ = 1.0f;
	// エネミーヒット時のカメラシェイク用
	bool enemyHitShakeActive_ = false;
	// シェイク時間
	float enemyHitTimer_ = 0.0f;
	// シェイク持続時間 
	Vector3 enemyHitBasePos{};
	// 前フレームの敵当たり判定
	bool wasEnemyHit_ = false;
	

	std::unique_ptr<ParticleEmitter>testParticle_;

	

	bool isPlayerControlLocked_ = false;

	
	// エネミーヒット時のシェイク中に出す被弾スプライト
	std::unique_ptr<Sprite> enemyHitSprite_;
	float alpha;
	
	std::unique_ptr<GamePlayHUD> gamePlayHUD_;

	std::unique_ptr<StartCamPhase> startCam_;

	Vector3 baseCameraPos_ = {};

	std::string stageKey;
};

