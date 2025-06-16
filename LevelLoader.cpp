#include "LevelLoader.h"
#include "ImguiManager.h"
void LevelLoader::Load(const std::string& fileName)
{
	const std::string kDefaultBaseDirectory = "resources/levels/stage/";
	const std::string kExtension = ".json";
	const std::string fullPath = kDefaultBaseDirectory + fileName + kExtension;

	/// ファイルストリーム
	std::ifstream file;

	/// ファイルを開く
	file.open(fullPath);
	/// ファイルオープンの成否
	if (file.fail()) {
		assert(0);
	}


	/// JSON文字列
	nlohmann::json deserialized;
	/// パース(解凍)
	file >> deserialized;
	
	/// 正しいレベルデータファイルか確認
	assert(deserialized.is_object());
	assert(deserialized.contains("name"));
	assert(deserialized["name"].is_string());

	/// "name"を文字列として取得
	std::string name = deserialized["name"].get<std::string>();

	/// 正しいレベルデータファイル
	assert(name.compare("scene") == 0);

	/// レベルデータ格納用インスタンスを生成
	levelData = new LevelData();

	/// "Objects"の全オブジェクトを走査
	for (nlohmann::json& object : deserialized["objects"]) {
		assert(object.contains("type"));

		/// 種別を取得
		std::string type = object["type"].get<std::string>();


		// MESH
		if (type.compare("MESH") == 0) {
			/// 要素追加
			levelData->objects.emplace_back(LevelLoader::ObjectData{});
			/// 追加した要素の参照を得る
			LevelLoader::ObjectData& objectData = levelData->objects.back();

			if (object.contains("file_name")) {
				objectData.fileName = object["file_name"];
			} else if (object.contains("name")) {
				objectData.fileName = object["name"]; 
			}


			/// トランスフォームのパラメータ読み込み
			nlohmann::json& transform = object["transform"];
			// Translate
			objectData.transform.translate.x = (float)transform["translation"][0];
			objectData.transform.translate.y = (float)transform["translation"][2];
			objectData.transform.translate.z = (float)transform["translation"][1];
			// Rotate
			objectData.transform.rotate.x = (float)transform["rotation"][0];
			objectData.transform.rotate.y = (float)transform["rotation"][2];
			objectData.transform.rotate.z = (float)transform["rotation"][1];
			// Scale
			objectData.transform.scale.x = (float)transform["scaling"][0];
			objectData.transform.scale.y = (float)transform["scaling"][2];
			objectData.transform.scale.z = (float)transform["scaling"][1];


		}


		/// @ オブジェクト走査を再帰関数にまとめ、再帰呼び出しで枝を走査する
		if (object.contains("Children")) {

		}
	}
}

void LevelLoader::CreateObject()
{
	// オブジェクトの生成、配置
	for (auto& objectData : levelData->objects) {
		// ファイル名から登録済みのモデルを検索
		auto it = models.find(objectData.fileName);
		if (it == models.end()) {
			// モデルが見つからなければスキップ
			continue;
		}

		Model* model = it->second;

		// モデルを指定して3Dオブジェクトを生成
		std::unique_ptr<Object3D> newObject = std::make_unique<Object3D>();
		newObject->Initialize();
		newObject->SetModel(model);
		newObject->SetTransform(objectData.transform);

		// 登録
		objects.push_back(std::move(newObject));
	}
}



void LevelLoader::Update()
{
	/// オブジェクトの更新
	for (auto& object : objects) {
		object->Update();
	}
}

void LevelLoader::Draw()
{
#ifdef _DEBUG

#endif

	/// オブジェクトの描画
	for (auto& object : objects) {
		object->Draw();
	}
}
