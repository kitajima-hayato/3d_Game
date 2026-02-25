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

	// --- 【修正】ディレクトリパスとファイル名を分離する ---
	std::string fullPath = "resources/" + string(filePath);
	std::string directoryPath;
	std::string fileName;

	// 最後のスラッシュの位置を探す
	size_t lastSlash = fullPath.find_last_of("/\\");
	if (lastSlash != std::string::npos) {
		// スラッシュより前をディレクトリパスとする
		directoryPath = fullPath.substr(0, lastSlash);
		// スラッシュより後ろをファイル名とする
		fileName = fullPath.substr(lastSlash + 1);
	} else {
		// もしスラッシュが無ければそのまま
		directoryPath = "resources";
		fileName = string(filePath);
	}

	// 分離した正しいディレクトリパスとファイル名を渡す
	model->Initialize(modelCommon.get(), directoryPath, fileName);
	// ------------------------------------------------------

	// モデルリストに追加
	models.insert(make_pair(key, move(model)));
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
