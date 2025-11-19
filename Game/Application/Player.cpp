#include "Player.h"
#include <algorithm>
#include <Input.h>
#ifdef USE_IMGUI
#include "engine/bace/ImGuiManager.h"
#endif
#include <Logger.h>

void Player::Initialize(Vector3 position)
{
	playerModel_ = std::make_unique<Object3D>();
	playerModel_->Initialize();
	playerModel_->SetTranslate(position);
	playerModel_->SetModel("Player.obj");
	SetDeathHeight(-1.0f);

}


void Player::Update()
{
	// プレイヤーの挙動更新
	UpdateBehavior();

#ifdef USE_IMGUI
	ImGui();
#endif 
	// モデルの更新
	playerModel_->Update();
}



void Player::Draw()
{
	// モデルの描画
	playerModel_->Draw();
}

void Player::UpdateBehavior()
{
	// マップがセットされていなかったら処理しない
	if (!map_)return;

	// 死亡していたらリセット処理
	if (isDead_) {
		DebugPlayerReset();
		return;
	}

	// 移動処理
	Move();
	// ジャンプ処理
	Jump();
	// 当たり判定処理
	CollisionMapInfo collisionInfo;
	// 地面にいなかったらリセット
	if (playerModel_->GetTranslate().y < deathHeight_ && !onGround_)
	{
		// 死亡処理
		isDead_ = true;
	}
	// 移動量の反映
	collisionInfo.move = velocity_;
	// マップ衝突チェック
	MapCollision(collisionInfo);
	// 天井衝突処理
	CellingCollisionMove(collisionInfo);
	// 床衝突処理
	LandingCollisionMove(collisionInfo);
	// 壁衝突処理
	WallCollisionMove(collisionInfo);

	// 速度反映
	PlayerCollisionMove(collisionInfo);
}


void Player::Move()
{
	// 左右移動入力
	bool DIK_D_ = Input::GetInstance()->PushKey(DIK_D);
	bool DIK_A_ = Input::GetInstance()->PushKey(DIK_A);

	Vector3 acceleration = {};
	if (DIK_D_ || DIK_A_) {
		if (DIK_D_ && !DIK_A_) {
			if (velocity_.x < 0.0f) {
				velocity_.x *= (1.0f - status_.kAttenuation);
			}
			acceleration.x += status_.kAcceleration;

		} else if (!DIK_D_ && DIK_A_) {
			if (velocity_.x > 0.0f) {
				velocity_.x *= (1.0f - status_.kAttenuation);
			}
			acceleration.x -= status_.kAcceleration;
		}
		velocity_.x += acceleration.x;
		velocity_.x = std::clamp(velocity_.x, -status_.kMaxSpeed, status_.kMaxSpeed);

	} else {
		//// 入力が無いときは減速(0に近づける)
		//if (velocity_.x > 0.0f) {
		//	velocity_.x -= status_.kAttenuation;
		//	if (velocity_.x < 0.0f) {
		//		velocity_.x = 0.0f;
		//	}
		//} else if (velocity_.x < 0.0f) {
		//	velocity_.x += status_.kAttenuation;
		//	if (velocity_.x > 0.0f) {
		//		velocity_.x = 0.0f;
		//	}
		//}

		velocity_.x *= (1.0f - status_.kAttenuation);

	}

	//// 速度反映
	//Vector3 pos = playerModel_->GetTranslate();
	//pos.x += velocity_.x;
	//playerModel_->SetTranslate(pos);


}




void Player::Jump()
{
	// 地面にいる場合
	if (onGround_) {
		// ジャンプキーが押されたら
		if (Input::GetInstance()->PushKey(DIK_SPACE)) {
			// ジャンプ処理
			velocity_.y = status_.kJumpPower;
		}
	} else {
		// 重力適応処理
		velocity_.y += -status_.kGravity;
		velocity_.y = (std::max)(velocity_.y, -status_.kMaxFallSpeed);
	}

}

void Player::ImGui()
{
#ifdef USE_IMGUI
	ImGui::Begin("Player Info");
	Vector3 pos = playerModel_->GetTranslate();
	ImGui::Text("Position: (%.2f, %.2f, %.2f)", pos.x, pos.y, pos.z);
	ImGui::Text("Velocity: (%.2f, %.2f, %.2f)", velocity_.x, velocity_.y, velocity_.z);
	ImGui::Text("On Ground: %s", onGround_ ? "Yes" : "No");
	ImGui::Text("Is Dead: %s", isDead_ ? "Yes" : "No");
	ImGui::Text("Is Goal: %s", isGoal_ ? "Yes" : "No");


	ImGui::End();
#endif
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

void Player::CellingCollisionMove(CollisionMapInfo& collisionInfo)
{
	// 天井に衝突しているかどうか
	if (collisionInfo.celling) {
		// 天井に衝突したら移動量を調整
		velocity_.y = 0.0f;
	}
}

void Player::LandingCollisionMove(CollisionMapInfo& collisionInfo)
{
	// 床に衝突しているかどうか
	if (onGround_) {
		if (velocity_.y > 0.0f) {
			// 上昇している場合は床に衝突しない
			onGround_ = false;
		} else {
			// 移動後の過度の計算
			std::array<Vector3, kNumCorners> newPositions;
			for (uint32_t i = 0; i < newPositions.size(); i++) {
				// 各コーナーの新しい位置を計算
				Vector3 position = playerModel_->GetTranslate();
				// 移動量を加算
				position += collisionInfo.move;
				// コーナー位置を取得
				newPositions[i] = CornerPosition(position, static_cast<Corner>(i));
			}
			// 各コーナーの新しい位置で当たり判定を確認
			BlockType blockType;
			// プレイヤー直下の判定
			bool hit = false;

			// 左点の判定
			IndexSet indexSet;
			// 自機の左下がマップチップの何番目にあるのか
			indexSet = map_->GetMapChipIndexSetByPosition(newPositions[kLeftBottom] + Vector3(0.0f, -status_.kEpsilon, 0.0f));
			// 取得したインデックスからそのマップチップの種類を取得
			blockType = map_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			// 取得したマップチップの種類が判定を取るブロックなのか判別
			if (IsHitBlockTable(blockType)) {
				hit = true;
			}
			// ゴールブロックの判定
			if (IsHitGoalBlockTable(blockType)) {
				isGoal_ = true;
			}

			// 右点の判定
			// 自機の右下がマップチップの何番目にあるのか
			indexSet = map_->GetMapChipIndexSetByPosition(newPositions[kRightBottom] + Vector3(0.0f, -status_.kEpsilon, 0.0f));
			// 取得したインデックスからそのマップチップの種類を取得
			blockType = map_->GetMapChipTypeByIndex(indexSet.xIndex, indexSet.yIndex);
			// 取得したマップチップの種類が判定を取るブロックなのか判別
			if (IsHitBlockTable(blockType)) {
				hit = true;
			}
			// ゴールブロックの判定
			if (IsHitGoalBlockTable(blockType)) {
				isGoal_ = true;
			}

			// どちらの点も当たっていなかったら床に衝突していないとする
			if (!hit) {
				onGround_ = false;
			}
		}
	} else {
		if (collisionInfo.landing) {
			// 床に衝突したら移動量を調整
			velocity_.y = 0.0f;
			onGround_ = true;
		}
	}

}

void Player::WallCollisionMove(CollisionMapInfo& collisionInfo)
{
	if (collisionInfo.hitWall) {
		// 壁に衝突したら移動量を調整
		velocity_.x *= (1.0f - status_.kEpsilon);
	}
}

void Player::PlayerCollisionMove(const CollisionMapInfo& info)
{
	// 移動量を反映
	// 判定を取った後の移動量を取得
	Vector3 position = playerModel_->GetTranslate();
	// 移動量を加算
	position += info.move;
	// 位置を設定
	playerModel_->SetTranslate(position);
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
		switch (type) {
		case CollisionType::kTop:
			Logger::Log("hit celling");
			break;
		case CollisionType::kBottom:
			Logger::Log("hit floor");
			break;
		case CollisionType::kRight:
			Logger::Log("hit right");
			break;
		case CollisionType::kLeft:
			Logger::Log("hit left");
			break;
		}
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

void Player::DebugPlayerReset()
{
	// デバッグ用にリセット
	velocity_ = {};
	playerModel_->SetTranslate({ 1.0f,1.0f,0.0f });
	isDead_ = false;
	onGround_ = true;
}

