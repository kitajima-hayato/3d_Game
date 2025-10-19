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
	/// <param name="dxCommon"></param>
	void Initialize(DirectXCommon* dxCommon);
	/// <summary>
	/// 共通描画設定
	/// </summary>
	void DrawSettingCommon();
public:	// Getter/Setter
	// DirectXCommon
	DirectXCommon* GetDxCommon()const { return dxCommon_; }
private:
	// 絶対にnew,deleteしない
	DirectXCommon* dxCommon_;

};

