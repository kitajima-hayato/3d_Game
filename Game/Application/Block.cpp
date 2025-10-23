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
		transform.scale = { 0.5f,0.5f,0.5f };
		break;
	default:
		blockModel->SetModel("nullBlock.obj");
		break;
	}
	blockModel->SetTransform(transform);

	
}



void Block::Update() {
	if (blockType == BlockType::Air) return;
	blockModel->Update();
}

void Block::Draw() {
	if (blockType == BlockType::Air) return;
	blockModel->Draw();
}
