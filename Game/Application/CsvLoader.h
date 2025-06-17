#pragma once
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
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
	std::vector<std::vector<int>> LoadMap(const std::string& filePath);
};

