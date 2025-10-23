#pragma once
#include "InsideScene/BaseScene.h"
#include "engine/3d/Object3D.h"
/// <summary>
/// ステージセレクトシーン
/// </summary>
class StageCelectScene :
	public BaseScene
{

public:
	/// <summary>
	/// コンストラクタ
	/// </summary>
	StageCelectScene();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~StageCelectScene();
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


private:
	// ステージセレクトモデル
	std::unique_ptr<Object3D> stageSelectModel;
	// ステージ番号
	int stageNum = 1;
	// ステージセレクトフラグ
	bool isStageSelect = false;


};

