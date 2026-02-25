#include "ModelList.h"
#include "ModelManager.h"
void ModelList::LoadAllModel()
{
	/// LoadModelList
	ModelManager::GetInstance().LoadModel("cubeR.obj");
	ModelManager::GetInstance().LoadModel("Cube.obj");
							   
							   
	/// TitleScene			   
	// プレイヤー
	ModelManager::GetInstance().LoadModel("Player.obj");
	// レインボープレーン		 
	ModelManager::GetInstance().LoadModel("RainbowPlane.obj");
	// タイトルロゴ			   
	ModelManager::GetInstance().LoadModel("title.obj");
	ModelManager::GetInstance().LoadModel("Fluff.obj");
	ModelManager::GetInstance().LoadModel("Flower_Pink.obj");
	ModelManager::GetInstance().LoadModel("BackGround/Flower_Purple.obj");
	ModelManager::GetInstance().LoadModel("Flower_Orange.obj");
	// タイトル背景			   
	ModelManager::GetInstance().LoadModel("Back3.obj");
	ModelManager::GetInstance().LoadModel("back2.obj");
	ModelManager::GetInstance().LoadModel("back1.obj");
	ModelManager::GetInstance().LoadModel("BackSky.obj");
	ModelManager::GetInstance().LoadModel("soil.obj");
							   
	/// StageSelectScene	   
	ModelManager::GetInstance().LoadModel("SelectBaseGround.obj");
	ModelManager::GetInstance().LoadModel("Node.obj");
	ModelManager::GetInstance().LoadModel("RoutePlane.obj");
							   
	/// GamePlayScene		   
	ModelManager::GetInstance().LoadModel("Grass.obj");
	ModelManager::GetInstance().LoadModel("nullBlock.obj");
	ModelManager::GetInstance().LoadModel("XYZLabel.obj");
	ModelManager::GetInstance().LoadModel("breakBlock.obj");
	ModelManager::GetInstance().LoadModel("moveBlock.obj");
	ModelManager::GetInstance().LoadModel("sand.obj");
	ModelManager::GetInstance().LoadModel("Unbreakable.obj");
							   
	ModelManager::GetInstance().LoadModel("GoalUp.obj");
	ModelManager::GetInstance().LoadModel("GoalDown.obj");
							   
	ModelManager::GetInstance().LoadModel("Tentativeenemy.obj");
}
