#pragma once
#include <d3d12.h>
#ifdef USE_IMGUI
#include "externals/imgui/imgui.h"
#endif
#include "engine/base/WinAPI.h" 
#include "engine/base/DirectXCommon.h"

/// <summary>
/// ImGui管理クラス
/// </summary>
/// <remarks>
/// ImGuiの初期化、描画を行う
/// シングルトンクラス
/// </remarks>

class ImGuiManager
{

public:
#ifdef USE_IMGUI
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinAPI*winAPI,DirectXCommon*dxCommon);

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize(); 
	
	/// <summary>
	/// ImGui受付開始
	/// <summary>
	void Begin();

	/// <summary>
	/// ImGui受付終了
	/// <summary>
	void End();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
#endif
private:
	DirectXCommon* dxCommon_ = nullptr;
	
	Microsoft::WRL::ComPtr<ID3D12Device> device = nullptr;
	
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_ = nullptr;


};
