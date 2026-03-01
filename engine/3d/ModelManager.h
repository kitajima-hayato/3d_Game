#pragma once
#include "ModelCommon.h"
#include "Math.h"
#include "TextureManager.h"
#include <string>
#include <vector>
#include <fstream>
#include "Model.h"
#include <map>
/// <summary>
/// 3Dモデル管理クラス
/// </summary>
class ModelManager
{
public: // メンバ関数
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	static ModelManager& GetInstance();
	/// <summary>
	/// シングルトンインスタンスの削除
	/// </summary>
	void Finalize();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(Engine::DirectXCommon* dxCommon);

	/// <summary>
	/// モデルファイルの読み込み
	/// </summary>
	/// <param name="filePath">読み込むモデルの名前</param>
	void LoadModel(std::string_view modelDir);
public: // Getter/Setter
	/// <summary>
	/// モデルの検索
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	Model* FindModel(std::string_view filePath);

private: // メンバ変数シングルトン
	/// <summary>
	/// シングルトンインスタンス
	/// </summary>
	static ModelManager* instance;
	/// <summary>
	/// コンストラクタ
	/// </summary>
	ModelManager() = default;
	/// <summary>
	/// デストラクタ
	/// </summary>
	~ModelManager() = default;
	/// <summary>
	/// コピーコンストラクタ無効化
	/// </summary>
	/// <param name=""></param>
	ModelManager(ModelManager&) = delete;
	/// <summary>
	/// コピー代入演算子無効化
	/// </summary>
	ModelManager& operator=(ModelManager&) = delete;
private: // メンバ変数
	std::unique_ptr<ModelCommon> modelCommon = nullptr;
	// モデルデータのリスト
	std::map<std::string, unique_ptr<Model>> models;

};

