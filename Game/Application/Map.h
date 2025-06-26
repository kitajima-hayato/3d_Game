#pragma once
#include "Block.h"

class CsvLoader;
class Map
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(std::string csvFilePath);

	/// <summary>
	/// 更新処理
	/// <summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 読み込んだマップデータを基にブロックを生成し配置する
	/// </summary>
	void CreateBlocksMap();

public:	// Setter / Getter
	/// <summary>
	/// csvファイルのパスを設定する
	/// </summary>
	void SetCsvFilePath(const std::string& path) { this->csvFilePath = path; }
private:
	/// CSVファイルを読み込む
	std::vector<std::vector<int>> mapData;

	/// CSVファイルのパス
	std::string csvFilePath;
	/// ブロックのリスト
	std::vector<std::vector<std::unique_ptr<Block>>> blocks;





};
