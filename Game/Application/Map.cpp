#include "Map.h"



void Map::Initialize()
{
	// マップデータの初期化
	mapChipData_.mapData.resize(kMapHeight, std::vector<BlockType>(kMapWidth, BlockType::Air));

}


void Map::Update()
{
	// マップの更新処理が必要な場合はここに追加します
}

void Map::Draw()
{
	// マップの描画処理が必要な場合はここに追加します
}

void Map::Finalize()
{
	// マップの終了処理が必要な場合はここに追加します
}

void Map::GenerareMapBlock()
{
}





IndexSet Map::GetMapChipIndexSetByPosition(const Vector3& position)
{
	// 指定座標がマップチップの何番に該当するかを取得
	IndexSet indexSet{};
	indexSet.xIndex = static_cast<uint32_t>((position.x + kBlockWidth / 2) / kBlockWidth);
	indexSet.yIndex = kMapHeight - 1 - static_cast<uint32_t>((position.y + kBlockHeight / 2) / kBlockHeight);
	return indexSet;
}

BlockType Map::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex)
{
	// インデックスがマップ外なら空気ブロックを返す
	if (xIndex < 0 || kBlockWidth - 1 < xIndex) {
		return BlockType::Air;
	}
	// インデックスがマップ外なら空気ブロックを返す
	if (yIndex < 0 || kMapHeight - 1 < yIndex) {
		return BlockType::Air;
	}
	// 指定インデックスのマップチップの種類を返す
	return mapChipData_.mapData[yIndex][xIndex];
}

Rect Map::GetRectByIndex(uint32_t xIndex, uint32_t yIndex)
{
	Vector3 center = GetMapChipPositionByIndex(xIndex, yIndex);
	Rect rect{};
	rect.left = center.x - (kBlockWidth / 2);
	rect.right = center.x + (kBlockWidth / 2);
	rect.bottom = center.y - (kBlockHeight / 2);
	rect.top = center.y + (kBlockHeight / 2);
	return rect;
}

Vector3 Map::GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex)
{
	return Vector3(kBlockWidth * xIndex, kBlockHeight * (kMapHeight - 1 - yIndex), 0.0f);
}
