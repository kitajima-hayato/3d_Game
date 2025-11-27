#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "BlockType.h"

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
	/// 現在のマップデータの保存
	/// </summary>
	static void SaveMapBlockType(const std::string& folePath,
		const std::vector<std::vector<BlockType>>& mapData);
private:
	static const std::string frontFilePath;

};

