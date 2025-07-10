#include "Player.h"
#include "Input.h"
#include "engine/bace/ImGuiManager.h"
#include "SpriteCommon.h"
#include "Game/Particle/EffectManager.h"
#include "Map.h"

AABB Player::GetAABB() const
{
	return {
		transform.translate - transform.scale/2,
		transform.translate + transform.scale/2
	};
}

void Player::OnCollision(Collider* other) 
{
	switch (other->GetType())
	{

	case Collider::Type::Enemy:
	{
		// 敵との衝突処理
		break;
	}
	default:
		break;
	}
}

AABB Player::CalcAABBAtPosition(const Vector3& pos)
{
	return  {
		pos - transform.scale / 2,
		pos + transform.scale / 2
	};
}



void Player::Initialize()
{
	// 初期化処理
	playerModel = std::make_unique<Object3D>();
	playerModel->Initialize();
	playerModel->SetModel("Player.obj");
	transform = {
		// Scale
		{1.0f, 1.0f, 1.0f},
		// Rotate
		{0.0f, 3.0f, 0.0f},
		// Translate
		{1.0f, -7.0f, 20.0f} };
	playerModel->SetTransform(transform);

	/// 一時的
	EffectManager::GetInstance()->CreateEffectGroup("ib", "resources/gradationLine.png");
	qux = std::make_unique<EffectEmitter>();

	emitterTransform = qux->GetTransform();
	emitterTransform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		{ 7.5f,-4.0f,1.5f}
	};
	qux->SetTransform(emitterTransform);
	qux->SetEffectName("ib");

	EffectManager::GetInstance()->CreateEffectGroup("id", "resources/gradationLine.png");

	quux = std::make_unique<EffectEmitter>();
	quuxTransform = quux->GetTransform();
	quuxTransform = {
		{0.5f,0.5f,0.5f },
		{ 0.0f,0.0f,0.0f },
		{ 6.5f,-4.0f,15.0f }
	};
	quux->SetTransform(quuxTransform);


	quux->SetEffectName("id");

	aabb = CalcAABBAtPosition(transform.translate);


}


void Player::Update()
{
	// 更新処理

	playerModel->Update();
	Move();
	Jump();
	playerModel->SetTransform(transform);


#ifdef _DEBUG
	DrawImgui();
#endif // DEBUG

}


void Player::Draw()
{

	// 描画処理
	playerModel->Draw();
	if (dashInputRight == 2 || dashInputLeft == 2) {
		/// @ ダッシュ演出
		qux->EmitRing();
		quux->EmitCylinder();
		EffectManager::GetInstance()->DrawRing();
		//EffectManager::GetInstance()->DrawCylinder();

	}
}


void Player::Move()
{
	/// 右方向への移動入力処理
	MoveRight();
	/// 左方向への移動入力処理
	MoveLeft();

}

void Player::MoveRight()
{
	/// Dが押されたらダッシュ入力を増やす
	if (Input::GetInstance()->TriggerKey(DIK_D)) {
		dashInputRight++;
	}

	// プレイヤーの移動処理
	if (Input::GetInstance()->PushKey(DIK_D)) {
		transform.translate.x += velocity.x;
	}
	/// ダッシュ入力が行われた場合
	if (dashInputRight == 1)
	{
		/// フレームのカウントスタート
		dashExtensionFrameRight++;
		/// カウントフレームが猶予フレームを超えたら受け付けた入力をリセットする
		if (dashExtensionFrameRight > dashInputMaxFrame) {
			dashInputRight = 0;
			dashExtensionFrameRight = 0;
		}
	}
	/// ダッシュ入力が２回行われダッシュ状態になった場合
	if (dashInputRight == 2) {

		/// ダッシュ速度を加算
		transform.translate.x += velocity.x + dashSpeed.x;
		if (Input::GetInstance()->RereseKey(DIK_D)) {
			/// ダッシュ入力が解除されたらダッシュ入力をリセット
			dashInputRight = 0;
			dashExtensionFrameRight = 0;
		}
	}

}

void Player::MoveLeft()
{
	/// Aが押されたらダッシュ入力を増やす
	if (Input::GetInstance()->TriggerKey(DIK_A)) {
		dashInputLeft++;
	}
	// プレイヤーの左への移動処理
	if (Input::GetInstance()->PushKey(DIK_A)) {
		transform.translate.x -= velocity.x;
	}

	/// ダッシュ入力が行われた場合
	if (dashInputLeft == 1)
	{
		/// フレームのカウントスタート
		dashExtensionFrameLeft++;
		/// カウントフレームが猶予フレームを超えたら受け付けた入力をリセットする
		if (dashExtensionFrameLeft > dashInputMaxFrame) {
			dashInputLeft = 0;
			dashExtensionFrameLeft = 0;
		}
	}
	/// ダッシュ入力が２回行われダッシュ状態になった場合
	if (dashInputLeft == 2) {
		transform.translate.x -= velocity.x + dashSpeed.x;
		if (Input::GetInstance()->RereseKey(DIK_A)) {
			/// ダッシュ入力が解除されたらダッシュ入力をリセット
			dashInputLeft = 0;
			dashExtensionFrameLeft = 0;
		}
	}
}


void Player::Jump()
{
	// プレイヤーのジャンプ処理

}

void Player::DrawImgui()
{
	// Imguiの描画処理
	ImGui::Begin("Player Settings");
	ImGui::Text("ExtensionFrameRight : %.f", dashExtensionFrameRight);
	ImGui::Text("DashInputRight : %d", dashInputRight);
	ImGui::Separator();
	ImGui::Text("ExtensionFrameLeft : %.f", dashExtensionFrameLeft);
	ImGui::Text("DashInputLeft : %d", dashInputLeft);

	ImGui::DragFloat3("Translate", &transform.translate.x, 0.1f);
	ImGui::DragFloat3("Scale", &transform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotate", &transform.rotate.x, 0.1f);
	ImGui::End();
}

void Player::CheckBlockCollision(const Map& map)
{
	Vector3 nextPos = transform.translate + velocity * deltaTime;

	AABB nextAABB = CalcAABBAtPosition(nextPos);

	std::vector<Block*> nearbyBlocks = map.GetNearbyBlocks(nextAABB);

	for (const Block* block : nearbyBlocks)
	{
		if (nextAABB.Intersects(block->GetAABB()))
		{
			// 衝突したので反応:停止
			velocity = Vector3(0, 0, 0);
			break;
		}
	}

	transform.rotate += velocity * deltaTime;
}


void Player::Finalize()
{
	// 終了処理
	EffectManager::GetInstance()->DeleteEffectGroup("ib");
	qux.reset();
	EffectManager::GetInstance()->DeleteEffectGroup("id");
	quux.reset();
}