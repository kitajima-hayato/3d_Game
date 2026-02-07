#pragma once
#include "engine/InsideScene/BaseScene.h"
#include <MyMath.h>
#include "SceneSelectGraph/StageSelectGraph.h"
#include "Sprite.h"

/// <summary>
/// ステージセレクトシーン
/// ステージ選択画面のシーン
/// </summary>


class Object3D;
class Camera;
class StageSelectScene :
    public BaseScene
{

    public:
    StageSelectScene();
    ~StageSelectScene();

    /// <summary>
    /// 初期化処理
    /// </summary>
    void Initialize(Engine::DirectXCommon* dxCommon)override;

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
    /// Imgui一括管理
	/// </summary>
	void DrawImgui();

	/// <summary>
	/// プレイヤー移動処理
	/// </summary>
    void PlayerMove();

private:
    /// <summary>
	/// ステージセレクト入力処理
    /// </summary>
    void HandleSelectInput();
	/// <summary>
	/// ノード位置をカーソル位置に合わせる
	/// </summary>
	void ApplyNodeToCursorTransform();
    
	/// <summary>
	/// ステージセレクトグラフ編集ImGui
	/// </summary>
	void DrawSelectGraphImGui();

	/// <summary>
	/// ノードIDからワールド座標を計算
	/// </summary>
	/// <param name="nodeId">取得するノード</param>
	/// <returns>ノードのワールド座標</returns>
	Vector3 CalcNodeWorldPos(uint32_t nodeId)const;

	/// <summary>
	/// カーソル移動更新
	/// </summary>
	void UpdateCursorMove();

	/// <summary>
	/// 指定座標からカメラ方向を向くためのヨー角を計算
	/// </summary>
	/// <param name="pos"></param>
	/// <returns></returns>
	float CalcYawFaceCamera(const Vector3& pos)const;

	/// <summary>
	/// カメラ方向を向く更新
	/// </summary>
	void UpdateFaceYaw();

	/// <summary>
	/// ルートモデルの構築
	/// </summary>
	void BuildRoutes();

private:
	// カメラ / トランスフォーム
	Camera* camera = nullptr;
	Transform cameraTransform;

	// ステージ選択オブジェクト
	std::unique_ptr<Object3D> playerModel;
	Transform playerTransform;

    // セレクト画面１の土台
	std::unique_ptr<Object3D> stageSelectBase1;
	Transform stageSelectBase1Transform;

    // 空背景
	std::unique_ptr<Object3D> skyBack;
	Transform skyBackTransform;


	// ステージセレクトグラフ
	std::unique_ptr<StageSelectGraph> stageSelectGraph;
    uint32_t currentNodeId = 0;
	uint32_t startNodeId = 0;

	// ノードモデル群
	std::vector<std::unique_ptr<Object3D>>nodeModels_;
	std::vector<Transform>nodeTransforms_;

	std::vector<std::unique_ptr<Object3D>> routeModels_;
	std::vector<Transform> routeTransforms_;

	// stages electUI
	std::unique_ptr<Sprite>stageSelect_;

	// kyeUI
	std::unique_ptr<Sprite> keyIcon_W;
	Vector2 keyIcon_W_Pos = { 1100.0f,650.0f };
	std::unique_ptr<Sprite> keyIcon_A;
	Vector2 keyIcon_A_Pos = { 1020.0f,680.0f };
	std::unique_ptr<Sprite> keyIcon_S;
	Vector2 keyIcon_S_Pos = { 1100.0f,710.0f };
	std::unique_ptr<Sprite> keyIcon_D;
	Vector2 keyIcon_D_Pos = { 1180.0f,680.0f };

	// esc
	std::unique_ptr<Sprite> keyIcon_Esc;
	Vector2 keyIcon_Esc_Pos = { 100.0f,650.0f };
	// enter
	std::unique_ptr<Sprite> keyIcon_Enter;
	Vector2 keyIcon_Enter_Pos = { 200.0f,650.0f };

	// MoveUI
	std::unique_ptr<Sprite>moveUI_;
	Vector2 moveUI_Pos_ = { 160.0f,635.0f };
	// CheckUI
	std::unique_ptr<Sprite>checkUI_;
	Vector2 checkUI_Pos_ = { 160.0f, 505.0f };
	// BackUI
	std::unique_ptr<Sprite>backUI_;
	Vector2 backUI_Pos_ = { 160.0f, 560.0f };

private:
	// ノード移動アニメーション用
	bool isMoving_ = false;
	float moveTime_ = 0.0f;
	float moveDuration_ = 0.18f;

	// 移動前後の座標
	Vector3 moveStartPos_ = {};
	Vector3 moveTargetPos_ = {};
	Vector3 prevFramePos_ = {};

	// カーソルの半径
	float cursorRadius_ = 2.5f;

	
	// 停止後の「カメラ方向を向く」補間用
	bool  isFacing_ = false;
	float faceTime_ = 0.0f;
	float faceDuration_ = 0.18f;   
	float faceStartYaw_ = 0.0f;
	float faceTargetYaw_ = 0.0f;

	float faceStartRx_ = 0.0f;
	float faceStartRz_ = 0.0f;

	float moveSpeed_ = 18.0f;      // ワールド単位/秒（好みで調整）
	float moveMinDuration_ = 0.18f;
	float moveMaxDuration_ = 0.55f;

	// --- 回頭（Yaw）制御 ---
	float yawTurnSpeed_ = 15.0f;   // [rad/sec] 回頭の速さ（大きいほどクイック）
	float camBiasMove_ = 1.0f;   // 移動中にどれだけカメラ方向へ寄せるか(0..1)
	float camBiasEnd_ = 0.8f;   // 移動の終盤でカメラ方向へ寄せる量(0..1)

	


#ifdef USE_IMGUI
	// 選択中ノードが変わったか判定用
	uint32_t prevEditNodeId_ = UINT32_MAX;


	// 編集バッファ（毎フレーム上書きされないように保持）
	int editX_ = 0;
	int editY_ = 0;
	int editStageId_ = 0;
	// デフォルトステージキー
	char editStageKey_[64] = "1-1";
	char newStageKey_[64] = "1-1";
	bool editUnlocked_ = false;

	int editNeighUp_ = -1;
	int editNeighDown_ = -1;
	int editNeighLeft_ = -1;
	int editNeighRight_ = -1;

	// フィルタID
	int filterStageId_ = -1;     // -1で無効
	bool filterUnlockedOnly_ = false;

	// JSON
	bool jsonDirty_ = true;
	char jsonBuf_[32768] = {};
	
	// 編集用ノードID
	uint32_t editNodeId_ = 0;

	// 追加用入力
	int newX_ = 0;
	int newY_ = 0;
	int newStageId_ = 0;
	bool newUnlocked_ = true;

	// 接続編集用（プルダウン）
	int neighborUp_ = -1;
	int neighborDown_ = -1;
	int neighborLeft_ = -1;
	int neighborRight_ = -1;
	
#endif
};

