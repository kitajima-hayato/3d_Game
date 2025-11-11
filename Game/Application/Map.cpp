#include "Map.h"
#include "engine/bace/ImGuiManager.h"


void Map::Initialize()
{
	// マップデータの初期化
	mapChipData_.mapData.resize(kMapHeight, std::vector<BlockType>(kMapWidth, BlockType::Air));

	LoadMapData("TestStage.csv");

	// マップブロックの生成
	GenerareMapBlock();

	// マップデータの変更検知
	previousMapData_ = mapChipData_;
}


void Map::Update()
{
	// マップの更新
	for(std::vector <Block*>& row : blockArray_) {
		for (Block* block : row) {
			if (!block) {
				continue;
			}
			block->Update();
		}
	}
#ifdef _DEBUG
	// ImGuiでマップ編集
	//ImGui::Begin("Map Editor");
	//// マップチップの種類を選択するコンボボックス
	//static int selectedBlockType = 0;
	//ImGui::Combo("Block Type", &selectedBlockType, "Air\0NormalBlock\0testBlock\0kGoalUp\0kGoalDown\0breakBlock\0moveBlock\0sandBlock\0");
	//// マップチップの編集
	//for (uint32_t y = 0; y < kMapHeight; ++y) {
	//	for (uint32_t x = 0; x < kMapWidth; ++x) {
	//		BlockType& blockType = mapChipData_.mapData[y][x];
	//		int currentType = static_cast<int>(blockType);
	//		ImGui::PushID(y * kMapWidth + x); // ユニークIDを設定
	//		if (ImGui::Combo("", &currentType, "Air\0NormalBlock\0testBlock\0kGoalUp\0kGoalDown\0breakBlock\0moveBlock\0sandBlock\0")) {
	//			blockType = static_cast<BlockType>(currentType);
	//			isMapDataChanged_ = true;
	//		}
	//		ImGui::PopID();
	//		if ((x + 1) % 10 != 0) {
	//			ImGui::SameLine();
	//		}
	//	}
	//}
	//ImGui::End();
#endif // _DEBUG
	
}
void Map::Draw()
{
	// マップの描画
	for (std::vector <Block*>& row : blockArray_) {
		for (Block* block : row) {
			if (!block) {
				continue;
			}
			block->Draw();
		}
	}
}

void Map::Finalize()
{
	// マップブロックの解放
	for (std::vector <Block*>& row : blockArray_) {
		for (Block* block : row) {
			if (block) {
				delete block;
				block = nullptr;
			}
		}
	}
}

void Map::GenerareMapBlock()
{
	const uint32_t h = GetHeight();
	const uint32_t w = GetWidth();

	// ブロック配列を実サイズで作り直す
	blockArray_.assign(h, std::vector<Block*>(w, nullptr));

	for (uint32_t y = 0; y < h; ++y) {
		for (uint32_t x = 0; x < w; ++x) {
			const BlockType type = mapChipData_.mapData[y][x];
			if (type == BlockType::Air) continue;

			const Vector3 pos = GetMapChipPositionByIndex(x, y);
			blockArray_[y][x] = Block::CreateBlock(type, pos);
		}
	}
}

void Map::LoadMapData(const char* filePath)
{
	CsvLoader csvLoader;
	mapChipData_.mapData = csvLoader.LoadMapBlockType(filePath);


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
	const uint32_t w = GetWidth();
	const uint32_t h = GetHeight();
	if (xIndex >= w || yIndex <= h) return BlockType::Air; 
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
	return Vector3(kBlockWidth * xIndex,
		kBlockHeight * yIndex,
		0.0f);
}

