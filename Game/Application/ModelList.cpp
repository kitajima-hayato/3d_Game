#include "ModelList.h"
#include "ModelManager.h"
void ModelList::LoadAllModel()
{
	/// LoadModelList
	ModelManager::GetInstance()->LoadModel("cubeR.obj");
	ModelManager::GetInstance()->LoadModel("Cube.obj");


	/// TitleScene
	// プレイヤー
	ModelManager::GetInstance()->LoadModel("Player.obj");
	// レインボープレーン
	ModelManager::GetInstance()->LoadModel("RainbowPlane.obj");
	// タイトルロゴ
	ModelManager::GetInstance()->LoadModel("title.obj");
	// タイトル背景
	ModelManager::GetInstance()->LoadModel("Back3.obj");
	ModelManager::GetInstance()->LoadModel("Back3Reverse.obj");
	ModelManager::GetInstance()->LoadModel("back2.obj");
	ModelManager::GetInstance()->LoadModel("Back2Reverse.obj");
	ModelManager::GetInstance()->LoadModel("back1.obj");
	ModelManager::GetInstance()->LoadModel("BackSky.obj");
	ModelManager::GetInstance()->LoadModel("soil.obj");

	/// StageSelectScene
	ModelManager::GetInstance()->LoadModel("SelectBaseGround.obj");

	/// GamePlayScene
	ModelManager::GetInstance()->LoadModel("Grass.obj");
	ModelManager::GetInstance()->LoadModel("SceneChange.obj");
	ModelManager::GetInstance()->LoadModel("nullBlock.obj");
	ModelManager::GetInstance()->LoadModel("XYZLabel.obj");
	ModelManager::GetInstance()->LoadModel("BreakBlock.obj");
	ModelManager::GetInstance()->LoadModel("moveBlock.obj");
	ModelManager::GetInstance()->LoadModel("sand.obj");
	ModelManager::GetInstance()->LoadModel("Unbreakable.obj");

	ModelManager::GetInstance()->LoadModel("GoalUp.obj");
	ModelManager::GetInstance()->LoadModel("GoalDown.obj");

	ModelManager::GetInstance()->LoadModel("Tentativeenemy.obj");
}
