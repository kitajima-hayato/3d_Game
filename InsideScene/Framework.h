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
class Framework
{

public:
	virtual ~Framework() = default;
public:	// メンバ関数
	// 初期化
	virtual void Initialize();
	// 更新
	virtual void Update();
	// 描画
	virtual void Draw() = 0;
	// 終了処理
	virtual void Finalize();
	// 実行
	void Run();
	// 終了リクエスト
	bool IsEndRequst() { return isEndRequst;}

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
	Transform cameraTransform;

	
	
};

