#pragma once
#include <memory>
#include "engine/2d/Sprite.h"
#include "engine/math/MyMath.h"
#include "PauseUI.h"

class PauseSystem
{

public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();


	/// <summary>
	/// true:ポーズ中 / false:ポーズ解除
	/// </summary>
	/// <returns></returns>
	bool Update();


	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// Pause画面描画
	/// </summary>
	void DrawImgui();

public:
	// Getter / Setter
	bool GetPause() const { return isPause_; }


private:

	// ポーズ中フラグ
	bool isPause_ = false;

	// ポーズUI
	std::unique_ptr<PauseUI> pauseUI_;
};

