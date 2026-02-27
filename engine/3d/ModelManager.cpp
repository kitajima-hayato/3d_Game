#include "ModelManager.h"
#include "Logger.h"
#include <filesystem>

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

static void LogModelLoadError(const std::string& message)
{
	Logger::Log(message.c_str());

}

void ModelManager::LoadModel(string_view modelDir)
{
    const std::string key(modelDir);

    // 既に読み込み済みなら何もしない
    if (models.contains(key)) {
        return;
    }

    // ---- ガード：旧仕様(.obj指定)が残っていたら即わかるようにする ----
    if (key.find(".obj") != std::string::npos || key.find(".OBJ") != std::string::npos) {
        // "cubeR.obj" -> "cubeR" にする例を自動生成
        std::string hint = key;
        auto pos = hint.rfind('.');
        if (pos != std::string::npos) {
            hint = hint.substr(0, pos);
        }

        std::string msg =
            "[ModelManager::LoadModel] ERROR: LoadModel expects a directory path under resources/, "
            "but got an .obj path: \"" + key + "\". "
            "Hint: remove the extension. Example: LoadModel(\"" + hint + "\") instead of LoadModel(\"" + key + "\").";
        LogModelLoadError(msg);
        assert(false);
        return;
    }

    // ---- resources/<modelDir> を探す ----
    // modelDir が "Player/" のように末尾スラッシュがあってもOKにしたい場合は fs で吸収されます
    const filesystem::path base = filesystem::path("resources");
    const filesystem::path dirPath = base / filesystem::path(key);

    if (!filesystem::exists(dirPath) || !filesystem::is_directory(dirPath)) {
        std::string msg =
            "[ModelManager::LoadModel] ERROR: Model directory not found: \"" + dirPath.string() +
            "\" (argument=\"" + key + "\")";
        LogModelLoadError(msg);
        assert(false);
        return;
    }

    // ---- フォルダ内の .obj を列挙（1つだけの想定） ----
    std::vector<filesystem::path> objFiles;
    for (const auto& entry : filesystem::directory_iterator(dirPath)) {
        if (!entry.is_regular_file()) { continue; }
        const filesystem::path p = entry.path();
        if (p.has_extension() && (p.extension() == ".obj" || p.extension() == ".OBJ")) {
            objFiles.push_back(p);
        }
    }

    if (objFiles.empty()) {
        std::string msg =
            "[ModelManager::LoadModel] ERROR: No .obj file found in directory: \"" + dirPath.string() +
            "\" (argument=\"" + key + "\")";
        LogModelLoadError(msg);
        assert(false);
        return;
    }
    if (objFiles.size() >= 2) {
        std::string msg =
            "[ModelManager::LoadModel] ERROR: Multiple .obj files found in directory: \"" + dirPath.string() +
            "\" (argument=\"" + key + "\"). Files:";
        for (const auto& p : objFiles) {
            msg += " " + p.filename().string();
        }
        LogModelLoadError(msg);
        assert(false);
        return;
    }

    // ---- 読み込み対象objを確定 ----
    const filesystem::path objPath = objFiles[0];
    const std::string directoryPath = objPath.parent_path().string(); // "resources/Player"
    const std::string fileName = objPath.filename().string();         // "Player.obj"

    // ---- モデル生成 & 初期化 ----
    std::unique_ptr<Model> model = std::make_unique<Model>();
    model->Initialize(modelCommon.get(), directoryPath, fileName);

    // ---- 登録（キーはフォルダ指定のまま�� ----
    models.insert(std::make_pair(key, std::move(model)));
}

Model* ModelManager::FindModel(string_view modelDir)
{
    const std::string key(modelDir);
    if (models.contains(key)) {
        return models.at(key).get();
    }
    return nullptr;
}
