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
	// タイトル背景
	ModelManager::GetInstance()->LoadModel("back3.obj");
	ModelManager::GetInstance()->LoadModel("back2.obj");
	ModelManager::GetInstance()->LoadModel("back1.obj");
	ModelManager::GetInstance()->LoadModel("BackSky.obj");
	ModelManager::GetInstance()->LoadModel("soil.obj");
	/// GamePlayScene
	ModelManager::GetInstance()->LoadModel("SceneChange.obj");
	ModelManager::GetInstance()->LoadModel("nullBlock.obj");
	ModelManager::GetInstance()->LoadModel("XYZLabel.obj");
	ModelManager::GetInstance()->LoadModel("breakBlock.obj");
	ModelManager::GetInstance()->LoadModel("moveBlock.obj");
	ModelManager::GetInstance()->LoadModel("sand.obj");

	ModelManager::GetInstance()->LoadModel("GoalUp.obj");
	ModelManager::GetInstance()->LoadModel("GoalDown.obj");

	ModelManager::GetInstance()->LoadModel("Player.obj");
	ModelManager::GetInstance()->LoadModel("Tentativeenemy.obj");
}
