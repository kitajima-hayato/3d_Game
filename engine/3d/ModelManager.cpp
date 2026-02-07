#include "ModelManager.h"

using namespace std;
using namespace Engine;
ModelManager* ModelManager::instance = nullptr;

ModelManager& ModelManager::GetInstance()
{
	static ModelManager instance;
	return instance;
}

void ModelManager::Finalize()
{
	models.clear();

	modelCommon.reset();
}

void ModelManager::Initialize(DirectXCommon* dxCommon)
{
	modelCommon = std::make_unique<ModelCommon>();
	modelCommon->Initialize(dxCommon);
}

void ModelManager::LoadModel(string_view filePath)
{
	// キー文字列の生成
	const string key(filePath);
	// 読み込み済みモデルを検索
	if (models.contains(key))
	{
		// 読み込み済みなら早期リターン
		return;
	}
	// モデルの生成とファイル読み込み、初期化
	unique_ptr<Model>model = make_unique<Model>();
	model->Initialize(modelCommon.get(), "resources", key);

	// モデルリストに追加
	//models[filePath] = move(model);
	models.insert(make_pair(filePath, move(model)));
}

Model* ModelManager::FindModel(string_view filePath)
{
	// キー文字列の生成
	const string key(filePath);
	// 読み込み済みモデルを検索
	if (models.contains(key))
	{
		// 読み込みモデルを戻り値として返す
		return models.at(key).get();
	}
	// 読み込み済みでない場合はnullptrを返す
	return nullptr;
}
