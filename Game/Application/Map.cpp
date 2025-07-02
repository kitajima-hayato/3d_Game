#include "Map.h"
#include "CsvLoader.h"

void Map::Initialize(std::string csvFilePath) {
	/// csvファイルを読み込む
	this->csvFilePath = csvFilePath;
	CreateBlocksMap();
}

void Map::Update() {
	for (auto& row : blocks) {
		for (auto& block : row) {
			if (block) {
				block->Update();
			}
		}
	}
}

void Map::Draw() {
	for(auto& row : blocks) {
		for (auto& block : row) {
			if (block) {
				block->Draw();
			}
		}
	}
}

void Map::CreateBlocksMap()
{
	/// CSVファイルからマップデータを読み込む
	CsvLoader csvLoader;
	mapData = csvLoader.LoadMapInt(this->csvFilePath);
	/// 読み込んだデータをもとにブロックを生成する
	/// マップの大きさを取得
	const Vector3 initPos = { 0.0f,0.0f,0.0f };
	const Vector3 blockSize = { 1.0f,1.0f,1.0f };
	const size_t mapHeight = mapData.size();
	const size_t mapWidth = mapData[0].size();

	/// ブロック格納領域を確保
	blocks.resize(mapHeight);
	for (size_t y = 0; y < mapHeight; ++y) {
		blocks[y].resize(mapWidth);
	}

	for (auto height = 0; height < mapHeight; ++height) {
		for (auto width = 0; width < mapWidth; ++width) {
			/// ブロックの位置を計算
			Transform blockTransform;
			blockTransform.translate = { static_cast<float>(width), static_cast<float>(-height), 20.0f };
			blockTransform.rotate = { 0.0f,0.0f,0.0f };
			blockTransform.scale = blockSize;
			/// ブロックの種類を取得
			int intBlockType = mapData[height][width];
			/// 不正な値は止める
			if (intBlockType < 0 ||
				static_cast<int>(BlockType::Count) <= intBlockType) {
				continue;
			}
			/// BlockTypeに変換
			BlockType blockType = static_cast<BlockType>(intBlockType);

			std::unique_ptr<Block> block = std::make_unique<Block>();
			block->Initialize(blockType,blockTransform.translate);
			block->SetTransform(blockTransform);

			/// ブロックを追加
			blocks[height][width] = std::move(block);

		}
	}
}
