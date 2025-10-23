#pragma once
#include "ModelCommon.h"
#include "Math.h"
#include "TextureManager.h"
#include <string>
#include <vector>
#include <fstream>
#include "Model.h"
#include <map>
using namespace std;
/// <summary>
/// 3Dモデル管理クラス
/// </summary>
class ModelManager
{
public: // メンバ関数
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	static ModelManager* GetInstance();
	/// <summary>
	/// シングルトンインスタンスの削除
	/// </summary>
	void Finalize();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon"></param>
	void Initialize(DirectXCommon* dxCommon);
	/// <summary>
	/// モデルファイルの読み込み
	/// </summary>
	/// <param name="filePath"></param>
	void LoadModel(const string& filePath);
public: // Getter/Setter
	/// <summary>
	/// モデルの検索
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	Model* FindModel(const string& filePath);

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
	ModelCommon* modelCommon = nullptr;
	// モデルデータのリスト
	map<string, unique_ptr<Model>> models;

};

