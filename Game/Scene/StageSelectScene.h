#pragma once
#include "engine/InsideScene/BaseScene.h"
#include <MyMath.h>
#include "SceneSelectGraph/StageSelectGraph.h"

/// <summary>
/// ステージセレクトシーン
/// ステージ選択画面のシーン
/// </summary>

struct StageStatus {

};
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

