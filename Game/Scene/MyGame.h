#pragma once
#ifdef USE_IMGUI
#include"externals/imgui/imgui_impl_dx12.h"
#include"externals/imgui/imgui_impl_win32.h"
#endif
#include"externals/DirectXTex/DirectXTex.h"
#include"externals/DirectXTex/d3dx12.h"
#include<vector>
#include<fstream>
#include<sstream>
#include<wrl.h>
#include "numbers"
#include "Input.h"
#include "WinAPI.h"
#include "DirectXCommon.h"
#include "Logger.h"
#include "D3DResourceLeakChecker.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "Object3D.h"
#include "Object3DCommon.h"
#include "Model.h"
#include "ModelCommon.h"
#include "ModelManager.h"
#include "srvManager.h"
#include "InsideScene/Framework.h"
#ifdef USE_IMGUI
#include "ImGuiManager.h"
#endif
#include "GamePlayScene.h"
#include "TitleScene.h"
#include "Game/Application/ModelList.h"
#include "engine/3d/SkyBox/SkyBox.h"

// ゲーム全体 : フレームワーククラス継承
class MyGame :public Framework
{
public:	// メンバ関数
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize() override;
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


private:	// メンバ変数
	
	// 読み込むモデルのリスト
	std::unique_ptr<ModelList> modelList;
	// スカイボックス
	std::unique_ptr<SkyBox> skyBox = nullptr;

	// レンダーテクスチャ
	bool usePostProcess_ = false;

};

