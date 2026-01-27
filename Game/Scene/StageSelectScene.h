#pragma once
#include "InsideScene/BaseScene.h"
#include <MyMath.h>
#include "SceneSelectGraph/StageSelectGraph.h"
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
    void HandleSelectInput();

	void ApplyNodeToCursorTransform();
    
	void DrawSelectGraphImGui();

private:
	// カメラ
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
	

#ifdef USE_IMGUI
	// 選択中ノードが変わったか判定用
	uint32_t prevEditNodeId_ = UINT32_MAX;


	// 編集バッファ（毎フレーム上書きされないように保持）
	int editX_ = 0;
	int editY_ = 0;
	int editStageId_ = 0;
	bool editUnlocked_ = false;

	int editNeighUp_ = -1;
	int editNeighDown_ = -1;
	int editNeighLeft_ = -1;
	int editNeighRight_ = -1;

	// フィルタ
	int filterStageId_ = -1;     // -1で無効
	bool filterUnlockedOnly_ = false;

	// JSON
	bool jsonDirty_ = true;
	char jsonBuf_[32768] = {};

	
#endif
};

