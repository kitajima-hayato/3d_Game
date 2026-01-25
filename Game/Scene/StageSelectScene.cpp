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
	ImGui::Begin("StageSelect Graph Editor");

	const uint32_t count = stageSelectGraph->GetNodeCount();
	ImGui::Text("Node Count: %u", count);

	// --- ノード一覧（選択） ---
	if (ImGui::BeginListBox("Nodes"))
	{
		for (uint32_t i = 0; i < count; ++i)
		{
			const StageNode& n = stageSelectGraph->GetNode(i);
			char label[64];
			sprintf_s(label, "ID %u  Pos(%d,%d)  Stage:%u  %s",
				i, n.position.x, n.position.y, n.id, n.unlocked ? "Unlocked" : "Locked");

			const bool selected = (editNodeId_ == i);
			if (ImGui::Selectable(label, selected))
			{
				editNodeId_ = i;
			}
		}
		ImGui::EndListBox();
	}

	ImGui::Separator();

	// --- 選択ノード編集 ---
	if (count > 0 && editNodeId_ < count)
	{
		StageNode n = stageSelectGraph->GetNode(editNodeId_); // コピー

		ImGui::Text("Editing Node ID: %u", editNodeId_);

		int x = n.position.x;
		int y = n.position.y;
		int stageId = (int)n.id;
		bool unlocked = n.unlocked;

		ImGui::InputInt("X", &x);
		ImGui::InputInt("Y", &y);
		ImGui::InputInt("StageId", &stageId);
		ImGui::Checkbox("Unlocked", &unlocked);

		
		if (ImGui::Button("Apply Node Changes"))
		{
			stageSelectGraph->SetNodePos(editNodeId_, { static_cast<uint32_t>(x), static_cast<uint32_t>(y) });
			stageSelectGraph->SetNodeStageId(editNodeId_, (uint32_t)stageId);
			stageSelectGraph->SetNodeUnlocked(editNodeId_, unlocked);
		}

		ImGui::Separator();

		// --- 接続編集（Up/Down/Left/Right） ---
		// UI用に "None = -1" を使う
		auto toUi = [&](uint32_t id)->int {
			return (id == StageSelectGraph::INVALID_NODE_ID) ? -1 : (int)id;
			};
		auto toId = [&](int v)->uint32_t {
			return (v < 0) ? StageSelectGraph::INVALID_NODE_ID : (uint32_t)v;
			};

		neighborUp_ = toUi(n.neighbor[(int)Direction::Up]);
		neighborDown_ = toUi(n.neighbor[(int)Direction::Down]);
		neighborLeft_ = toUi(n.neighbor[(int)Direction::Left]);
		neighborRight_ = toUi(n.neighbor[(int)Direction::Right]);

		ImGui::InputInt("Neighbor Up (ID or -1)", &neighborUp_);
		ImGui::InputInt("Neighbor Down (ID or -1)", &neighborDown_);
		ImGui::InputInt("Neighbor Left (ID or -1)", &neighborLeft_);
		ImGui::InputInt("Neighbor Right (ID or -1)", &neighborRight_);

		if (ImGui::Button("Apply Neighbors"))
		{
			stageSelectGraph->SetNeighbor(editNodeId_, Direction::Up, toId(neighborUp_));
			stageSelectGraph->SetNeighbor(editNodeId_, Direction::Down, toId(neighborDown_));
			stageSelectGraph->SetNeighbor(editNodeId_, Direction::Left, toId(neighborLeft_));
			stageSelectGraph->SetNeighbor(editNodeId_, Direction::Right, toId(neighborRight_));
		}
	}

	ImGui::Separator();

	// --- ノード追加 ---
	ImGui::Text("Add New Node");
	ImGui::InputInt("New X", &newX_);
	ImGui::InputInt("New Y", &newY_);
	ImGui::InputInt("New StageId", &newStageId_);
	ImGui::Checkbox("New Unlocked", &newUnlocked_);

	if (ImGui::Button("Add Node"))
	{
		uint32_t newId = stageSelectGraph->AddNode(
			{ static_cast<uint32_t>((std::max)(newX_, 0)), static_cast<uint32_t>((std::max)(newY_, 0)) },
			static_cast<uint32_t>((std::max)(newStageId_, 0)),
			newUnlocked_
		);
		editNodeId_ = newId; 
	}

	ImGui::End();
#endif
}

