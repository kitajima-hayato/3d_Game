#pragma once
#include "InsideScene/BaseScene.h"
#include "engine/3d/Object3D.h"
class StageCelectScene :
    public BaseScene
{

    public:
    StageCelectScene();
    ~StageCelectScene();
    // 初期化
    void Initialize(DirectXCommon* dxCommon)override;
    // 更新
    void Update()override;
    // 描画
    void Draw()override;
    // 終了処理
	void Finalize()override;


private:
    std::unique_ptr<Object3D> stageSelectModel;
    int stageNum = 1;
	bool isStageSelect = false;

    
};

