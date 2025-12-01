#include "Map.h"
#ifdef USE_IMGUI
#include "engine/bace/ImGuiManager.h"
#endif
#include <algorithm>

#ifdef USE_IMGUI
// ブロックタイプに応じた色を取得するヘルパー関数
ImVec4 GetBlockColorByType(BlockType blockType) {
	switch (blockType) {
	case BlockType::Air:         return ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	case BlockType::NormalBlock: return ImVec4(0.4f, 0.4f, 0.8f, 1.0f);
	case BlockType::testBlock:   return ImVec4(0.8f, 0.4f, 0.4f, 1.0f);
	case BlockType::kGoalUp:     return ImVec4(0.4f, 0.8f, 0.4f, 1.0f);
	case BlockType::kGoalDown:   return ImVec4(0.4f, 0.8f, 0.8f, 1.0f);
	case BlockType::breakBlock:  return ImVec4(0.8f, 0.8f, 0.4f, 1.0f);
	case BlockType::moveBlock:   return ImVec4(0.8f, 0.4f, 0.8f, 1.0f);
	case BlockType::sandBlock:   return ImVec4(0.7f, 0.6f, 0.3f, 1.0f);
	default:                     return ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	}
}
#endif

void Map::Initialize()
{
	// マップデータの初期化
	mapChipData_.mapData.resize(kMapHeight, std::vector<BlockType>(kMapWidth, BlockType::Air));

	// マップデータの読み込み
	LoadMapData("1-1.csv");

	// マップブロックの生成
	GenerareMapBlock();

	// マップデータの変更検知
	previousMapData_ = mapChipData_;
}


void Map::Update()
{
	// マップの更新
	for (std::vector <Block*>& row : blockArray_) {
		for (Block* block : row) {
			if (!block) {
				continue;
			}
			block->Update();
		}
	}
#ifdef USE_IMGUI
	// ImGuiによるデバッグ情報表示

	ImGui::Begin("Map Info");
	ImGui::Text("Map Width: %d", GetWidth());
	ImGui::Text("Map Height: %d", GetHeight());
	ImGui::End();

	// マップデータの編集　
	ImGui::Begin("Map Editor");

	// 1. ブロックタイプ選択用のコンボ
	static int currentTypeInt = 0; // 今ペイントするブロック
	const char* blockTypeNames[] = {
		"Air",
		"NormalBlock",
		"TestBlock",
		"GoalUp",
		"GoalDown",
		"BreakBlock",
		"MoveBlock",
		"SandBlock",
	};

	ImGui::Text("Paint Type:");
	ImGui::SameLine();
	ImGui::Combo("##PaintType",
		&currentTypeInt,
		blockTypeNames,
		IM_ARRAYSIZE(blockTypeNames));

	// 選択中のブロックの色のプレビュー
	ImGui::SameLine();
	ImGui::Text("BlockColor:");
	ImGui::SameLine();

	// currentTypeInt をBlockTypeにキャストして色を取得
	BlockType currentType = static_cast<BlockType>(currentTypeInt);
	ImVec4 previewColor = GetBlockColorByType(currentType);

	// 閲覧用の小さいボタンを表示 / 応答はなし
	ImGui::PushStyleColor(ImGuiCol_Button, previewColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, previewColor);
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, previewColor);
	ImGui::Button("##PaintPreview", ImVec2(24.0f, 24.0f));
	ImGui::PopStyleColor(3);

	// ツールチップで種類名も出す
	if (ImGui::IsItemHovered()) {
		ImGui::SetTooltip("%s", blockTypeNames[currentTypeInt]);
	}

	// 2. マップ本体のグリッド表示（スクロールできるように Child にする）
	ImGui::Separator();
	ImGui::Text("Map Grid");
	ImGui::BeginChild("MapGrid", ImVec2(0, 0), true, ImGuiWindowFlags_HorizontalScrollbar);

	const float cellSize = 20.0f; // 1マスのサイズ（お好みで）

	for (uint32_t y = 0; y < GetHeight(); ++y) {
		for (uint32_t x = 0; x < GetWidth(); ++x) {
			BlockType& blockType = mapChipData_.mapData[y][x];

			// 3. 色をブロックタイプで変えると見やすい
			ImVec4 color = GetBlockColorByType(blockType);


			ImGui::PushID(static_cast<int>(y * GetWidth() + x)); // ID被り防止
			ImGui::PushStyleColor(ImGuiCol_Button, color);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(color.x + 0.1f, color.y + 0.1f, color.z + 0.1f, 1.0f));
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(color.x * 0.8f, color.y * 0.8f, color.z * 0.8f, 1.0f));

			// ラベルは非表示 ("##cell") にして、ボタンだけ並べる
			if (ImGui::Button("##cell", ImVec2(cellSize, cellSize))) {
				blockType = static_cast<BlockType>(currentTypeInt);
				isMapDataChanged_ = true;
			}

			// マウスを乗せた時に情報を出す
			if (ImGui::IsItemHovered()) {
				ImGui::SetTooltip("(%u, %u)\n%s", x, y, blockTypeNames[static_cast<int>(blockType)]);
			}

			ImGui::PopStyleColor(3);
			ImGui::PopID();

			// 横に並べる
			if (x < GetWidth() - 1) {
				ImGui::SameLine();
			}
		}
	}

	ImGui::EndChild();
	static char saveFileName[256] = "stage01.csv"; // 初期値はお好みで
	ImGui::InputText("Save File", saveFileName, IM_ARRAYSIZE(saveFileName));

	static std::string saveMessage;

	if (ImGui::Button("Save CSV")) {
		try {
			// mapChipData_.mapData をそのまま渡す
			CsvLoader::SaveMapBlockType(saveFileName, mapChipData_.mapData);
			saveMessage = std::string("Saved: ") + saveFileName;
		}
		catch (const std::exception& e) {
			saveMessage = std::string("Save Failed: ") + e.what();
		}
	}


	if (!saveMessage.empty()) {
		ImGui::Text("%s", saveMessage.c_str());
	}


	// 4. マップデータが変更された場合、マップブロックを再生成
	if (isMapDataChanged_) {
		GenerareMapBlock();
		isMapDataChanged_ = false;
	}

	ImGui::End();


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

	// ① 既存ブロックを全て解放
	for (auto& row : blockArray_) {
		for (Block*& block : row) {
			if (block) {
				delete block;
				block = nullptr;
			}
		}
	}
	// ポインタを解放したので配列自体もクリア
	blockArray_.clear();

	// ② 新しいサイズで配列を作り直す
	blockArray_.assign(h, std::vector<Block*>(w, nullptr));

	// ③ マップデータからブロックを生成
	for (uint32_t y = 0; y < h; ++y) {
		for (uint32_t x = 0; x < w; ++x) {
			const BlockType type = mapChipData_.mapData[y][x];
			if (type == BlockType::Air) {
				continue;
			}

			Vector3 pos = GetMapChipPositionByIndex(x, y);
			pos.x += blockOffset_;
			pos.y -= blockOffset_;
			blockArray_[y][x] = Block::CreateBlock(type, pos);
		}
	}
}


void Map::LoadMapData(const char* filePath)
{
	// CSVファイルからマップデータを読み込む
	CsvLoader csvLoader;
	mapChipData_.mapData = csvLoader.LoadMapBlockType(filePath);
}





IndexSet Map::GetMapChipIndexSetByPosition(const Vector3& position)
{
	IndexSet indexSet{};

	// 座標からインデックスを計算
	float x = std::floor(position.x / kBlockWidth);
	float y = std::floor((GetHeight() - 1) - (position.y / kBlockHeight));

	// 座標がマップ外の場合は最大値を返す
	int ix = static_cast<int>(x);
	int iy = static_cast<int>(y);

	// マップ外チェック / マップ外なら範囲外を示すインデックスを返す
	if (ix < 0 || ix >= static_cast<int>(GetWidth()) ||
		iy < 0 || iy >= static_cast<int>(GetHeight())) {
		indexSet.xIndex = GetWidth();
		indexSet.yIndex = GetHeight();
		return indexSet;
	}
	// インデックスをセット
	indexSet.xIndex = static_cast<uint32_t>(ix);
	indexSet.yIndex = static_cast<uint32_t>(iy);

	// インデックスを返す
	return indexSet;
}

BlockType Map::GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex)
{
	// マップ外チェック
	uint32_t w = GetWidth();
	uint32_t h = GetHeight();
	// マップ外ならAirを返す
	if (xIndex >= w || yIndex >= h) {
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
	const uint32_t h = GetHeight();
	return Vector3(kBlockWidth * xIndex,
		kBlockHeight * (h - 1 - yIndex),
		0.0f);
}

