#pragma once
#include "DirectXCommon.h"
/// <summary>
/// 3Dモデル共通部
/// </summary>
class ModelCommon
{
public:	// メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ModelCommon();
	/// <summary>
	/// デストラクタ
	/// </summary>
	~ModelCommon();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Engine::DirectXCommon* dxCommon);
	/// <summary>
	/// 共通描画設定
	/// </summary>
	void DrawSettingCommon();
public:	// Getter/Setter
	// DirectXCommonの取得
	Engine::DirectXCommon* GetDxCommon()const { return dxCommon_; }
private:
	// 絶対にnew,deleteしない
	Engine::DirectXCommon* dxCommon_;

};

