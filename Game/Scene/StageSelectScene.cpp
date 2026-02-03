#include "StageSelectScene.h"
#include "engine/InsideScene/Framework.h"
#include "Game/Camera/Camera.h"
#include "engine/3d/Object3D.h"
#include "Game/Application/PlayContext.h"
#ifdef USE_IMGUI
#include "engine/base/ImGuiManager.h"
#endif

static float EaseOutCubic(float t)
{
	float inv = 1.0f - t;
	return 1.0f - inv * inv * inv;
}

static float WrapAngle0To2Pi(float angle)
{
	const float twoPi = 6.28318530718f;
	while (angle < 0.0f) angle += twoPi;
	while (angle >= twoPi) angle -= twoPi;
	return angle;
}

Vector3 StageSelectScene::CalcNodeWorldPos(uint32_t nodeId)const
{
	const Vector2 uv = stageSelectGraph->GetNodeUV(nodeId);

	const float mapWidth = 40.0f;
	const float mapHeight = 20.0f;

	Vector3 origin = stageSelectBase1Transform.translate;
	origin.y = 1.0f;

	Vector3 t = origin;
	t.x += (uv.x - 0.5f) * mapWidth;

	// 上下反転済みの式（あなたが直したやつ）
	t.z += (0.5f - uv.y) * mapHeight;

	return t;
}

void StageSelectScene::UpdateCursorMove()
{
	// 移動中でなければ終了
	if (!isMoving_)return;
	// 経過時間を進める
	const float dt = 1.0f / 120.0f;
	moveTime_ += dt;

	float t = moveTime_ / moveDuration_;
	if (t >= 1.0f) t = 1.0f;

	float ease = EaseOutCubic(t);

	// 位置の保管
	Vector3 position;
	position = moveStartPos_ + (moveTargetPos_ - moveStartPos_) * ease;

	// プレイヤー回転
	Vector3 delta = position - prevFramePos_;
	delta.y = 0.0f;

	// 移動距離に応じた回転量
	const float distance = std::sqrtf(delta.x * delta.x + delta.z * delta.z);
	
	// プレイヤートランスフォーム取得
	Transform transform = playerModel->GetTransform();
	// 位置更新
	transform.translate = position;


	if (distance > 1e-6f && cursorRadius_ > 1e-6f)
	{
		const float angle = distance / cursorRadius_;

		const float invDist = 1.0f / distance;
		const float nx = delta.x * invDist;
		const float nz = delta.z * invDist;

		// 回転軸 = ( +nz, 0, -nx ) 相当を Eulerで近似
		transform.rotate.x += angle * nz;
		transform.rotate.z += angle * -nx;
	}
	// プレイヤー位置更新
	playerModel->SetTransform(transform);

	prevFramePos_ = position;

	if (t >= 1.0f)
	{
		// 位置を完全に固定（誤差除去）
		Transform endTr = playerModel->GetTransform();
		endTr.translate = moveTargetPos_;
		endTr.rotate.y = CalcYawFaceCamera(moveTargetPos_);

		// 「止まったら正面向いて綺麗に」したいなら転がり成分を消す
		endTr.rotate.x = 0.0f;
		endTr.rotate.z = 0.0f;

		playerModel->SetTransform(endTr);

		prevFramePos_ = moveTargetPos_;
		isMoving_ = false;
	}
}

float StageSelectScene::CalcYawFaceCamera(const Vector3& pos) const
{
	// カメラ位置（あなたは cameraTransform を持ってるのでそれを使う）
	Vector3 camPos = cameraTransform.translate;

	Vector3 d = camPos - pos;
	d.y = 0.0f;

	const float len = std::sqrtf(d.x * d.x + d.z * d.z);
	if (len < 1e-6f) return 0.0f;

	d.x /= len;
	d.z /= len;

	// yaw 0 が「+Z向き」の系なら atan2(x,z) が定番
	float yaw = std::atan2f(d.x, d.z);

	// もしモデルの正面が「右(+X)」基準なら、オフセットが必要
	// 右向きが正面 → yawに +90deg(=π/2) などを足して調整
	const float modelYawOffset = -1.570796326f; // 必要なら 1.570796f を試す
	yaw += modelYawOffset;

	return WrapAngle0To2Pi(yaw);
}



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
		{2.5f,2.5f,2.5f},
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

	Vector3 pos = CalcNodeWorldPos(currentNodeId);
	Transform transform = playerModel->GetTransform();
	transform.translate = pos;
	playerModel->SetTransform(transform);

	isMoving_ = false;
	moveTime_ = 0.0f;
	moveStartPos_ = pos;
	moveTargetPos_ = pos;
	prevFramePos_ = pos;

	ApplyNodeToCursorTransform();
}

void StageSelectScene::Update()
{
	if (Input::GetInstance()->TriggerKey(DIK_RETURN)) {
		const StageNode& node = stageSelectGraph->GetNode(currentNodeId);
		PlayContext::GetInstance().SetSelectedStage(node.stageId, node.stageKey);

		// ステージシーンへ切り替え
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}


	// 空背景の更新
	skyBack->Update();
	// ステージセレクト土台の更新
	stageSelectBase1->Update();

	// 入力した方向に移動 
	HandleSelectInput();

	// カーソル移動更新
	UpdateCursorMove();

	if (!isMoving_) {
		// ステージセレクト入力処理
		ApplyNodeToCursorTransform();
	}

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


	playerModel->SetTransform(transform);
}

void StageSelectScene::HandleSelectInput()
{

	if (isMoving_)return;

	// 現在のノードIDを保存
	uint32_t next = currentNodeId;

	// 入力した方向に移動できるノードがあれば移動
	if (Input::GetInstance()->TriggerKey(DIK_W)) {
		next = stageSelectGraph->Move(currentNodeId, Direction::Up);
	} else if (Input::GetInstance()->TriggerKey(DIK_S)) {
		next = stageSelectGraph->Move(currentNodeId, Direction::Down);
	} else if (Input::GetInstance()->TriggerKey(DIK_A)) {
		next = stageSelectGraph->Move(currentNodeId, Direction::Left);
	} else if (Input::GetInstance()->TriggerKey(DIK_D)) {
		next = stageSelectGraph->Move(currentNodeId, Direction::Right);
	}
	// ノードが変わらなければ何もしない
	if (next == currentNodeId)return;

	currentNodeId = next;

	// 移動開始
	isMoving_ = true;
	moveTime_ = 0.0f;

	// ノード更新
	Transform transform = playerModel->GetTransform();
	moveStartPos_ = transform.translate;
	moveTargetPos_ = CalcNodeWorldPos(next);
	prevFramePos_ = moveStartPos_;
}

void StageSelectScene::ApplyNodeToCursorTransform()
{
	//const StageNode& node = stageSelectGraph->GetNode(currentNodeId);

	//// 正規化座標に変換（Graph側で bounds を見て計算）
	//const Vector2 uv = stageSelectGraph->GetNodeUV(currentNodeId);


	//// マップの表示範囲（ワールド単位）
	//const float mapWidth = 30.0f;  // 横幅
	//const float mapHeight = 20.0f;  // 縦幅

	//// マップの基準（中心）位置
	//Vector3 origin = stageSelectBase1Transform.translate;
	//origin.y = 1.0f; // カーソル高さ

	//// ノード位置をワールド座標に変換
	//Vector3 t = origin;
	//t.x += (uv.x - 0.5f) * mapWidth;
	//t.z += (0.5f - uv.y) * mapHeight;

	// 絶対座標でセット
	Transform tr = playerModel->GetTransform();
	tr.translate = CalcNodeWorldPos(currentNodeId);

	tr.rotate.y = CalcYawFaceCamera(tr.translate);

	tr.rotate.x = 0.0f;
	tr.rotate.z = 0.0f;
	playerModel->SetTransform(tr);
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
		prevEditNodeId_ = UINT32_MAX;
	} else if (editNodeId_ >= count)
	{
		editNodeId_ = count - 1;
	}

	// 選択が変わったときだけ編集バッファへロード
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
		strncpy_s(editStageKey_, sizeof(editStageKey_), n.stageKey.c_str(), _TRUNCATE);
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
						editNodeId_ = i;
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
				ImGui::SetNextItemWidth(200.0f);
				ImGui::InputText("StageKey", editStageKey_, sizeof(editStageKey_));
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
					stageSelectGraph->SetNodeStageKey(editNodeId_, std::string(editStageKey_));

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
			ImGui::SetNextItemWidth(120.0f); ImGui::InputText("New StageKey", newStageKey_, sizeof(newStageKey_));
			ImGui::SameLine();
			ImGui::Checkbox("New Unlocked", &newUnlocked_);

			if (ImGui::Button("Add"))
			{
				uint32_t newId = stageSelectGraph->AddNode(
					{ (uint32_t)(std::max)(newX_, 0), (uint32_t)(std::max)(newY_, 0) },
					(uint32_t)(std::max)(newStageId_, 0),
					std::string(newStageKey_),
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



		if (ImGui::BeginTabItem("Cheack")) {

			ImGui::Text("CurrentNodeId: %u", currentNodeId);

			const StageNode& cn = stageSelectGraph->GetNode(currentNodeId);
			ImGui::Text("Pos: (%u, %u)", cn.position.x, cn.position.y);

			auto showTo = [&](Direction dir, const char* name) {
				uint32_t to = cn.neighbor[(int)dir];
				ImGui::Text("%s -> %u", name, to);
				};
			showTo(Direction::Up, "Up");
			showTo(Direction::Down, "Down");
			showTo(Direction::Left, "Left");
			showTo(Direction::Right, "Right");

			ImGui::EndTabItem();
		}

		ImGui::EndTabBar();
	}

	ImGui::End();
#endif
}
