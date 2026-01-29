#include "StageSelectScene.h"
#include "InsideScene/Framework.h"
#include "Game/Camera/Camera.h"
#include "engine/3d/Object3D.h"
#ifdef USE_IMGUI
#include "engine/base/ImGuiManager.h"
#endif

StageSelectScene::StageSelectScene()
{
}

StageSelectScene::~StageSelectScene()
{
}

void StageSelectScene::Initialize(DirectXCommon* dxCommon)
{
    SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	// カメラの取得
	camera = Framework::GetMainCamera();
	cameraTransform.translate = { 0.0f,20.0f,0.0f };
	cameraTransform.rotate = { 0.35f,0.0f,0.0f };
	camera->SetTranslate(cameraTransform.translate);
	camera->SetRotate(cameraTransform.rotate);
	// プレイヤーオブジェクトの初期化
	playerModel = std::make_unique<Object3D>();
	playerModel->Initialize();
	playerModel->SetModel("Player.obj");
	playerTransform = {
		// scale
		{1.5f,1.5f,1.5f},
		// rotate
		{0.0f,0.0f,0.0f},
		// translate
		{-20.0f,1.0f,50.0f},
	};
	playerModel->SetTransform(playerTransform);

	// 背景オブジェクトの初期化
	stageSelectBase1 = std::make_unique<Object3D>();
	stageSelectBase1->Initialize();
	stageSelectBase1->SetModel("SelectBaseGround.obj");
	stageSelectBase1Transform = {
		// Scale
		{ 1.5f, 1.0f, 2.5f },
		// Rotate
		{ 0.0f, 0.0f, 0.0f },
		// Translate
		{ 0.0f, 0.0f, 50.0f }
	};
	stageSelectBase1->SetTransform(stageSelectBase1Transform);

	skyBack = std::make_unique<Object3D>();
	skyBack->Initialize();
	skyBack->SetModel("BackSky.obj");
	skyBackTransform = {
		// Scale
		{ 50.0f, 50.0f, 40.0f },
		// Rotate
		{ 3.2f, 0.0f, 0.0f },
		// Translate
		{ 0.0f, 0.0f, 100.0f }
	};
	skyBack->SetTransform(skyBackTransform);


	stageSelectGraph = std::make_unique<StageSelectGraph>();
	stageSelectGraph->Initialize();
	// ノードの追加
	startNodeId = 0;
	currentNodeId = startNodeId;

	ApplyNodeToCursorTransform();
}

void StageSelectScene::Update()
{

	

	// 空背景の更新
	skyBack->Update();
	// ステージセレクト土台の更新
	stageSelectBase1->Update();

	// ステージセレクト入力処理
	ApplyNodeToCursorTransform();

	// 入力した方向に移動 
	HandleSelectInput();

	// プレイヤー移動処理
	PlayerMove();

	// プレイヤーモデルの更新
	playerModel->Update();

	// ImGuiの描画
	DrawImgui();
}

void StageSelectScene::Draw()
{
	// 空背景の描画
	skyBack->Draw();
	// ステージセレクト土台の描画
	stageSelectBase1->Draw();
	// プレイヤーモデルの描画
	playerModel->Draw();
}

void StageSelectScene::Finalize()
{
}

void StageSelectScene::DrawImgui()
{
#ifdef USE_IMGUI
	ImGui::Begin("StageSelectScene");
	// カメラ位置の調整
	ImGui::DragFloat3("CameraPos", &cameraTransform.translate.x, 0.1f);
	ImGui::DragFloat3("CameraRot", &cameraTransform.rotate.x, 0.01f);
	camera->SetTranslate(cameraTransform.translate);
	camera->SetRotate(cameraTransform.rotate);

	ImGui::Separator();

	// プレイヤー
	Transform transform = playerModel->GetTransform();
	ImGui::DragFloat3("PlayerPos", &transform.translate.x, 0.1f);
	ImGui::DragFloat3("PlayerRot", &transform.rotate.x, 0.1f);
	ImGui::DragFloat3("PlayerScale", &transform.scale.x, 0.1f);
	playerModel->SetTransform(transform);

	ImGui::Separator();
	// ステージセレクト土台１
	stageSelectBase1Transform = stageSelectBase1->GetTransform();
	ImGui::DragFloat3("StageSelectBase1Pos", &stageSelectBase1Transform.translate.x, 0.1f);
	ImGui::DragFloat3("StageSelectBase1Rot", &stageSelectBase1Transform.rotate.x, 0.1f);
	ImGui::DragFloat3("StageSelectBase1Scale", &stageSelectBase1Transform.scale.x, 0.1f);
	stageSelectBase1->SetTransform(stageSelectBase1Transform);

	ImGui::Separator();

	// 空背景
	skyBackTransform = skyBack->GetTransform();
	ImGui::DragFloat3("SkyBackPos", &skyBackTransform.translate.x, 0.1f);
	ImGui::DragFloat3("SkyBackRot", &skyBackTransform.rotate.x, 0.1f);
	ImGui::DragFloat3("SkyBackScale", &skyBackTransform.scale.x, 0.1f);
	skyBack->SetTransform(skyBackTransform);

	ImGui::End();
	// ステージセレクトグラフのデバッグ表示
	DrawSelectGraphImGui();
#endif
}

void StageSelectScene::PlayerMove()
{
	// プレイヤーの座標取得
	Transform transform = playerModel->GetTransform();

	if (Input::GetInstance()->PushKey(DIK_W)) {
		transform.translate.z += 0.5f;
	}

	playerModel->SetTransform(transform);
}

void StageSelectScene::HandleSelectInput()
{
	if (Input::GetInstance()->TriggerKey(DIK_W)) {
		currentNodeId = stageSelectGraph->Move(currentNodeId, Direction::Up);
	} else if (Input::GetInstance()->TriggerKey(DIK_S)) {
		currentNodeId = stageSelectGraph->Move(currentNodeId, Direction::Down);
	} else if (Input::GetInstance()->TriggerKey(DIK_A)) {
		currentNodeId = stageSelectGraph->Move(currentNodeId, Direction::Left);
	} else if (Input::GetInstance()->TriggerKey(DIK_D)) {
		currentNodeId = stageSelectGraph->Move(currentNodeId, Direction::Right);
	}
}

void StageSelectScene::ApplyNodeToCursorTransform()
{
	const StageNode& node = stageSelectGraph->GetNode(currentNodeId);
	MapPos pos = node.position;
	Vector3 translate = playerModel->GetTransform().translate;
	translate.x += static_cast<float>(pos.x);
	translate.y;
	translate.z += static_cast<float>(pos.y);
	playerModel->SetTranslate(translate);

	
}

void StageSelectScene::DrawSelectGraphImGui()
{
#ifdef USE_IMGUI
    // Graph が無い場合は何もしない
    if (!stageSelectGraph)
    {
        ImGui::Begin("StageSelect Graph");
        ImGui::TextDisabled("stageSelectGraph is null.");
        ImGui::End();
        return;
    }

    const uint32_t count = stageSelectGraph->GetNodeCount();

    // 選択IDの補正
    if (count == 0)
    {
        editNodeId_ = 0;
        prevEditNodeId_ = UINT32_MAX; // 次にノードができたらロードされるように
    } else if (editNodeId_ >= count)
    {
        editNodeId_ = count - 1;
    }

    // 選択が変わったときだけ編集バッファへロード（★途中編集できない問題の解消点）
    auto toUi = [&](uint32_t id)->int {
        return (id == StageSelectGraph::INVALID_NODE_ID) ? -1 : (int)id;
        };

    if (count > 0 && editNodeId_ != prevEditNodeId_)
    {
        prevEditNodeId_ = editNodeId_;
        const StageNode& n = stageSelectGraph->GetNode(editNodeId_);

        editX_ = (int)n.position.x;
        editY_ = (int)n.position.y;
        editStageId_ = (int)n.stageId;
        editUnlocked_ = n.unlocked;

        editNeighUp_ = toUi(n.neighbor[(int)Direction::Up]);
        editNeighDown_ = toUi(n.neighbor[(int)Direction::Down]);
        editNeighLeft_ = toUi(n.neighbor[(int)Direction::Left]);
        editNeighRight_ = toUi(n.neighbor[(int)Direction::Right]);

        jsonDirty_ = true;
    }

    auto toId = [&](int v)->uint32_t {
        return (v < 0) ? StageSelectGraph::INVALID_NODE_ID : (uint32_t)v;
        };

    auto validateNeighbor = [&](int v)->int {
        if (v < 0) return 0;                 // none
        if ((uint32_t)v >= count) return 2;  // invalid
        return 1;                            // ok
        };

    // ========= UI =========
    ImGui::Begin("StageSelect Graph", nullptr, ImGuiWindowFlags_NoCollapse);

    ImGui::Text("Nodes: %u", count);
    ImGui::SameLine();
    ImGui::TextDisabled("Selected: %u", (count > 0) ? editNodeId_ : 0);
    ImGui::Separator();

    // ---- Tabs ----
    if (ImGui::BeginTabBar("GraphTabs"))
    {
        // ======================
        // List tab
        // ======================
        if (ImGui::BeginTabItem("List"))
        {
            ImGui::Checkbox("Unlocked only", &filterUnlockedOnly_);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(140.0f);
            ImGui::InputInt("Filter StageId", &filterStageId_);
            ImGui::SameLine();
            if (ImGui::Button("Clear"))
            {
                filterUnlockedOnly_ = false;
                filterStageId_ = -1;
            }

            ImGui::Separator();

            ImGui::BeginChild("NodeList", ImVec2(0, 0), true);

            if (ImGui::BeginTable("nodes_table", 6,
                ImGuiTableFlags_RowBg | ImGuiTableFlags_Borders | ImGuiTableFlags_ScrollY |
                ImGuiTableFlags_Resizable | ImGuiTableFlags_SizingStretchProp))
            {
                ImGui::TableSetupColumn("Index", ImGuiTableColumnFlags_WidthFixed, 60);
                ImGui::TableSetupColumn("StageId", ImGuiTableColumnFlags_WidthFixed, 70);
                ImGui::TableSetupColumn("Unlocked", ImGuiTableColumnFlags_WidthFixed, 80);
                ImGui::TableSetupColumn("X", ImGuiTableColumnFlags_WidthFixed, 60);
                ImGui::TableSetupColumn("Y", ImGuiTableColumnFlags_WidthFixed, 60);
                ImGui::TableSetupColumn("Neigh", ImGuiTableColumnFlags_WidthFixed, 70);
                ImGui::TableHeadersRow();

                for (uint32_t i = 0; i < count; ++i)
                {
                    const StageNode& n = stageSelectGraph->GetNode(i);

                    if (filterUnlockedOnly_ && !n.unlocked) continue;
                    if (filterStageId_ >= 0 && (int)n.stageId != filterStageId_) continue;

                    int neighCount = 0;
                    for (int d = 0; d < (int)Direction::count; ++d)
                        if (n.neighbor[d] != StageSelectGraph::INVALID_NODE_ID) neighCount++;

                    ImGui::TableNextRow();

                    ImGui::TableNextColumn();
                    bool selected = (editNodeId_ == i);

                    ImGui::PushID((int)i);
                    if (!n.unlocked) ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(160, 160, 160, 255));
                    if (ImGui::Selectable("##row", selected, ImGuiSelectableFlags_SpanAllColumns))
                    {
                        editNodeId_ = i;     // 次フレームでバッファがロードされる
                    }
                    if (!n.unlocked) ImGui::PopStyleColor();
                    ImGui::PopID();

                    ImGui::SameLine();
                    ImGui::Text("%u", i);

                    ImGui::TableNextColumn(); ImGui::Text("%u", n.stageId);
                    ImGui::TableNextColumn(); ImGui::TextUnformatted(n.unlocked ? "Yes" : "No");
                    ImGui::TableNextColumn(); ImGui::Text("%u", n.position.x);
                    ImGui::TableNextColumn(); ImGui::Text("%u", n.position.y);
                    ImGui::TableNextColumn(); ImGui::Text("%d", neighCount);
                }

                ImGui::EndTable();
            }

            ImGui::EndChild();
            ImGui::EndTabItem();
        }

        // ======================
        // Node tab
        // ======================
        if (ImGui::BeginTabItem("Node"))
        {
            if (count == 0)
            {
                ImGui::TextDisabled("No nodes.");
            } else
            {
                ImGui::Text("Editing node: %u", editNodeId_);
                ImGui::Separator();

                ImGui::SetNextItemWidth(200.0f);
                ImGui::InputInt("StageId", &editStageId_);
                ImGui::Checkbox("Unlocked", &editUnlocked_);

                ImGui::Separator();

                ImGui::SetNextItemWidth(120.0f);
                ImGui::InputInt("PosX", &editX_);
                ImGui::SameLine();
                ImGui::SetNextItemWidth(120.0f);
                ImGui::InputInt("PosY", &editY_);

                if (ImGui::Button("Apply Node"))
                {
                    stageSelectGraph->SetNodeStageId(editNodeId_, (uint32_t)(std::max)(editStageId_, 0));
                    stageSelectGraph->SetNodeUnlocked(editNodeId_, editUnlocked_);
                    stageSelectGraph->SetNodePos(editNodeId_, {
                        (uint32_t)(std::max)(editX_, 0),
                        (uint32_t)(std::max)(editY_, 0)
                        });

                    jsonDirty_ = true;
                }

                ImGui::SameLine();
                if (ImGui::Button("Reload From Graph"))
                {
                    prevEditNodeId_ = UINT32_MAX; // 次フレームで再ロード
                }
            }

            ImGui::EndTabItem();
        }

        // ======================
        // Neighbors tab
        // ======================
        if (ImGui::BeginTabItem("Neighbors"))
        {
            if (count == 0)
            {
                ImGui::TextDisabled("No nodes.");
            } else
            {
                ImGui::Text("Editing node: %u", editNodeId_);
                ImGui::TextDisabled("Use -1 for none. Valid range: 0..%u", (count > 0) ? (count - 1) : 0);
                ImGui::Separator();

                ImGui::SetNextItemWidth(140.0f); ImGui::InputInt("Up", &editNeighUp_);
                {
                    int s = validateNeighbor(editNeighUp_);
                    ImGui::SameLine();
                    if (s == 0) ImGui::TextDisabled("none");
                    else if (s == 2) ImGui::TextColored(ImVec4(1, 0.2f, 0.2f, 1), "invalid");
                    else ImGui::Text("ok");
                }

                ImGui::SetNextItemWidth(140.0f); ImGui::InputInt("Down", &editNeighDown_);
                {
                    int s = validateNeighbor(editNeighDown_);
                    ImGui::SameLine();
                    if (s == 0) ImGui::TextDisabled("none");
                    else if (s == 2) ImGui::TextColored(ImVec4(1, 0.2f, 0.2f, 1), "invalid");
                    else ImGui::Text("ok");
                }

                ImGui::SetNextItemWidth(140.0f); ImGui::InputInt("Left", &editNeighLeft_);
                {
                    int s = validateNeighbor(editNeighLeft_);
                    ImGui::SameLine();
                    if (s == 0) ImGui::TextDisabled("none");
                    else if (s == 2) ImGui::TextColored(ImVec4(1, 0.2f, 0.2f, 1), "invalid");
                    else ImGui::Text("ok");
                }

                ImGui::SetNextItemWidth(140.0f); ImGui::InputInt("Right", &editNeighRight_);
                {
                    int s = validateNeighbor(editNeighRight_);
                    ImGui::SameLine();
                    if (s == 0) ImGui::TextDisabled("none");
                    else if (s == 2) ImGui::TextColored(ImVec4(1, 0.2f, 0.2f, 1), "invalid");
                    else ImGui::Text("ok");
                }

                if (ImGui::Button("Apply Neighbors"))
                {
                    stageSelectGraph->SetNeighbor(editNodeId_, Direction::Up, toId(editNeighUp_));
                    stageSelectGraph->SetNeighbor(editNodeId_, Direction::Down, toId(editNeighDown_));
                    stageSelectGraph->SetNeighbor(editNodeId_, Direction::Left, toId(editNeighLeft_));
                    stageSelectGraph->SetNeighbor(editNodeId_, Direction::Right, toId(editNeighRight_));
                    jsonDirty_ = true;
                }

                ImGui::SameLine();
                if (ImGui::Button("Jump To Up") && editNeighUp_ >= 0 && (uint32_t)editNeighUp_ < count)    editNodeId_ = (uint32_t)editNeighUp_;
                ImGui::SameLine();
                if (ImGui::Button("Jump To Down") && editNeighDown_ >= 0 && (uint32_t)editNeighDown_ < count) editNodeId_ = (uint32_t)editNeighDown_;

                if (ImGui::Button("Jump To Left") && editNeighLeft_ >= 0 && (uint32_t)editNeighLeft_ < count) editNodeId_ = (uint32_t)editNeighLeft_;
                ImGui::SameLine();
                if (ImGui::Button("Jump To Right") && editNeighRight_ >= 0 && (uint32_t)editNeighRight_ < count) editNodeId_ = (uint32_t)editNeighRight_;
            }

            ImGui::EndTabItem();
        }

        // ======================
        // JSON tab
        // ======================
        if (ImGui::BeginTabItem("JSON"))
        {
            ImGui::TextDisabled("Graph JSON preview (read-only).");

            if (ImGui::Button("Refresh"))
            {
                jsonDirty_ = true;
            }
            ImGui::SameLine();

            static char saveName[128] = "StageSelectNodes";
            ImGui::SetNextItemWidth(220.0f);
            ImGui::InputText("Save BaseName", saveName, sizeof(saveName));
            ImGui::SameLine();
            if (ImGui::Button("Save JSON"))
            {
                stageSelectGraph->SaveToJsonFile(saveName);
            }

            ImGui::Separator();

            if (jsonDirty_)
            {
                std::string s = stageSelectGraph->ToJsonString(2);
                strncpy_s(jsonBuf_, sizeof(jsonBuf_), s.c_str(), _TRUNCATE);
                jsonDirty_ = false;
            }

            ImGui::InputTextMultiline("##json",
                jsonBuf_, sizeof(jsonBuf_),
                ImVec2(-1.0f, -1.0f),
                ImGuiInputTextFlags_ReadOnly);

            ImGui::EndTabItem();
        }

        // ======================
        // Ops tab
        // ======================
        if (ImGui::BeginTabItem("Ops"))
        {
            ImGui::Text("Add Node");
            ImGui::SetNextItemWidth(90.0f);  ImGui::InputInt("New X", &newX_);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(90.0f);  ImGui::InputInt("New Y", &newY_);
            ImGui::SameLine();
            ImGui::SetNextItemWidth(120.0f); ImGui::InputInt("New StageId", &newStageId_);
            ImGui::SameLine();
            ImGui::Checkbox("New Unlocked", &newUnlocked_);

            if (ImGui::Button("Add"))
            {
                uint32_t newId = stageSelectGraph->AddNode(
                    { (uint32_t)(std::max)(newX_, 0), (uint32_t)(std::max)(newY_, 0) },
                    (uint32_t)(std::max)(newStageId_, 0),
                    newUnlocked_
                );
                editNodeId_ = newId;
                prevEditNodeId_ = UINT32_MAX; // 次フレームでロード
                jsonDirty_ = true;
            }

            ImGui::Separator();
            ImGui::TextDisabled("Tips: Edit values here, then press Apply in each tab.");

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }

    ImGui::End();
#endif
}
