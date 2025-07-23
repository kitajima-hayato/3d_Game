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
        Transform transform; 
        std::string fileName;
    };  

    struct PlayerSpawnData {
		Transform transform;
		std::string fileName;
	};

    struct LevelData {  
        std::vector<ObjectData> objects; 
		std::vector<PlayerSpawnData> playerSpawn;
    }; 

    const std::vector<PlayerSpawnData>& getPlayerSpawns()const;
    bool HasPlayerSpawn()const;


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
