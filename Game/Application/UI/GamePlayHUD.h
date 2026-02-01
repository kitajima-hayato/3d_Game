#pragma once
#include <memory>
#include "engine/2d/Sprite.h"
#include "engine/math/MyMath.h"

class GamePlayHUD
{
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	/// <param name="dt">ゲーム内時間</param>
	void Update();

	/// <summary>
	/// 操作UIの更新
	/// </summary>
	void UpdateControlUI();

	/// <summary>
	/// ポーズアイコンの更新
	/// </summary>
	void UpdatePauseIcon();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="isPaused">ポーズ中か</param>
	/// <param name="showControls">操作系UIが見える状態か</param>
	void Draw(bool isPaused,bool showControls);


	/// <summary>
	/// ImGuiの描画
	/// </summary>
	void DrawImGui();

	/// <summary>
	/// スプライトの生成ヘルパ
	/// </summary>
	std::unique_ptr<Sprite> MakeSprite(const std::string& path, const Vector2& pos, const Vector2& size, const Vector4 color);


private:
	// 操作UI
	// DキーUI
	std::unique_ptr<Sprite> controlUI_D;
	Vector2 controlUI_DPos_ = { 100.0f, 600.0f };
	Vector4 controlUIColorD_ = { 1.0f,1.0f,1.0f,0.5f };
	// AキーUI
	std::unique_ptr<Sprite>controlUI_A;
	Vector2 controlUIAPos_ = { 50.0f,600.0f };
	Vector4 controlUIColorA_ = { 1.0f,1.0f,1.0f,0.5f };
	// WキーUI
	std::unique_ptr<Sprite>controlUI_W;
	Vector2 controlUIWPos_ = { 75.0f,550.0f };
	Vector4 controlUIColorW_ = { 1.0f,1.0f,1.0f,0.5f };
	// SキーUI
	std::unique_ptr<Sprite>controlUI_S;
	Vector2 controlUISPos_ = { 75.0f,650.0f };
	Vector4 controlUIColorS_ = { 1.0f,1.0f,1.0f,0.5f };

	// ポーズ案内(esc)
	std::unique_ptr<Sprite> pauseSprite_;

	// UIアクティブフラグ
	bool UiActive_ = false;
	int32_t uiTimer = 0;

	// 非アクティブカラー / 押されていない時は半透明
	Vector4 inactiveColor_ = { 1.0f,1.0f,1.0f,0.5f };
	// アクティブカラー / 押されている時は不透明
	Vector4 activeColor_ = { 1.0f,1.0f,1.0f,1.0f };

	// 押された時のオフセット量
	float pressedOffset_ = 5.0f;

	
};

