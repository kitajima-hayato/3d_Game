#pragma once  
#include <string>  
#include <fstream>  
#include <cassert>  
#include "resources/levels/json.hpp"  
#include "engine/math/MyMath.h"  
#include "engine/3d/Model.h"
#include "engine/3d/Object3D.h"
class LevelLoader  
{  
public:  
    struct ObjectData {  
        std::string type;  
        Transform transform; 
        std::string fileName;
    };  

    struct LevelData {  
        std::vector<ObjectData> objects;  
    };  

public:  
    /// JSONファイルを読み込む  
    void Load(const std::string& jsonFilePath);  

    ///
    void CreateObject();
    /// <summary>
	/// 更新処理
    /// </summary>
    void Update();
	/// <summary>
	///  描画処理
	/// </summary>
	void Draw();



private:
    LevelData* levelData;

    std::unordered_map<std::string, Model*> models;
	std::vector<std::unique_ptr<Object3D>> objects;

};
