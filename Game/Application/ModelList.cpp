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
	// タイトルロゴ
	ModelManager::GetInstance()->LoadModel("title.obj");

	/// GamePlayScene
	ModelManager::GetInstance()->LoadModel("nullBlock.obj");
	ModelManager::GetInstance()->LoadModel("XYZLabel.obj");

	ModelManager::GetInstance()->LoadModel("Player.obj");
	ModelManager::GetInstance()->LoadModel("Tentativeenemy.obj");
}
