#include "Map.h"
#ifdef USE_IMGUI
#include "engine/base/ImGuiManager.h"
#endif
#include "Game/Particle/ParticleManager.h"

#ifdef USE_IMGUI
// ブロックタイプに応じた色を取得するヘルパー関数
ImVec4 GetBlockColorByType(BlockType blockType) {
	switch (blockType) {
	case BlockType::Air:         return ImVec4(0.2f, 0.2f, 0.2f, 1.0f);
	case BlockType::GrassBlock: return ImVec4(0.4f, 0.4f, 0.8f, 1.0f);
	case BlockType::SoilBlock:   return ImVec4(0.8f, 0.4f, 0.4f, 1.0f);
	case BlockType::kGoalUp:     return ImVec4(0.4f, 0.8f, 0.4f, 1.0f);
	case BlockType::kGoalDown:   return ImVec4(0.4f, 0.8f, 0.8f, 1.0f);
	case BlockType::breakBlock:  return ImVec4(0.8f, 0.8f, 0.4f, 1.0f);
	case BlockType::moveBlock:   return ImVec4(0.8f, 0.4f, 0.8f, 1.0f);
	case BlockType::sandBlock:   return ImVec4(0.7f, 0.6f, 0.3f, 1.0f);
	case BlockType::Unbreakable: return ImVec4(0.3f, 0.3f, 0.3f, 1.0f);
	default:                     return ImVec4(0.5f, 0.5f, 0.5f, 1.0f);
	}
}
#endif

void Map::Initialize(const char* filePath)
{
	// マップデータの初期化
	mapChipData_.mapData.resize(kMapHeight, std::vector<BlockType>(kMapWidth, BlockType::Air));

	// マップ番号の保存
	mapNumber_ = filePath;

	// マップデータの読み込み
	LoadMapData(filePath);

	// マップブロックの生成
	GenerateMapBlock();

	// マップデータの変更検知
	previousMapData_ = mapChipData_;

	ParticleManager::GetInstance()->CreateParticleGroup(
        "BreakParticle", "resources/BackSky.png");
	breakParticleEmitter_ = std::make_unique<ParticleEmitter>();
	breakParticleEmitter_->SetParticleName("BreakParticle");
	breakParticleEmitter_->SetScale({ 0.2f, 0.2f, 0.2f });
    
}


void Map::Update()
{
    //breakParticleEmitter_->Update();
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
    //  ここから ImGui 部分を「1つのタブ付きウィンドウ」に統合

    // 1つのタブにまとめる
    ImGui::Begin("Map Tools"); 

    if (ImGui::BeginTabBar("MapTabs"))
    {
        // タブ1: Map Info
        if (ImGui::BeginTabItem("Info"))
        {
            ImGui::Text("Map Width: %d", GetWidth());
            ImGui::Text("Map Height: %d", GetHeight());
            ImGui::EndTabItem();
        }

        
        // タブ2: Block Editor 
        if (ImGui::BeginTabItem("Block Editor"))
        {
            // ① 一番上に「保存 / ロードまわり」のUI
            static char mapFileName[256] = ".csv"; 
            static std::string mapFileMessage;

            ImGui::Text("Map CSV File:");
            ImGui::SameLine();
            ImGui::InputText("##MapFile", mapFileName, IM_ARRAYSIZE(mapFileName));

            // --- 保存ボタン ---
            if (ImGui::Button("Save Map CSV")) {
                try {
                    CsvLoader::SaveMapBlockType(mapFileName, mapChipData_.mapData);
                    mapFileMessage = std::string("Saved: ") + mapFileName;
                }
                catch (const std::exception& e) {
                    mapFileMessage = std::string("Save Failed: ") + e.what();
                }
            }

            ImGui::SameLine();

            // --- ロードボタン ---
            if (ImGui::Button("Load Map CSV")) {
                try {
                    // "1_1.csv" → "1_1" に変換して、ゲーム本編と同じ形式で読む
                    std::string baseName = mapFileName;
                    size_t dotPos = baseName.rfind('.');
                    if (dotPos != std::string::npos) {
                        baseName = baseName.substr(0, dotPos);
                    }

                    // マップ＆エネミーレイヤーを再読み込み
                    LoadMapData(baseName.c_str());

                    // マップブロックも作り直す
                    GenerateMapBlock();

                    mapFileMessage = std::string("Loaded: ") + mapFileName;
                }
                catch (const std::exception& e) {
                    mapFileMessage = std::string("Load Failed: ") + e.what();
                }
            }

            // メッセージ表示
            if (!mapFileMessage.empty()) {
                ImGui::SameLine();
                ImGui::Text("%s", mapFileMessage.c_str());
            }

            ImGui::Separator();

            // ② ここから下は「編集UI」と「グリッド表示」

            // ブロックタイプ選択用のコンボ
            static int currentTypeInt = 0;
            const char* blockTypeNames[] = {
                "Air",
                "GrassBlock",
                "SoilBlock",
                "GoalUp",
                "GoalDown",
                "BreakBlock",
                "MoveBlock",
                "SandBlock",
				"Unbreakable",
            };

            ImGui::Text("Paint Type:");
            ImGui::SameLine();
            ImGui::Combo("##PaintType",
                &currentTypeInt,
                blockTypeNames,
                IM_ARRAYSIZE(blockTypeNames));

            ImGui::Separator();
            ImGui::Text("Map Grid");

            ImGui::BeginChild("MapGrid", ImVec2(0, 260), true, ImGuiWindowFlags_HorizontalScrollbar);

            const float cellSize = 20.0f;
            uint32_t mapHeight = GetHeight();
            uint32_t mapWidth = GetWidth();

            for (uint32_t y = 0; y < mapHeight; ++y) {
                for (uint32_t x = 0; x < mapWidth; ++x) {

                    BlockType& cell = mapChipData_.mapData[y][x];

                    ImGui::PushID(static_cast<int>(y * mapWidth + x));

                    ImVec4 color = GetBlockColorByType(cell);
                    ImGui::PushStyleColor(ImGuiCol_Button, color);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);

                    std::string label = std::to_string(static_cast<int>(cell));

                    // ボタンは見た目用として押すだけ
                    ImGui::Button(label.c_str(), ImVec2(cellSize, cellSize));

                    // エネミーエディターと同じドラッグ塗り
                    if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                        cell = static_cast<BlockType>(currentTypeInt);
                        isMapDataChanged_ = true;
                    }

                    ImGui::PopStyleColor(3);
                    ImGui::PopID();

                    if (x < mapWidth - 1) {
                        ImGui::SameLine();
                    }
                }
            }

            ImGui::EndChild();


            

            ImGui::EndTabItem();
        }


        // タブ3: Enemy Layer
        if (ImGui::BeginTabItem("Enemy Layer"))
        {
            // エネミータイプ一覧
            static int currentEnemyTypeInt = 0;
            const char* enemyTypeNames[] = {
                "None",
                "NormalEnemy",
                "FlyingEnemy",
            };

            ImGui::Text("Paint Enemy Type:");
            ImGui::SameLine();
            ImGui::Combo("##EnemyType",
                &currentEnemyTypeInt,
                enemyTypeNames,
                IM_ARRAYSIZE(enemyTypeNames));

            // ファイル名入力＋セーブ/ロード
            static char enemyFileName[256] = "stage01_EnemyLayer.csv";
            ImGui::InputText("EnemyLayer File", enemyFileName, IM_ARRAYSIZE(enemyFileName));

            static std::string enemyLayerMessage;

            // セーブボタン
            if (ImGui::Button("Save Enemy CSV")) {
                try {
                    CsvLoader::SaveMapEnemyType(enemyFileName, enemyLayerData_.enemyData);
                    enemyLayerMessage = std::string("Saved: ") + enemyFileName;
                }
                catch (const std::exception& e) {
                    enemyLayerMessage = std::string("Save Failed: ") + e.what();
                }
            }

            ImGui::SameLine();

            // ロードボタン
            if (ImGui::Button("Load Enemy CSV")) {
                try {
                    // 拡張子 .csv を外して Load 用のベース名を作る
                    std::string baseName = enemyFileName;
                    size_t dotPos = baseName.rfind('.');
                    if (dotPos != std::string::npos) {
                        baseName = baseName.substr(0, dotPos);
                    }

                    CsvLoader loader;
                    // LoadMapEnemyType は frontFilePath + baseName + ".csv" を読む
                    enemyLayerData_.enemyData = loader.LoadMapEnemyType(baseName);

                    // 敵レイヤーが更新されたことを記録
                    enemyLayerDirty_ = true;

                    enemyLayerMessage = std::string("Loaded: ") + enemyFileName;
                }
                catch (const std::exception& e) {
                    enemyLayerMessage = std::string("Load Failed: ") + e.what();
                }
            }

            if (!enemyLayerMessage.empty()) {
                ImGui::Text("%s", enemyLayerMessage.c_str());
            }

            // エネミーデータの参照
            auto& enemyLayer = enemyLayerData_.enemyData;

            // グリッド描画前に空チェック
            if (enemyLayer.empty() || enemyLayer[0].empty()) {
                ImGui::Text("Enemy layer is empty or not loaded.");
            } else {
                uint32_t enemyLayerHeight = static_cast<uint32_t>(enemyLayer.size());
                uint32_t enemyLayerWidth = static_cast<uint32_t>(enemyLayer[0].size());

                const float cellSize = 20.0f;

                // エネミーレイヤーグリッド表示
                ImGui::BeginChild("EnemyLayerGrid", ImVec2(0, 260), true, ImGuiWindowFlags_HorizontalScrollbar);

                for (uint32_t y = 0; y < enemyLayerHeight; ++y) {
                    for (uint32_t x = 0; x < enemyLayerWidth; ++x) {

                        EnemyType& cell = enemyLayer[y][x];
                        uint32_t cellInt = static_cast<uint32_t>(cell);

                        ImGui::PushID(static_cast<int>(y * enemyLayerWidth + x));

                        ImVec4 color;
                        switch (cell) {
                        case EnemyType::NormalEnemy: color = ImVec4(0.2f, 0.8f, 0.2f, 1.0f); break;
                        case EnemyType::FlyingEnemy: color = ImVec4(0.2f, 0.4f, 1.0f, 1.0f); break;
                        default:                     color = ImVec4(0.4f, 0.4f, 0.4f, 1.0f); break;
                        }

                        ImGui::PushStyleColor(ImGuiCol_Button, color);
                        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
                        ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);

                        std::string label = std::to_string(cellInt);
                        ImGui::Button(label.c_str(), ImVec2(cellSize, cellSize));

                        // 左ドラッグ中にカーソルが乗っていたら塗る
                        if (ImGui::IsItemHovered() && ImGui::IsMouseDown(ImGuiMouseButton_Left)) {
                            cell = static_cast<EnemyType>(currentEnemyTypeInt);
                        }

                        ImGui::PopStyleColor(3);
                        ImGui::PopID();

                        if (x < enemyLayerWidth - 1) {
                            ImGui::SameLine();
                        }
                    }
                }

                ImGui::EndChild();
            }

            ImGui::EndTabItem();
        }
		// タブ4: Map Preview
        if (ImGui::BeginTabItem("Map Preview"))
        {
            ImGui::Text("Map Preview (Read-Only)");
            ImGui::BeginChild("MapPreview", ImVec2(0, 400), true, ImGuiWindowFlags_HorizontalScrollbar);
            const float cellSize = 20.0f;
            uint32_t mapHeight = GetHeight();
            uint32_t mapWidth = GetWidth();
            for (uint32_t y = 0; y < mapHeight; ++y) {
                for (uint32_t x = 0; x < mapWidth; ++x) {
                    BlockType cell = mapChipData_.mapData[y][x];
                    ImGui::PushID(static_cast<int>(y * mapWidth + x));
                    ImVec4 color = GetBlockColorByType(cell);
                    ImGui::PushStyleColor(ImGuiCol_Button, color);
                    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, color);
                    ImGui::PushStyleColor(ImGuiCol_ButtonActive, color);
                    std::string label = std::to_string(static_cast<int>(cell));
                    // プレビュー用ボタン（押せない）
                    ImGui::Button(label.c_str(), ImVec2(cellSize, cellSize));
                    ImGui::PopStyleColor(3);
                    ImGui::PopID();
                    if (x < mapWidth - 1) {
                        ImGui::SameLine();
                    }
                }
            }
            ImGui::EndChild();
            ImGui::EndTabItem();
		}

        ImGui::EndTabBar();
    }

    ImGui::End(); // "Map Tools"

    // ▲▲▲ ImGui 統合ここまで ▲▲▲

#endif // USE_IMGUI
    // 末尾に追加：死んだブロックだけ回収
    for (auto& row : blockArray_) {
        for (Block*& block : row) {
            if (block && !block->GetAliveBlock()) {
                delete block;
                block = nullptr;
            }
        }
    }

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

void Map::GenerateMapBlock()
{
	const uint32_t h = GetHeight();
	const uint32_t w = GetWidth();

	// 既存のブロックを解放
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

	// 新しいサイズで配列を作り直す
	blockArray_.assign(h, std::vector<Block*>(w, nullptr));

	// マップデータからブロックを生成
	for (uint32_t y = 0; y < h; ++y) {
		for (uint32_t x = 0; x < w; ++x) {
			const BlockType type = mapChipData_.mapData[y][x];
			if (type == BlockType::Air) {
				continue;
			}
			// ブロックの生成
			Vector3 pos = GetMapChipPositionByIndex(x, y);
			pos.x += blockOffset_;
			pos.y -= blockOffset_;
			blockArray_[y][x] = Block::CreateBlock(type, pos);
		}
	}
}

void Map::GenerateEnemyLayer()
{
	const uint32_t h = GetHeight();
	const uint32_t w = GetWidth();

	// エネミーデータから敵を生成
	for (uint32_t y = 0; y < h; y++) {
		for(uint32_t x = 0; x < w; x++) {
			const EnemyType type = enemyLayerData_.enemyData[y][x];
			if (type == EnemyType::None) {
				continue;
			}
			// 敵の生成
			Vector3 pos = GetMapChipPositionByIndex(x, y);
			pos.x += blockOffset_;
			pos.y -= blockOffset_;
		}
	}
}


void Map::LoadMapData(const char* filePath)
{
	// CSVファイルからマップデータを読み込む
	CsvLoader csvLoader;
	mapChipData_.mapData = csvLoader.LoadMapBlockType(filePath);

	std::string enemyLayerFilePath = filePath + std::string("_EnemyLayer");
	// 敵レイヤーデータの読み込み
	enemyLayerData_.enemyData = csvLoader.LoadMapEnemyType(enemyLayerFilePath);


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

void Map::BreakBlock(uint32_t xIndex, uint32_t yIndex)
{
	// マップ外チェック
    if (yIndex >= mapChipData_.mapData.size() ||
        xIndex >= mapChipData_.mapData[yIndex].size()) {
        return;
    }
	// 破壊可能ブロックかチェック
    if (mapChipData_.mapData[yIndex][xIndex] != BlockType::breakBlock) {
        return;
    }

    // 該当データの書き換え / Airに変更
    
    isMapDataChanged_ = true;

    // 実体（描画/更新）をその場で消す（Map全再生成しない）
    if (yIndex < blockArray_.size() && xIndex < blockArray_[yIndex].size()) {
        if (blockArray_[yIndex][xIndex]) {
            // 生存フラグ方式（演出を挟むなら Kill のみにしてもOK）
            blockArray_[yIndex][xIndex]->SetBroken();

			Vector3 breakPos = GetMapChipPositionByIndex(xIndex, yIndex);
            // 破壊時にパーティクルの発生
           
			breakParticleEmitter_->SetTranslate(breakPos);

			// 該当のBreakBlockをAirに変更
            mapChipData_.mapData[yIndex][xIndex] = BlockType::Air;
        }
    }
}

