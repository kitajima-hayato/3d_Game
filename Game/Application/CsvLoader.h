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

private:
	std::string frontFilePath = "resources/MapData/";

};

