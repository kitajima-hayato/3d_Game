#pragma once
#include "InsideScene/BaseScene.h"
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

};

