#include "Block.h"
#include "engine/3d/ModelManager.h"

void Block::OnCollision(Collider* other)
{

}


void Block::Initialize(BlockType blockType, Vector3 position) {

	/// モデルの初期化
	this->blockType = blockType;
	blockModel = std::make_unique<Object3D>();
	blockModel->Initialize();
	transform = {
		// Scale
		{1.0f, 1.0f, 1.0f},
		// Rotate
		{0.0f, 0.0f, 0.0f},
		// Translate
		position
	};


	switch (this->blockType) {
	case BlockType::Air:
		break;
	case BlockType::NormalBlock:
		blockModel->SetModel("nullBlock.obj");
		break;
	case BlockType::testBlock:
		blockModel->SetModel("XYZLabel.obj");
		//元のブロックの大きさが1なので半分に縮小
		transform.scale = { 0.5f,0.5f,0.5f };
		break;
	case BlockType::breakBlock:
		blockModel->SetModel("breakBlock.obj");
		break;
	case BlockType::moveBlock:
		blockModel->SetModel("moveBlock.obj");
		break;
	case BlockType::sandBlock:
		blockModel->SetModel("sand.obj");
		break;
	case BlockType::kGoalUp:
		blockModel->SetModel("GoalUp.obj");
		break;
	case BlockType::kGoalDown:
		blockModel->SetModel("GoalDown.obj");
		break;
	default:
		blockModel->SetModel("null.obj");
		break;
	}
	blockModel->SetTransform(transform);


}



void Block::Update() {
	if (blockType == BlockType::Air) return;
	if (blockType == BlockType::moveBlock) {
		if (moveRight_) {
			moveTime_ += moveSpeed_;
			if (moveTime_ > 1.0f) {
				moveTime_ = 1.0f;
				moveRight_ = false; // 右端に到達したら左へ
			}
		} else {
			moveTime_ -= moveSpeed_;
			if (moveTime_ < 0.0f) {
				moveTime_ = 0.0f;
				moveRight_ = true; // 左端に到達したら右へ
			}
		}

		// ---- イージング関数（easeInOutSine） ----
		// 0～1 → 0～1へ滑らかに補間
		float eased = 0.5f - 0.5f * cosf(moveTime_ * 3.14159f);

		// -range ～ +range の範囲で往復
		transform.translate.x = (eased * 2.0f - 1.0f) * moveRange_;

		// モデルに反映
		blockModel->SetTransform(transform);
	}
	blockModel->Update();
}

void Block::Draw() {
	if (blockType == BlockType::Air) return;
	blockModel->Draw();
}

Block* Block::CreateBlock(BlockType blockType, Vector3 position)
{
	Block* newBlock = new Block();
	newBlock->Initialize(blockType, position);
	return newBlock;
}
