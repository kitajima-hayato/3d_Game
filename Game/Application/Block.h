#pragma once
#include "engine/3d/Object3D.h"
#include "BlockType.h"
class Block
{

public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(BlockType blockType,Vector3 position);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

private:
	/// ブロックモデル
	std::unique_ptr<Object3D> blockModel;
	/// ブロックの位置
	Transform transform;
	/// 表示するブロックの種類
	BlockType blockType;
};

