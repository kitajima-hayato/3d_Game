#include "CsvLoader.h"

std::vector<std::vector<int>> CsvLoader::LoadMap(const std::string& filePath)
{
	/// 読み込んだものを格納する変数
	std::vector<std::vector<int>> mapData;
	/// ファイルを開く
	std::ifstream file(filePath);

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
