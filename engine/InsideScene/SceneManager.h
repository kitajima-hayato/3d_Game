#pragma once
#include "BaseScene.h"
#include "AbstractSceneFactory.h"
// シーン管理クラス
// シングルトンクラス
class SceneManager
{
public:
	/// <summary>
	/// シングルトンの取得
	/// </summary>
	/// <returns></returns>
	static SceneManager* GetInstance();
	static void DestroyInstance();
	/// <summary>
	/// コンストラクタ
	/// </summary>
	SceneManager() = default;
	/// <summary>
	/// デストラクタ
	/// </summary>
	~SceneManager() = default;
	/// <summary>
	/// コピーコンストラクタの無効化
	/// </summary>
	/// <param name=""></param>
	SceneManager(SceneManager&) = delete;
	/// <summary>
	/// 代入演算子の無効化
	/// </summary>
	/// <param name=""></param>
	/// <returns></returns>
	SceneManager& operator=(SceneManager&) = delete;

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon"></param>
	void Update(DirectXCommon*dxCommon);
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

public:
	/// <summary>
	/// シーンファクトリーのセット
	/// </summary>
	/// <param name="sceneFactory"></param>
	void SetSceneFactory(AbstractSceneFactory* sceneFactory) { sceneFactory_ = sceneFactory; }
	/// <summary>
	/// シーン変更
	/// </summary>
	/// <param name="sceneName"></param>
	void ChangeScene(const std::string& sceneName);

private:
	// 今のシーン
	std::unique_ptr<BaseScene> scene_ = nullptr;
	// 次のシーン
	std::unique_ptr<BaseScene> nextScene_ = nullptr;

	// シーンファクトリー // 借り物
	AbstractSceneFactory* sceneFactory_ = nullptr;
	
};

