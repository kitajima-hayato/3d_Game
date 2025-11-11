#include "Player.h"
#include <algorithm>

void Player::Initialize(Vector3 position)
{
	playerModel_ = std::make_unique<Object3D>();
	playerModel_->Initialize();
	playerModel_->SetTranslate(position);
	playerModel_->SetModel("Player.obj");
	SetDeathHeight(-10.0f);


}


void Player::Update()
{
	UpdateBehavior();
	//　仮のTransform設定
	Transform transform = {
		{1.0f,1.0f,1.0f},
		{0.0f,0.0f,0.0f},
		playerModel_->GetTranslate()
	};
	playerModel_->SetTransform(transform);
	playerModel_->Update();

#ifdef _DEBUG
	ImGui();
#endif // _DEBUG
}



void Player::Draw()
{
	playerModel_->Draw();
}

void Player::UpdateBehavior()
{
	// 移動処理
	Move();
	// 当たり判定処理
	CollisionMapInfo collisionInfo;
	if (playerModel_->GetTranslate().y < deathHeight_)
	{
		// 死亡処理
	}
	// 移動量の反映
	collisionInfo.move = velocity_;
	// マップ衝突チェック
	MapCollision(collisionInfo);


}


void Player::Move()
{
	MoveRight();
	MoveLeft();
}


void Player::MoveRight()
{
}

void Player::MoveLeft()
{
}

void Player::ImGui()
{
}

void Player::MapCollision(CollisionMapInfo& collisionInfo)
{
	CollisionMapInfoDirection(
		collisionInfo,
		CollisionType::kRight,
		{ kRightTop,kRightBottom },
		Vector3(status_.kEpsilon, 0.0f, 0.0f),
		[](const CollisionMapInfo& info) {
			return info.move.x > 0.0f;
		});

	CollisionMapInfoDirection(
		collisionInfo,
		CollisionType::kLeft,
		{ kLeftTop,kLeftBottom },
		Vector3(-status_.kEpsilon, 0.0f, 0.0f),
		[](const CollisionMapInfo& info) {
			return info.move.x < 0.0f;
		});

	CollisionMapInfoDirection(
		collisionInfo,
		CollisionType::kTop,
		{ kRightTop,kLeftTop },
		Vector3(0.0f, 0.0f, 0.0f),
		[](const CollisionMapInfo& info) {
			return info.move.y > 0.0f;
		});

	CollisionMapInfoDirection(
		collisionInfo,
		CollisionType::kBottom,
		{ kRightBottom,kLeftBottom },
		Vector3(0.0f, -status_.kEpsilon, 0.0f),
		[](const CollisionMapInfo& info) {
			return info.move.y < 0.0f;
		});
}

void Player::CollisionMapInfoDirection(CollisionMapInfo& collisionInfo, CollisionType type, const std::array<Corner, 2>& checkCorners, const Vector3& offset, std::function<bool(const CollisionMapInfo&)> mooveCondition)
{
	// 移動量が0なら処理しない
	if (!mooveCondition(collisionInfo))return;
	// 現在の位置に移動量を加算した位置を取得
	Vector3 position = playerModel_->GetTranslate() + collisionInfo.move;

	// ２つのコーナー位置の当たり判定を取得
	std::array<Vector3, 2> points = {
		CornerPosition(position, checkCorners[0]) + offset,
		CornerPosition(position, checkCorners[1]) + offset
	};
	// 当たり判定があったか
	if (CheckCollisionPoints(points, static_cast<CollisionType>(type), collisionInfo)) {
		// 必要であればImGui表示する

	}
}

Vector3 Player::CornerPosition(const Vector3& center, Corner corner)
{
	Vector3 offsetTable[kNumCorners] = {
		{+status_.kWidth / 2.0f, -status_.kHeight / 2.0f, 0.0f},// 右下
		{-status_.kWidth / 2.0f, -status_.kHeight / 2.0f, 0.0f},// 左下
		{+status_.kWidth / 2.0f, +status_.kHeight / 2.0f, 0.0f},// 右上
		{-status_.kWidth / 2.0f, +status_.kHeight / 2.0f, 0.0f},// 左上
	};
	return center + offsetTable[static_cast<uint32_t>(corner)];
}

bool Player::CheckCollisionPoints(const std::array<Vector3, 2>& posList, CollisionType type, CollisionMapInfo& collisionInfo)
{
	// 当たり判定フラグ
	bool isHit = false;

	for (const auto& pos : posList) {
		// 判定を行うマップチップのインデックスを取得
		IndexSet index = map_->GetMapChipIndexSetByPosition(pos);
		// マップチップの種類を取得
		BlockType chip = map_->GetMapChipTypeByIndex(index.xIndex, index.yIndex);

		if (IsHitBlockTable(chip)) {
			isHit = true;
		} else if (IsHitGoalBlockTable(chip)) {
			// ゴール処理
			isGoal_ = true;
		}

	}

	if (isHit) {
		Vector3 position = playerModel_->GetTranslate();
		IndexSet index = map_->GetMapChipIndexSetByPosition(position);

		Rect rect = map_->GetRectByIndex(index.xIndex, index.yIndex);

		switch (type)
		{
		case CollisionType::kTop:
			collisionInfo.celling = true;
			collisionInfo.move.y = (std::max)(0.0f, rect.bottom - position.y - (status_.kHeight / 2.0f + status_.kBlank));
			break;
		case CollisionType::kBottom:
			collisionInfo.landing = true;
			collisionInfo.move.y = (std::min)(0.0f, rect.top - position.y + (status_.kHeight / 2.0f + status_.kBlank));
			break;
		case CollisionType::kRight:
			collisionInfo.hitWall = true;
			collisionInfo.move.x = (std::max)(0.0f, rect.left - position.x - (status_.kWidth / 2.0f + status_.kBlank));
			break;
		case CollisionType::kLeft:
			collisionInfo.hitWall = true;
			collisionInfo.move.x = (std::min)(0.0f, rect.right - position.x + (status_.kWidth / 2.0f + status_.kBlank));
			break;
		}
	}

	return isHit;

}

bool Player::IsHitBlockTable(BlockType type)
{
	switch (type)
	{
	case BlockType::NormalBlock:
	case BlockType::testBlock:
	case BlockType::kGoalUp:
	case BlockType::kGoalDown:
		return true;
	default:
		return false;
	}
}

bool Player::IsHitGoalBlockTable(BlockType type)
{
	switch (type) {
	case BlockType::kGoalUp:
	case BlockType::kGoalDown:
		return true;
	default:
		return false;
	}
}

