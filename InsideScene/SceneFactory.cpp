#include "SceneFactory.h"
#include "Game/Scene/TitleScene.h"
#include "Game/Scene/GamePlayScene.h"
#include "Game/Scene/StageSelectScene.h"
#include "Game/Scene/StageClearScene.h"
BaseScene* SceneFactory::CreateScene(const std::string& sceneName)
{
    // 次のシーン生成
	BaseScene* scene = nullptr;
	if (sceneName == "TITLE")
	{
		scene = new TitleScene();
	}
	else if (sceneName == "STAGESELECT")
	{
		scene = new StageSelectScene();
	}
	else if (sceneName == "GAMEPLAY")
	{
		scene = new GamePlayScene();
	}
	else if (sceneName == "STAGECLEAR")
	{
		scene = new StageClearScene();
	}
	return scene;
}
