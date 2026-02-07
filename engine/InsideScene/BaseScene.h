#pragma once
#include "DirectXCommon.h"

/// <summary>
/// シーン基底クラス
/// </summary>
class SceneManager;
class BaseScene
{
public:
    /// <summary>
	/// デストラクタ
    /// </summary>
    virtual ~BaseScene() = default;
    /// <summary>
	/// 初期化
    /// </summary>
    /// <param name="dxCommon"></param>
    virtual void Initialize(Engine::DirectXCommon* dxCommon) = 0;
    /// <summary>
	/// 更新
    /// </summary>
    virtual void Update() = 0;
    /// <summary>
	/// 描画
    /// </summary>
    virtual void Draw() = 0;
    /// <summary>
	/// 終了処理
    /// </summary>
    virtual void Finalize() = 0;
    // Setter
	/// <summary>
	/// シーンマネージャーのセット
	/// </summary>
    virtual void SetSceneManager(SceneManager* sceneManager) { this->sceneManager = sceneManager; }

protected:
	// シーンマネージャー
    SceneManager* sceneManager = nullptr;
};

