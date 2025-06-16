#include "ModelList.h"
#include "ModelManager.h"
void ModelList::LoadAllModel()
{
	/// LoadModelList
	ModelManager::GetInstance()->LoadModel("cubeR.obj");


	/// TitleScene
	// 仮オブジェ
	ModelManager::GetInstance()->LoadModel("plane.obj");

	/// GamePlayScene

}
