#pragma once
#include "BaseScene.h"
#include <string>
// シーン工場クラス
class AbstractSceneFactory
{
public:
	/// <summary>
	/// 仮想デストラクタ
	/// </summary>
	virtual ~AbstractSceneFactory() = default;
	/// <summary>
	/// シーン生成
	/// </summary>
	/// <param name="sceneName"></param>
	/// <returns></returns>
	virtual BaseScene* CreateScene(const std::string&sceneName) = 0;
};

