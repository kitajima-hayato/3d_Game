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

	// スタート演出
	enum class StartCamPhase {
		None,
		MoveToLeft,   // 左端へ移動（初期位置から）
		PanToRight,   // 左→右へパン
		Hold,         // 少し止める（任意）
		ReturnToStart // プレイヤー開始地点へ戻る
	};

	/// 横パン演出
	// 目標位置
	Vector3 camTargetPos_ = { 8.0f,3.5f,-20.0f };
	// 左端へ行く時間
	float introMoveDur_ = 0.6f;
	// 左から右パン時間
	float introPanDur_ = 1.6f;
	// 右端での停止時間
	float introHoldDur_ = 1.0f;
	// 開始地点へ戻る時間
	float introReturnDur_ = 3.0f;

	// 左端と右端
	float introLeftX_ = 8.0f;
	float introRightX_ = 92.0f;
	// 背景が崩壊しない高さ
	float introFixedY_ = 3.5f;
	// 固定Z座標 / 通常距離
	float introFixedZ_ = -20.0f;



	// 現在のフェーズ
	StartCamPhase startPhase_ = StartCamPhase::None;
	// カメラタイマー
	float startTimer_ = 0.0f;
	// 開始位置
	Vector3 camStartPos_;
	
	// オーバーシュート位置
	Vector3 camOvershootPos_;
	// どれだけ引くか
	const float pullBack = 30.0f;

	// カメラのパラメーター
	// ドリーイン時間
	float durDollyIn_ = 2.5f;
	// 揺れ時間
	float durSettle_ = 0.35f;
	// どれくらい近づくか
	float overShootAmt_ = 2.0f;
	// 揺れの時間
	float shakeTime_ = 0.3f;
	// 揺れの強さ
	float shakeAmp_ = 0.1f;

	static inline float EaseOutCubic(float t) {
		t = std::clamp(t, 0.0f, 1.0f);
		return 1.0f - std::pow(1.0f - t, 3.0f);
	}
	static inline float EaseOutBack(float t, float s = 1.70158f) {
		t = std::clamp(t, 0.0f, 1.0f);
		return 1.0f + (s + 1.0f) * std::pow(t - 1.0f, 3.0f) + s * std::pow(t - 1.0f, 2.0f);
	}
	static inline Vector3 Lerp(const Vector3& a, const Vector3& b, float t) {
		return { a.x + (b.x - a.x) * t, a.y + (b.y - a.y) * t, a.z + (b.z - a.z) * t };
	}


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
	// エネミーヒット時のシェイク中に出す被弾スプライト
	std::unique_ptr<Sprite> enemyHitSprite_;
	float alpha;

	std::unique_ptr<ParticleEmitter>testParticle_;

	// 操作UI
	std::unique_ptr<Sprite> controlUI_D;
	Vector2 controlUI_DPos_ = { 100.0f, 600.0f };
	Vector4 controlUIColorD_ = { 1.0f,1.0f,1.0f,0.5f };

	std::unique_ptr<Sprite>controlUI_A;
	Vector2 controlUIAPos_ = { 50.0f,600.0f };
	Vector4 controlUIColorA_ = { 1.0f,1.0f,1.0f,0.5f };

	std::unique_ptr<Sprite>controlUI_W;
	Vector2 controlUIWPos_ = { 75.0f,550.0f };
	Vector4 controlUIColorW_ = { 1.0f,1.0f,1.0f,0.5f };

	std::unique_ptr<Sprite>controlUI_S;
	Vector2 controlUISPos_ = { 75.0f,650.0f };
	Vector4 controlUIColorS_ = { 1.0f,1.0f,1.0f,0.5f };

	std::unique_ptr<Sprite>controlUI_DashUI;
	Vector2 controlUIDashUIPos_ = { 200.0f,600.0f };
	Vector4 controlUIDashUIColor_ = { 1.0f,1.0f,1.0f,0.5f };

	std::unique_ptr<Sprite>controlUI_move;
	Vector2 controlUImovePos_ = { 100.0f,700.0f };
	Vector4 controlUImoveColor_ = { 1.0f,1.0f,1.0f,0.5f };

	// UIアクティブフラグ
	bool UiActive_ = false;
	int32_t uiTimer = 0;

	bool isPlayerControlLocked_ = false;

	
	

	std::unique_ptr<Sprite> pauseSprite_;

};

