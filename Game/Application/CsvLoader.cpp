#include "CsvLoader.h"
#include "Block.h"


std::vector<std::vector<int>> CsvLoader::LoadMapInt(const std::string& filePath)
{
	// 読み込んだものを格納する変数
	std::vector<std::vector<int>> mapData;
	// ファイルを開く
	std::ifstream file(frontFilePath+filePath);

	// ファイルが開けなかった場合のエラーチェック
	if (!file.is_open()) {
		throw std::runtime_error("CSVファイルが開けません: " + filePath);
	}
	// 1行ずつ読み込む
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
	// ファイルを閉じる
	file.close();
	// 読み込んだデータを返す
	return mapData;

}

std::vector<std::vector<BlockType>> CsvLoader::LoadMapBlockType(const std::string& filePath)
{
	// 読み込んだ物を格納する変数	
	std::vector<std::vector<BlockType>> mapData;
	// ファイルを開く
	std::ifstream file(frontFilePath+filePath);
	// ファイルが開けなかった場合のエラーチェック
	if (!file.is_open()) {
		throw std::runtime_error("CSVファイルが開けません: " + filePath);
	}
	// 1行ずつ読み込む
	std::string line;
	// 各行をカンマで区切って処理
	while (std::getline(file, line)) {
		std::vector<BlockType> row;
		std::stringstream ss(line);
		std::string value;
		// カンマで区切られた各値を処理
		while (std::getline(ss, value, ',')) {
			// 文字列をintに変換し、それをBlockTypeへ変換
			int intVal = std::stoi(value);
			BlockType type = static_cast<BlockType>(intVal);
			row.push_back(type);
		}
		// 行をmapDataに追加
		mapData.push_back(row);
	}

	file.close();
	return mapData;
}

void CsvLoader::SaveMapBlockType(const std::string& filePath,
	const std::vector<std::vector<BlockType>>& mapData)
{
	// frontFilePath を使っているなら読み込みと同じように
	std::string filePathFull = "resources/MapData/" + filePath;
	std::ofstream file(filePathFull);

	if (!file.is_open()) {
		throw std::runtime_error("CSVファイルを書き込み用に開けません: " + filePath);
	}

	// ※ Excel などで開く予定があるなら BOM を付けたい場合もある
	// file << "\xEF\xBB\xBF"; // 必要ならコメントアウト外す

	const uint32_t height = static_cast<uint32_t>(mapData.size());
	for (uint32_t y = 0; y < height; ++y) {
		const uint32_t width = static_cast<uint32_t>(mapData[y].size());
		for (uint32_t x = 0; x < width; ++x) {
			int val = static_cast<int>(mapData[y][x]);
			file << val;
			if (x < width - 1) {
				file << ",";        // カンマ区切り
			}
		}
		file << "\n";               // 行ごとに改行
	}
}

