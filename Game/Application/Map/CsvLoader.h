#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "Game/Application/Block/BlockType.h"
#include "Game/Application/Enemy/EnemyType.h"

class CsvLoader
{
public:
	CsvLoader() = default;
	~CsvLoader() = default;
	/// <summary>
	/// csvファイルの読み込み
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	std::vector<std::vector<int>> LoadMapInt(const std::string& filePath);
	std::vector<std::vector<BlockType>> LoadMapBlockType(const std::string& filePath);

	/// <summary>
	/// エネミーレイヤーデータの読み込み
	/// </summary>
	std::vector<std::vector<EnemyType>> LoadMapEnemyType(const std::string& filePath);

	/// <summary>
	/// 現在のマップデータの保存
	/// </summary>
	static void SaveMapBlockType(const std::string& filePath,
		const std::vector<std::vector<BlockType>>& mapData);

	static void SaveMapEnemyType(const std::string& filePath,
		const std::vector<std::vector<EnemyType>>& enemyData);
private:
	static const std::string frontFilePath;
	static const std::string extensionCsv_;

};

