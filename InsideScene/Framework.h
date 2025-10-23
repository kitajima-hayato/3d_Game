#pragma once
#include<fstream>
#include<sstream>
#include<wrl.h>
#include "numbers"
#include "WinAPI.h"
#include "DirectXCommon.h"
#include "Sprite.h"
#include "SpriteCommon.h"
#include "TextureManager.h"
#include "Object3D.h"
#include "Object3DCommon.h"
#include "ModelManager.h"
#include "ImGuiManager.h"
#include "Audio.h"
#include "SceneManager.h"
#include "Game/Particle/ParticleManager.h"
#include "Game/Particle/EffectManager.h"
#include "engine/3d/SkyBox/SkyBox.h"
#include "AbstractSceneFactory.h"
#include "D3DResourceLeakChecker.h"
#include "Input.h"
#include "engine/2d/RenderTexture.h"
/// <summary>
/// フレームワーク基底クラス
/// </summary>
class Framework
{

public:
	/// <summary>
	/// デストラクタ
	/// </summary>
	virtual ~Framework() = default;
public:	// メンバ関数
	/// <summary>
	/// 初期化処理
	/// </summary>
	virtual void Initialize();
	/// <summary>
	/// 更新処理
	/// </summary>
	virtual void Update();
	/// <summary>
	/// 描画処理
	/// </summary>
	virtual void Draw() = 0;
	/// <summary>
	/// 終了処理
	/// </summary>
	virtual void Finalize();
	/// <summary>
	/// 実行
	/// </summary>
	void Run();
	/// <summary>
	/// 終了リクエストの有無
	/// </summary>
	/// <returns></returns>
	bool IsEndRequst() { return isEndRequst; }

protected:
	// 終了リクエスト
	bool isEndRequst = false;
protected:// Initialize関連
	// ウィンドウAPI
	std::unique_ptr<WinAPI> winAPI;
	// DirectX共通部
	std::unique_ptr<DirectXCommon> dxCommon;
	// ImGui
#ifdef _DEBUG
	std::unique_ptr<ImGuiManager> imGui;
#endif
	// SRVマネージャー
	std::unique_ptr<SrvManager> srvManager;
	// モデル共通部
	std::unique_ptr<ModelCommon> modelCommon;
	// カメラ
	std::unique_ptr<Camera> camera;

	// 
	std::unique_ptr<RenderTexture> renderTexture = nullptr;

public:
	// シーンファクトリー
	std::unique_ptr<AbstractSceneFactory> sceneFactory_ = nullptr;
private:
	// リークチェッカー
	D3DResourceLeakChecker leakCheck;
	/// カメラの座標
	Transform cameraTransform = {
		{0.0f, 0.0f, -10.0f}, // translate
		{0.0f, 0.0f, 0.0f}, // rotate
		{ 1.0f, 1.0f,-10.0f } // scale
	};

	
};

