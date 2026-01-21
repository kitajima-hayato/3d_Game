#pragma once
#include "InsideScene/BaseScene.h"
#include "MyMath.h"

class Object3D;
class Camera;
class StageClearScene :
    public BaseScene
{
public:
	StageClearScene();
	~StageClearScene();

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

private:
	/// カメラ
	Camera* camera = nullptr;
	Transform cameraTransform;
};

