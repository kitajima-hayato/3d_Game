#include "Block.h"
#include "engine/3d/ModelManager.h"

void Block::Initialize(BlockType blockType, Vector3 position)
{
	/// モデルの初期化
	blockModel = std::make_unique<Object3D>();
	blockModel->Initialize();
	switch (blockType) {
	case BlockType::Air:
		blockModel->SetModel("nullBlock.obj");
		break;
	case BlockType::NormalBlock:
		blockModel->SetModel("nullBlock.obj");
		break;
	default:
		blockModel->SetModel("nullBlock.obj");
		break;
	}
	blockModel->SetTranslate(position);

}



void Block::Update() {
	blockModel->Update();
}

void Block::Draw() {

	blockModel->Draw();
}
