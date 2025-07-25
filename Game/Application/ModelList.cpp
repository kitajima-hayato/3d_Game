#include "ModelList.h"
#include "ModelManager.h"
void ModelList::LoadAllModel()
{
	/// LoadModelList
	ModelManager::GetInstance()->LoadModel("cubeR.obj");
	ModelManager::GetInstance()->LoadModel("Cube.obj");


	/// TitleScene
	// 仮オブジェ
	ModelManager::GetInstance()->LoadModel("plane.obj");
	// プレイヤー
	ModelManager::GetInstance()->LoadModel("Player.obj");
	// レインボープレーン
	ModelManager::GetInstance()->LoadModel("RainbowPlane.obj");

	/// GamePlayScene

}
