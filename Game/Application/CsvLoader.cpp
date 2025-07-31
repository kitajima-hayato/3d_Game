#include "CsvLoader.h"
#include "Block.h"


std::vector<std::vector<int>> CsvLoader::LoadMapInt(const std::string& filePath)
{
	/// 読み込んだものを格納する変数
	std::vector<std::vector<int>> mapData;
	/// ファイルを開く
	std::ifstream file(frontFilePath+filePath);

	/// ファイルが開けなかった場合のエラーチェック
	if (!file.is_open()) {
		throw std::runtime_error("CSVファイルが開けません: " + filePath);
	}

	std::string line;
	while (std::getline(file, line)) {
		std::vector<int> row;
		std::stringstream ss(line);
		std::string value;

		while (std::getline(ss, value, ',')) {

			// 文字列を整数に変換
			row.push_back(std::stoi(value));

		}
		mapData.push_back(row);
	}
	/// ファイルを閉じる
	file.close();
	/// 読み込んだデータを返す
	return mapData;

}

std::vector<std::vector<BlockType>> CsvLoader::LoadMapBlockType(const std::string& filePath)
{
	std::vector<std::vector<BlockType>> mapData;
	std::ifstream file(frontFilePath+filePath);

	if (!file.is_open()) {
		throw std::runtime_error("CSVファイルが開けません: " + filePath);
	}

	std::string line;
	while (std::getline(file, line)) {
		std::vector<BlockType> row;
		std::stringstream ss(line);
		std::string value;

		while (std::getline(ss, value, ',')) {
			// 文字列をintに変換し、それをBlockTypeへ変換
			int intVal = std::stoi(value);
			BlockType type = static_cast<BlockType>(intVal);
			row.push_back(type);
		}

		mapData.push_back(row);
	}

	file.close();
	return mapData;
}

