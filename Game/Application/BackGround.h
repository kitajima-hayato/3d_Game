#pragma once
#include "engine/3d/Object3D.h"
#include "engine/math/MyMath.h"
#include <memory>
/// <summary>
/// 背景クラス
/// 各ステージの背景を管理する
/// </summary>

class BackGround
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	/// <summary>
	/// Imgui一括管理 / 描画関数
	/// </summary>
	void DrawImgui();
private:
	// タイトル背景
	std::unique_ptr<Object3D> titleBackGround3;
	Transform titleBackGround3Transform;

	std::unique_ptr<Object3D> titleBackGround3_2;
	Transform titleBackGround3_2Transform;

	std::unique_ptr<Object3D> titleBackGround2;
	Transform titleBackGround2Transform;

	std::unique_ptr<Object3D> titleBackGround2_2;
	Transform titleBackGround2_2Transform;

	std::unique_ptr<Object3D> titleBackGround2_3;
	Transform titleBackGround2_3Transform;

	std::unique_ptr<Object3D> titleBackGround1;
	Transform titleBackGround1Transform;

	std::unique_ptr<Object3D> titleBackGround1_2;
	Transform titleBackGround1_2Transform;

	std::unique_ptr<Object3D> titleBackGround1_3;
	Transform titleBackGround1_3Transform;

	std::unique_ptr<Object3D> titleBackGround1_4;
	Transform titleBackGround1_4Transform;

	// ステージセレクト背景
	std::unique_ptr<Object3D> skyBack;
	Transform skyBackTransform;

	// 足場　土
	std::unique_ptr<Object3D> soil;
	Transform soilTransform;
};

