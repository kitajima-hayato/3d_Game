#include "LevelLoader.h"
#include "ImguiManager.h"
#include "ModelManager.h"
void LevelLoader::Load(const std::string& fileName)
{
	const std::string kDefaultBaseDirectory = "resources/levels/stage/";
	const std::string kExtension = ".json";
	const std::string fullPath = kDefaultBaseDirectory + fileName + kExtension;
	const std::string objPath = ".obj";

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
			
			/// 無効フラグがある場合はスキップ
			if (object.contains("disabled_flag") && object["disabled_flag"].get<bool>()) {
				continue;
			}
			
			/// ObjectData要素追加
			levelData->objects.emplace_back(LevelLoader::ObjectData{});
			/// 追加した要素の参照を得る
			LevelLoader::ObjectData& objectData = levelData->objects.back();
			
			if (object.contains("file_name")) {
				objectData.fileName = object["file_name"];
			}
			else if (object.contains("name")) {
				objectData.fileName = object["name"];
			}
			/// ファイル名に拡張子を追加
			objectData.fileName += objPath;


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
		else if (type.compare("PlayerSpawn") == 0) {

			/// 無効フラグがある場合はスキップ
			if (object.contains("disabled_flag") && object["disabled_flag"].get<bool>()) {
				continue;
			}

			/// 新しい PlayerSpawnData を追加
			levelData->playerSpawn.emplace_back(LevelLoader::PlayerSpawnData{});
			LevelLoader::PlayerSpawnData& spawnData = levelData->playerSpawn.back();

			/// ファイル名の読み込み
			if (object.contains("file_name")) {
				spawnData.fileName = object["file_name"];
			} else if (object.contains("name")) {
				spawnData.fileName = object["name"];
			}

			/// 必要に応じて拡張子を追加（例えば ".obj"）
			spawnData.fileName += objPath;

			// Transform
			const auto& transform = object["transform"];
			// Translate
			spawnData.transform.translate.x = static_cast<float>(transform["translation"][0]);
			spawnData.transform.translate.y = static_cast<float>(transform["translation"][2]);
			spawnData.transform.translate.z = static_cast<float>(transform["translation"][1]);
			// Rotate
			spawnData.transform.rotate.x = static_cast<float>(transform["rotation"][0]);
			spawnData.transform.rotate.y = static_cast<float>(transform["rotation"][2]);
			spawnData.transform.rotate.z = static_cast<float>(transform["rotation"][1]);

			
		}
		else if (type.compare("EnemySpawn") == 0) {
			/// 無効フラグがある場合はスキップ
			if (object.contains("disabled_flag") && object["disabled_flag"].get<bool>()) {
				continue;
			}
			/// 新しい EnemySpawnData を追加
			levelData->enemySpawn.emplace_back(LevelLoader::EnemySpawnData{});
			LevelLoader::EnemySpawnData& enemyData = levelData->enemySpawn.back();
			/// ファイル名の読み込み
			if (object.contains("file_name")) {
				enemyData.fileName = object["file_name"];
			} else if (object.contains("name")) {
				enemyData.fileName = object["name"];
			}
			/// 必要に応じて拡張子を追加（例えば ".obj"）
			enemyData.fileName += objPath;
			// Transform
			const auto& transform = object["transform"];
			// Translate
			enemyData.transform.translate.x = static_cast<float>(transform["translation"][0]);
			enemyData.transform.translate.y = static_cast<float>(transform["translation"][2]);
			enemyData.transform.translate.z = static_cast<float>(transform["translation"][1]);
			// Rotate
			enemyData.transform.rotate.x = static_cast<float>(transform["rotation"][0]);
			enemyData.transform.rotate.y = static_cast<float>(transform["rotation"][2]);
			enemyData.transform.rotate.z = static_cast<float>(transform["rotation"][1]);

			enemyData.transform.scale.x = 1.0f; // デフォルトスケール
			enemyData.transform.scale.y = 1.0f; // デフォルトスケール
			enemyData.transform.scale.z = 1.0f; // デフォルトスケール
			
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
		// モデルを指定して3Dオブジェクトを生成
		std::unique_ptr<Object3D> newObject = std::make_unique<Object3D>();
		newObject->Initialize();
		newObject->SetModel(objectData.fileName);
		newObject->SetTransform(objectData.transform);
		// 登録
		objects.push_back(std::move(newObject));
	}
}



void LevelLoader::Update()
{
	for (auto& object : objects) {
		if (object) {
#ifdef _DEBUG
			// ImGuiで座標変更UIを表示
			ImGui::Begin("Object Transform");

			Vector3 pos = object->GetTranslate();
			if (ImGui::DragFloat3("Position", &pos.x, 0.1f)) {
				object->SetTranslate(pos);
			}

			Vector3 rot = object->GetRotate();
			if (ImGui::DragFloat3("Rotation", &rot.x, 0.1f)) {
				object->SetRotate(rot);
			}

			Vector3 scale = object->GetScale();
			if (ImGui::DragFloat3("Scale", &scale.x, 0.1f)) {
				object->SetScale(scale);
			}

			ImGui::End();
#endif
			object->Update();
		}
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


const std::vector<LevelLoader::PlayerSpawnData>& LevelLoader::GetPlayerSpawns() const
{
	assert(levelData);
	return levelData->playerSpawn;
}

bool LevelLoader::HasPlayerSpawn() const
{
	assert(levelData);
	return !levelData->playerSpawn.empty();
}

const std::vector<LevelLoader::EnemySpawnData>& LevelLoader::GetEnemySpawns() const
{
	assert(levelData);
	return levelData->enemySpawn;
}

uint32_t LevelLoader::GetEnemySpawnCount() const
{
	assert(levelData);
	return static_cast<uint32_t>(levelData->enemySpawn.size());
}
