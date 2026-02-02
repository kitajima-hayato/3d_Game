#pragma once
#include "AbstractSceneFactory.h"
#include <memory>
/// <summary>
/// シーン工場クラス
/// </summary>
class SceneFactory : public AbstractSceneFactory
{
public:
	/// <summary>
	/// シーン生成
	/// </summary>
	/// <param name="sceneName"></param>
	/// <returns></returns>
	std::unique_ptr<BaseScene> CreateScene(const std::string& sceneName)override;
};

