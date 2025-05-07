#pragma once
#include "Object3D.h"
class Block
{
public:
	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();

private:
	// モデル
	std::unique_ptr<Object3D> object3D;
	std::unique_ptr<Model> model;
};

