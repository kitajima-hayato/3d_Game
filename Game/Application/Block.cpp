#include "Block.h"
#include "engine/3d/ModelManager.h"

void Block::OnCollision(Collider* other)
{
	
}


void Block::Initialize(BlockType blockType, Vector3 position){
	/// モデルの初期化
	this->blockType = blockType;
	blockModel = std::make_unique<Object3D>();
	blockModel->Initialize();
	switch (this->blockType) {
	case BlockType::Air:
		break;
	case BlockType::NormalBlock:
		blockModel->SetModel("nullBlock.obj");
		break;
	case BlockType::testBlock:
		blockModel->SetModel("XYZLabel.obj");
		break;
	default:
		blockModel->SetModel("nullBlock.obj");
		break;
	}
	blockModel->SetTranslate(position);

	aabb = {
		position - Vector3(0.5f, 0.5f, 0.5f),
		position + Vector3(0.5f, 0.5f, 0.5f)
	};
}



void Block::Update() {
	if (blockType == BlockType::Air) return;
	blockModel->Update();
}

void Block::Draw() {
	if (blockType == BlockType::Air) return;
	blockModel->Draw();
}
