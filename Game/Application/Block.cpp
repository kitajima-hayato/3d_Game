#include "Block.h"
#include "Object3DCommon.h"
#include "ModelCommon.h"
void Block::Initialize()
{
	//modelCommon = 
	object3D = make_unique<Object3D>();
	model = make_unique<Model>();
	//model->Initialize()
	object3D->Initialize(&Object3DCommon::GetInstance());
	object3D->SetModel(model.get());

}

void Block::Update()
{
}

void Block::Draw()
{
}
