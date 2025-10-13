#define NOMINMAX
#include "Player.h"
#include "Input.h"
#include "engine/bace/ImGuiManager.h"
#include "SpriteCommon.h"
#include "Game/Particle/EffectManager.h"
#include "Map.h"
#include "engine/bace/Logger.h"
#include <algorithm>

AABB Player::GetAABB() const
{
	return {
		transform.translate - transform.scale / 2,
		transform.translate + transform.scale / 2
	};
}

void Player::OnCollision(Collider* other)
{
	if (other->GetType() == Collider::Type::Enemy) {
		hitThisFrame = true;
		isAlive = false;  // プレイヤーが敵に当たったら死亡
		// 必要ならダメージ処理など
	}
}

AABB Player::CalcAABBAtPosition(const Vector3& pos)
{
	return  {
		pos - transform.scale * 0.5f,
		pos + transform.scale * 0.5f
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
	/*EffectManager::GetInstance()->CreateEffectGroup("ib", "resources/gradationLine.png");
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


	quux->SetEffectName("id");*/

	aabb = CalcAABBAtPosition(transform.translate);

	//mapChipField 
}


void Player::Update()
{

	// 更新処理
	if (!isAlive) {
		// プレイヤーが死亡している場合は何もしない
		return;
	}
	// 毎フレーム冒頭で呼ぶ
	prevTranslate = transform.translate;
	
	Move();
	Jump();
	CheckBlockCollision();
	playerModel->SetTransform(transform);
	
	playerModel->Update();

#ifdef _DEBUG
	DrawImgui();
#endif // DEBUG

	velocity = { 0.0f, 0.0f, 0.0f };

}


void Player::Draw()
{
	if (!isAlive) {
		/// プレイヤーが死亡している場合は何もしない
		return;
	}

	// 描画処理
	/// スペースキーを押したら
	if (!Input::GetInstance()->PushKey(DIK_SPACE)) {
		playerModel->Draw();
	}

	if (dashInputRight == 2 || dashInputLeft == 2) {
		/// @ ダッシュ演出
		/*qux->EmitRing();
		quux->EmitCylinder();
		EffectManager::GetInstance()->DrawRing();*/
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

	/// プレイヤーの移動処理
	if (Input::GetInstance()->PushKey(DIK_D)) {
		velocity = { 0.0f, 0.0f, 0.0f };
		velocity.x = playerParameter.kAcceleration;
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
	/// プレイヤーの左への移動処理
	if (Input::GetInstance()->PushKey(DIK_A)) {
		velocity = { 0.0f,0.0f, 0.0f };
		velocity.x = -playerParameter.kAcceleration;
		transform.translate.x += velocity.x;
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
		transform.translate.x += velocity.x - dashSpeed.x;
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
#ifdef _DEBUG
	// Imguiの描画処理

	ImGui::Begin("PlayerParamater");
	ImGui::Text("Acceleration : %.2f", playerParameter.kAcceleration);
	ImGui::Text("velocity : %.2f", velocity.x);
	ImGui::Separator();
	ImGui::End();


	ImGui::Begin("Player Settings");
	ImGui::Text("ExtensionFrameRight : %.f", dashExtensionFrameRight);
	ImGui::Text("DashInputRight : %d", dashInputRight);
	ImGui::Separator();
	ImGui::Text("ExtensionFrameLeft : %.f", dashExtensionFrameLeft);
	ImGui::Text("DashInputLeft : %d", dashInputLeft);

	ImGui::DragFloat3("Translate", &transform.translate.x, 0.1f);
	ImGui::DragFloat3("Scale", &transform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotate", &transform.rotate.x, 0.1f);
	ImGui::Separator();
	ImGui::Text("Hit: %s", hitThisFrame ? "True" : "False");
	ImGui::End();
#endif // DEBUG
}

void Player::CheckBlockCollision()
{
	
	/// 判定の初期化
	CollisionMapInfo collisionMapInfo;
	/// 移動量に速度をコピー
	Vector3 desiredMove = transform.translate - prevTranslate;
	/// 移動量をセット
	transform.translate = prevTranslate;
	/// 
	collisionMapInfo.move = desiredMove;
	/// マップの衝突確認
	MapCollision(collisionMapInfo);
	/// 天井との衝突処理
	CeilingCollisionMove(collisionMapInfo);
	/// 床との衝突処理
	LandingCollisionMove(collisionMapInfo);
	/// 壁との衝突処理
	WallCollisionMove(collisionMapInfo);
	/// 移動量を加算
	transform.translate += collisionMapInfo.move;
	playerModel->SetTransform(transform);


}

void Player::MapCollision(CollisionMapInfo& mapInfo)
{
	// マップの衝突判定統括
	CollisionMapInfoDirection(
		mapInfo,
		CollisionType::Top,
		{ kLeftTop,kRightTop },
		Vector3(0.0f, 0.0f, 0.0f),
		[](const CollisionMapInfo& info) {return info.move.y > 0.0f; }
	);
	CollisionMapInfoDirection(
		mapInfo,
		CollisionType::Bottom,
		{ kLeftBottom,kRightBottom },
		Vector3(0.0f, 0.0f, 0.0f),
		[](const CollisionMapInfo& info) {return info.move.y < 0.0f; }
	);
	CollisionMapInfoDirection(
		mapInfo,
		CollisionType::Left,
		{ kLeftTop,kLeftBottom },
		Vector3(-playerParameter.kEpsilon, 0.0f, 0.0f),
		[](const CollisionMapInfo& info) {return info.move.x < 0.0f; }
	);
	CollisionMapInfoDirection(
		mapInfo,
		CollisionType::Right,
		{ kRightTop,kRightBottom },
		Vector3(playerParameter.kEpsilon, 0.0f, 0.0f),
		[](const CollisionMapInfo& info) {return info.move.x > 0.0f; }
	);

}

void Player::CollisionMapInfoDirection(CollisionMapInfo& mapInfo, CollisionType dir, const std::array<Corner, 2>& CheckCorners, const Vector3& offset, std::function<bool(const CollisionMapInfo&)> moveCondition)
{
	// 衝突判定の方向別処理

	/// 移動量が0なら処理しない
	if (!moveCondition(mapInfo)) {
		return;
	}
	// 移動後の座標を計算
	Vector3 position = playerModel->GetTranslate() + mapInfo.move;

	// 2つのコーナーを計算
	std::array<Vector3, 2> corners = {
		CornerPosition(position, CheckCorners[0]) + offset,
		CornerPosition(position, CheckCorners[1]) + offset
	};

	// 衝突判定を行う
	if (CheackCollisionPoints(corners, static_cast<CollisionType>(dir), mapInfo)) {
#ifdef _DEBUG
		switch (dir)
		{
		case CollisionType::Top:ImGui::Text("Top Hit"); break;
		case CollisionType::Bottom:ImGui::Text("Bottom Hit"); break;
		case CollisionType::Left:ImGui::Text("Left Hit"); break;
		case CollisionType::Right:ImGui::Text("Right Hit"); break;
		default:
			break;
		};
#endif // DEBUG
	}




}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner)
{
	Vector3 offsetTable[kNumCorners] = {
		{+playerParameter.kWidth / 2.0f,-playerParameter.kHeight / 2.0f,0},
		{+playerParameter.kWidth / 2.0f,+playerParameter.kHeight / 2.0f,0},
		{-playerParameter.kWidth / 2.0f,+playerParameter.kHeight / 2.0f,0},
		{-playerParameter.kWidth / 2.0f,-playerParameter.kHeight / 2.0f,0},
	};
	return center + offsetTable[static_cast<uint32_t>(corner)];
}

bool Player::CheackCollisionPoints(const std::array<Vector3, 2>& posList, CollisionType type, CollisionMapInfo& mapInfo)
{
	/// ブロックとの当たり判定
	bool hit = false;

	/// 特殊ブロックごとに処理を追加
	for (const auto& pos : posList) {
		MapIndex mapIndex = mapChipField->GetMapChipIndexSetByPosition(pos);
		BlockType blockType = mapChipField->GetMapChipTypeByIndex(mapIndex);

		/*	＠例
		if(blockType == MapChipType::〇〇〇){
			if(Type == CollisionType::〇〇〇){
				hit = true;
				// onGround = true;
			}
		}*/

		if (IsHitTargetBlockType(blockType)) {
			hit = true;
		} else if (blockType == BlockType::kGoalDown || blockType == BlockType::kGoalUp) {
			goalFlag = true;
		}

	}
	/// 当たった場合の処理
	if (hit) {
		/// 移動量を0にする
		Vector3 position = playerModel->GetTranslate();
		MapIndex mapIndex = mapChipField->GetMapChipIndexSetByPosition(position);

		Rect rect = mapChipField->GetRectByIndex(mapIndex);
		switch (type) {
		case CollisionType::Top:
			mapInfo.move.y =
				std::max(0.0f,
					rect.bottom - position.y -
					(playerParameter.kHeight / 2.0f + playerParameter.blank));

			mapInfo.ceiling = true;
			break;
		case CollisionType::Bottom:
			mapInfo.move.y = std::min(
				0.0f,
				rect.top - position.y + (playerParameter.kHeight / 2.0f + playerParameter.blank));
			mapInfo.landing = true;
			break;

		case CollisionType::Left:
			mapInfo.move.x = std::max(
				0.0f,
				rect.right - position.x - (playerParameter.kWidth / 2.0f + playerParameter.blank));
			mapInfo.hitWall = true;
			break;
		case CollisionType::Right:
			mapInfo.move.x = std::min(
				0.0f,
				rect.left + position.x + (playerParameter.kWidth / 2.0f + playerParameter.blank));
			mapInfo.hitWall = true;
			break;
		default:
			Logger::Log("未対応の衝突タイプです / ヒットはしてるけど判定部分でおかしい");
			break;

		}


	}
	return hit;
}

bool Player::IsHitTargetBlockType(BlockType type)
{
	switch (type)
	{
		/// 判定を持たないブロックタイプ
	case BlockType::Air:
		return false;
		break;
		/// 判定を持つブロックタイプ
	case BlockType::NormalBlock:
	case BlockType::testBlock:
		return true;
		break;
	/// 未対応のブロックタイプ / エラー文も表示
	default:
		Logger::Log("未対応のブロックタイプです");
		return false;
		break;
	}
}
// 天井との衝突判定
void Player::CeilingCollisionMove(const CollisionMapInfo& mapInfo)
{
	/// 天井に当たったら上昇力を０に
	if (mapInfo.ceiling) {
		velocity.y = 0.0f;
	}
}
// 地面との衝突判定
void Player::LandingCollisionMove(const CollisionMapInfo& mapInfo)
{
	if (onGround /*&& mapInfo.landing*/) {

		/// 高さの加速度が０以上なら空中判定
		if (velocity.y > 0.0f) {
			onGround = false;
		}

		/// 着地しているなら移動量を０に
		else {
			// 移動後の四つ角の計算
			std::array<Vector3, kNumCorners> newPositions;
			for (uint32_t i = 0; i < newPositions.size(); ++i) {
				Vector3 position = transform.translate + mapInfo.move;
				newPositions[i] = CornerPosition(position, static_cast<Corner>(i));
			}
			// 四つ角のブロックタイプを取得
			BlockType blockType{};
			// 真下の当たり判定
			bool hit = false;

			// 左点の判定

			MapIndex mapIndex;
			mapIndex = mapChipField->GetMapChipIndexSetByPosition(newPositions[static_cast<uint32_t>(Corner::kLeftBottom)] + Vector3(0, -playerParameter.kEpsilon, 0));

			/// 各ブロックを種別に判定
			// 乗ることのできるブロック
			if (IsGroundTile(blockType)) {
				hit = true;
			}
			// ゴールブロック
			if(IsGoalTile(blockType)){
				goalFlag = true;
			}

			// 右点の判定
			mapIndex = mapChipField->GetMapChipIndexSetByPosition(newPositions[static_cast<uint32_t>(Corner::kRightBottom)] + Vector3(0, -playerParameter.kEpsilon, 0));
			blockType = mapChipField->GetMapChipTypeByIndex(mapIndex);
			/// 各ブロックを種別に判定
			// 乗ることのできるブロック
			if (IsGroundTile(blockType)) {
				hit = true;
			}
			// ゴールブロック
			if (IsGoalTile(blockType)) {
				goalFlag = true;
			}

		}
	} else {
		/// 着地しているなら高さの移動量を０に
		if (mapInfo.landing) {
			onGround = true;
			velocity.y = 0.0f;
		}
	}
}

// 壁との衝突判定
void Player::WallCollisionMove(const CollisionMapInfo& mapInfo)
{
	if (!mapInfo.hitWall)return;

	/// どちら側の壁か判定
	if (mapInfo.move.x > 0.0f) {
		/// 左側の壁なら
		if (velocity.x < 0.0f) {
			velocity.x = 0.0f;
		}
		/// 右側の壁なら
		else if(mapInfo.move.x < 0.0f) {
			if (velocity.x > 0.0f) {
				velocity.x = 0.0f;
			}
		}
	}
	/// 壁に当たったら移動速度を減衰させる
	velocity.x *= (1.0f - playerParameter.kEpsilon);
}

bool Player::IsGroundTile(BlockType type)
{
	switch (type)
	{
	case BlockType::NormalBlock:
	case BlockType::testBlock:
		return true;
		break;
	default:
		return false;
		break;
	}
}

bool Player::IsGoalTile(BlockType type)
{
	return type == BlockType::kGoalUp || type == BlockType::kGoalDown;
}

void Player::DrawHitImgui()
{
#ifdef _DEBUG

#endif // DEBUG
}


void Player::Finalize()
{
	//// 終了処理
	//EffectManager::GetInstance()->DeleteEffectGroup("ib");
	//qux.reset();
	//EffectManager::GetInstance()->DeleteEffectGroup("id");
	//quux.reset();
}