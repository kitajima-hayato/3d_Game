#pragma once
#include <memory>
#include "ITransitionEffect.h"
/// <summary>
/// シーン遷移の状態
/// </summary>
enum class TransitionPhase {
	None,			// 遷移なし
	FadeOut,		// フェードアウト中
	Loading,		// ロード中
	FadeIn			// フェードイン中
};

/// <summary>
/// 遷移の状態
/// </summary>
enum class TransitionType {
	Start,	// タイトル → ゲーム開始（少し凝ってる）
	Normal,	// 通常のシーン切り替え（遷移速め）
};

/// <summary>
/// シーン遷移の管理クラス
/// </summary>
class TransitionManager
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	void Update(float deltaTime);
	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();
	/// <summary>
	/// 遷移を開始
	/// </summary>
	void StartTransition(TransitionType type);
public:	// Getter / Setter
	/// <summary>
	/// 遷移演出のセット
	/// </summary>
	/// <param name="effect"></param>
	void SetTransitionEffect(std::unique_ptr<ITransitionEffect>effect);
	/// <summary>
	/// 現在の遷移フェーズの取得
	/// </summary>
	/// <returns>現在の遷移フェーズ</returns>
	TransitionPhase GetPhase() const { return phase_; }
	/// <summary>
	/// ロード開始可能か
	/// </summary>
	/// <returns>開始していいか</returns>
	bool CanStartLoading() const { return phase_ == TransitionPhase::Loading; }
	/// <summary>
	/// ロードの完了通知
	/// </summary>
	void OnLoadingComplete();
	/// <summary>
	/// 遷移中かどうか
	/// </summary>
	/// <returns>遷移中か</returns>
	bool IsTransitioning() const { return phase_ != TransitionPhase::None; }

private:
	// 現在のフェーズ
	TransitionPhase phase_ = TransitionPhase::None;

	// 遷移演出
	std::unique_ptr<ITransitionEffect> transitionEffect_ = nullptr;

	// 遷移の終了フラグ
	bool loadingComplete_ = false;

};

