#include "ModelList.h"
#include "ModelManager.h"
void ModelList::LoadAllModel()
{
	/// TitleScene
	// 仮オブジェ
	ModelManager::GetInstance()->LoadModel("plane.obj");

	/// GamePlayScene
	ModelManager::GetInstance()->LoadModel("nullBlock.obj");
	ModelManager::GetInstance()->LoadModel("XYZLabel.obj");
}
