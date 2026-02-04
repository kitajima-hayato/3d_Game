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

static float WrapAngleMinusPiToPi(float a)
{
	const float pi = 3.14159265359f;
	const float twoPi = 6.28318530718f;
	while (a <= -pi) a += twoPi;
	while (a > pi)  a -= twoPi;
	return a;
}

static float LerpAngle(float from, float to, float t)
{
	float delta = WrapAngleMinusPiToPi(to - from); 
	return from + delta * t;
}
static float DampAngle(float current, float target, float turnSpeedRadPerSec, float dt)
{
	// 1-exp(-k*dt) でフレームレート非依存の追従率にする
	float a = 1.0f - std::exp(-turnSpeedRadPerSec * dt);
	return LerpAngle(current, target, a);
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
	const float dt = 1.0f / 60.0f;
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

	// delta から進行方向yaw（Yaw 0が+Z前提）
	if (distance > 1e-6f)
	{
		// 進行方向Yaw
		float moveYaw = std::atan2f(delta.x, delta.z);
		const float modelYawOffset = -1.570796326f; // あなたのモデル補正
		moveYaw = WrapAngle0To2Pi(moveYaw + modelYawOffset);

		// カメラ方向Yaw（位置は position を使う：移動中に徐々に“こっち向き”にしたいので）
		float camYaw = CalcYawFaceCamera(position);

		// 移動中の “カメラ寄せ” 量：序盤は弱く、終盤で強く
		float bias = camBiasMove_ + (camBiasEnd_ - camBiasMove_) * ease; // 0..1

		// 進行方向とカメラ方向をブレンドした“狙うYaw”
		float targetYaw = WrapAngle0To2Pi(LerpAngle(moveYaw, camYaw, bias));

		// 現在YawからターゲットYawへ、回頭速度付きで追従
		float curYaw = playerModel->GetTransform().rotate.y;
		transform.rotate.y = WrapAngle0To2Pi(DampAngle(curYaw, targetYaw, yawTurnSpeed_, dt));
	}


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

		playerModel->SetTransform(endTr);

		prevFramePos_ = moveTargetPos_;
		isMoving_ = false;

		faceStartRx_ = endTr.rotate.x;
		faceStartRz_ = endTr.rotate.z;

		// 向き補間開始
		isFacing_ = true;
		faceTime_ = 0.0f;
		
		// 角度を 0..2pi にしているなら補間前に揃えておくと安定しやすい
		faceStartYaw_ = WrapAngle0To2Pi(endTr.rotate.y);
		faceTargetYaw_ = WrapAngle0To2Pi(CalcYawFaceCamera(endTr.translate));

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

void StageSelectScene::UpdateFaceYaw()
{
	if (!isFacing_) return;

	const float dt = 1.0f / 60.0f;
	faceTime_ += dt;

	float t = faceTime_ / faceDuration_;
	if (t >= 1.0f) t = 1.0f;

	float ease = EaseOutCubic(t);

	Transform tr = playerModel->GetTransform();

	// yaw補間を反映
	float yaw = LerpAngle(faceStartYaw_, faceTargetYaw_, ease);
	tr.rotate.y = WrapAngle0To2Pi(yaw); 



	// 転がり成分は0へ戻す（好み）
	tr.rotate.x = faceStartRx_ * (1.0f - ease);
	tr.rotate.z = faceStartRz_ * (1.0f - ease);

	playerModel->SetTransform(tr);

	if (t >= 1.0f)
	{
		Transform end = playerModel->GetTransform();
		end.rotate.y = faceTargetYaw_;
		end.rotate.x = 0.0f;
		end.rotate.z = 0.0f;
		playerModel->SetTransform(end);
		isFacing_ = false;
	}
}

void StageSelectScene::BuildRoutes()
{
	routeModels_.clear();
	routeTransforms_.clear();

	const uint32_t count = stageSelectGraph->GetNodeCount();
	if (count == 0) return;

	for (uint32_t i = 0; i < count; ++i)
	{
		const StageNode& n = stageSelectGraph->GetNode(i);

		for (int d = 0; d < (int)Direction::count; ++d)
		{
			uint32_t to = n.neighbor[d];
			if (to == StageSelectGraph::INVALID_NODE_ID) continue;

			// 重複生成防止（i->to と to->i を1本にする）
			if (to < i) continue;

			// 端点（ワールド座標）
			Vector3 a = CalcNodeWorldPos(i);
			Vector3 b = CalcNodeWorldPos(to);

			// 少し浮かせる（床に埋まらないように）
			a.y += 0.05f;
			b.y += 0.05f;

			Vector3 diff = b - a;
			diff.y = 0.0f;

			//float len = std::sqrtf(diff.x * diff.x + diff.z * diff.z);
			//if (len < 1e-6f) continue;

			Vector3 mid = (a + b) * 0.5f;

			// yaw（Yaw 0 が +Z 前提）
			float yaw = std::atan2f(diff.x, diff.z);

			// ルートモデルが「+Z方向に長い板」ならこのままでOK
			// もしモデルが +X 方向に伸びてるなら yaw += PI/2 が必要
			// yaw += 1.570796326f;

			// Object3D作成
			auto obj = std::make_unique<Object3D>();
			obj->Initialize();
			obj->SetModel("RoutePlane.obj"); // ←用意したルート用モデル名

			Transform tr{};
			tr.translate = mid;
			tr.translate.y -= 1.0f; // 少し浮かせる
			tr.rotate = { 0.0f, yaw, 0.0f };

			// 重要：長さ方向のスケールを len に合わせる
			// どの軸が「長手」かはモデル次第：
			//  - 板が +Z 方向に長いなら scale.z = len
			//  - 板が +X 方向に長いなら scale.x = len
			tr.scale = { 0.5f, 0.5f, 1.0f };   // ←まずはこれで試す

			obj->SetTransform(tr);

			routeTransforms_.push_back(tr);
			routeModels_.push_back(std::move(obj));
		}
	}
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

	BuildRoutes();
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

	const uint32_t count = stageSelectGraph->GetNodeCount();
	nodeModels_.clear();
	nodeTransforms_.clear();
	nodeModels_.reserve(count);
	nodeTransforms_.reserve(count);

	for (uint32_t i = 0; i < count; ++i)
	{
		auto obj = std::make_unique<Object3D>();
		obj->Initialize();
		obj->SetModel("Node.obj"); // 同じモデル

		Transform tr{};
		tr.scale = { 1.8f, 1.8f, 1.8f };   // 目印なら少し小さく
		tr.rotate = { 0.0f, 0.0f, 0.0f };
		tr.translate = CalcNodeWorldPos(i);

		// ちょい浮かせたいなら
		tr.translate.y += -1.1f; // 例: 0.3f

		// 「常にカメラ向き」にしたいなら
		tr.rotate.y = CalcYawFaceCamera(tr.translate);

		obj->SetTransform(tr);

		nodeTransforms_.push_back(tr);
		nodeModels_.push_back(std::move(obj));
	}
	// UI
	stageSelect_ = std::make_unique<Sprite>();
	stageSelect_->Initialize("resources/StageSelect/StageSelect.png");
	stageSelect_->SetPosition({ 0.0f,0.0f });
	stageSelect_->SetSize({ 1280.0f,720.0f });

	// KeyIconUi　/ 左下に配置
	keyIcon_W = std::make_unique<Sprite>();
	keyIcon_W->Initialize("resources/KyeUI/W.png");
	keyIcon_W->SetPosition({ 55.0f, 615.0f });
	keyIcon_W->SetSize({ 50.0f, 50.0f });
	keyIcon_A = std::make_unique<Sprite>();
	keyIcon_A->Initialize("resources/KyeUI/A.png");
	keyIcon_A->SetPosition({ 10.0f, 635.0f });
	keyIcon_A->SetSize({ 50.0f, 50.0f });
	keyIcon_S = std::make_unique<Sprite>();
	keyIcon_S->Initialize("resources/KyeUI/S.png");
	keyIcon_S->SetPosition({ 55.0f, 660.0f });
	keyIcon_S->SetSize({ 50.0f, 50.0f });
	keyIcon_D = std::make_unique<Sprite>();
	keyIcon_D->Initialize("resources/KyeUI/D.png");
	keyIcon_D->SetPosition({ 100.0f, 635.0f });
	keyIcon_D->SetSize({ 50.0f, 50.0f });

	// esc / enter UI配置
	keyIcon_Esc = std::make_unique<Sprite>();
	keyIcon_Esc->Initialize("resources/KyeUI/Esc.png");
	keyIcon_Esc->SetPosition({ 55.0f, 560.0f });
	keyIcon_Esc->SetSize({ 50.0f, 50.0f });
	keyIcon_Enter = std::make_unique<Sprite>();
	keyIcon_Enter->Initialize("resources/KyeUI/Enter.png");
	keyIcon_Enter->SetPosition({ 55.0f, 505.0f });
	keyIcon_Enter->SetSize({ 50.0f, 50.0f });

	// MoveUI
	moveUI_ = std::make_unique<Sprite>();
	moveUI_->Initialize("resources/StageSelect/Move.png");
	moveUI_->SetPosition(moveUI_Pos_);
	moveUI_->SetSize({ 125.0f,50.0f });
	// CheckUI
	checkUI_ = std::make_unique<Sprite>();
	checkUI_->Initialize("resources/StageSelect/Check.png");
	checkUI_->SetPosition(checkUI_Pos_);
	checkUI_->SetSize({ 125.0f,50.0f });
	// BackUI
	backUI_ = std::make_unique<Sprite>();
	backUI_->Initialize("resources/StageSelect/Back.png");
	backUI_->SetPosition(backUI_Pos_);
	backUI_->SetSize({ 125.0f,50.0f });
	


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

	if (Input::GetInstance()->TriggerKey(DIK_ESCAPE)) {
		// タイトルシーンへ切り替え
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}


	// 空背景の更新
	skyBack->Update();
	// ステージセレクト土台の更新
	stageSelectBase1->Update();
	// ノードモデルの更新
	for (auto& m : nodeModels_) { m->Update(); }

	for (auto& m : routeModels_) { m->Update(); }

	// 入力した方向に移動 
	HandleSelectInput();

	// カーソル移動更新
	UpdateCursorMove();

	// 停止後の向き補間更新
	UpdateFaceYaw();

	if (!isMoving_&&!isFacing_) {
		// ステージセレクト入力処理
		ApplyNodeToCursorTransform();
	}

	// プレイヤー移動処理
	PlayerMove();

	// プレイヤーモデルの更新
	playerModel->Update();
	// ステージセレクトUIの更新
	stageSelect_->Update();
	// KeyUIの更新
	keyIcon_W->Update();
	keyIcon_A->Update();
	keyIcon_S->Update();
	keyIcon_D->Update();
	keyIcon_Esc->Update();
	keyIcon_Enter->Update();
	// Move / Check / Back UIの更新
	moveUI_->Update();
	checkUI_->Update();
	backUI_->Update();

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
	// ノードモデルの描画
	for (auto& m : nodeModels_) { m->Draw(); }
	for (auto& m : routeModels_) { m->Draw(); }
	// ステージセレクトUIの描画
	stageSelect_->Draw();
	// KeyUIの描画
	keyIcon_W->Draw();
	keyIcon_A->Draw();
	keyIcon_S->Draw();
	keyIcon_D->Draw();
	keyIcon_Esc->Draw();
	keyIcon_Enter->Draw();
	// Move / Check / Back UIの描画
	moveUI_->Draw();
	checkUI_->Draw();
	backUI_->Draw();

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

	// UIKeyIcon配置変更
	keyIcon_W_Pos = keyIcon_W->GetPosition();
	keyIcon_A_Pos = keyIcon_A->GetPosition();
	keyIcon_S_Pos = keyIcon_S->GetPosition();
	keyIcon_D_Pos = keyIcon_D->GetPosition();
	keyIcon_Esc_Pos = keyIcon_Esc->GetPosition();
	keyIcon_Enter_Pos = keyIcon_Enter->GetPosition();
	ImGui::DragFloat2("KeyIcon_W_Pos", &keyIcon_W_Pos.x, 1.0f);
	ImGui::DragFloat2("KeyIcon_A_Pos", &keyIcon_A_Pos.x, 1.0f);
	ImGui::DragFloat2("KeyIcon_S_Pos", &keyIcon_S_Pos.x, 1.0f);
	ImGui::DragFloat2("KeyIcon_D_Pos", &keyIcon_D_Pos.x, 1.0f);
	ImGui::DragFloat2("KeyIcon_Esc_Pos", &keyIcon_Esc_Pos.x, 1.0f);
	ImGui::DragFloat2("KeyIcon_Enter_Pos", &keyIcon_Enter_Pos.x, 1.0f);


	keyIcon_W->SetPosition(keyIcon_W_Pos);
	keyIcon_A->SetPosition(keyIcon_A_Pos);
	keyIcon_S->SetPosition(keyIcon_S_Pos);
	keyIcon_D->SetPosition(keyIcon_D_Pos);
	keyIcon_Esc->SetPosition(keyIcon_Esc_Pos);
	keyIcon_Enter->SetPosition(keyIcon_Enter_Pos);

	// Move / Check / Back UI配置変更
	moveUI_Pos_ = moveUI_->GetPosition();
	ImGui::DragFloat2("MoveUI_Pos", &moveUI_Pos_.x, 1.0f);
	moveUI_->SetPosition(moveUI_Pos_);
	
	checkUI_Pos_ = checkUI_->GetPosition();
	ImGui::DragFloat2("CheckUI_Pos", &checkUI_Pos_.x, 1.0f);
	checkUI_->SetPosition(checkUI_Pos_);

	backUI_Pos_ = backUI_->GetPosition();
	ImGui::DragFloat2("BackUI_Pos", &backUI_Pos_.x, 1.0f);
	backUI_->SetPosition(backUI_Pos_);
	

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

	if (isMoving_||isFacing_)return;

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

	Vector3 diff = moveTargetPos_ - moveStartPos_;
	diff.y = 0.0f;
	float dist = std::sqrtf(diff.x * diff.x + diff.z * diff.z);

	// 速度から所要時間を決める
	float dur = dist / moveSpeed_;

	// clamp
	if (dur < moveMinDuration_) dur = moveMinDuration_;
	if (dur > moveMaxDuration_) dur = moveMaxDuration_;

	moveDuration_ = dur;

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
				prevEditNodeId_ = UINT32_MAX; 
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
