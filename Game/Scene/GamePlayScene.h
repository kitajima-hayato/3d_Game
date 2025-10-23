#pragma once
#include "Audio.h"
#include "SpriteCommon.h"
#include "InsideScene/BaseScene.h"
#include "engine/math/MyMath.h"
#include "Game/Application/Enemy/EnemyFactory.h"
#include "SceneTransition/SceneTransition.h"
#include "Game/Application/BackGround.h"
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
	/// 当たり判定
	/// </summary>
	void CheckCollision();

	/// <summary>
	/// スタートカメラの更新
	/// </summary>
	void UpdateStartCamera(float dt);

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

	/// スタート演出
	enum class StartCamPhase { None, DollyIn, Settle, Shake };
	StartCamPhase startPhase_ = StartCamPhase::None;
	// カメラタイマー
	float startTimer_ = 0.0f;
	// 開始位置
	Vector3 camStartPos_;
	// 目標位置
	Vector3 camTargetPos_;
	// オーバーシュート位置
	Vector3 camOvershootPos_;

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
	//std::unique_ptr<SceneTransition> sceneTransition;

};

