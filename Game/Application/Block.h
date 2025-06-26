#pragma once
#include "engine/3d/Object3D.h"
#include "BlockType.h"
class Block
{

public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(BlockType blockType, Vector3 position);

	/// <summary>
	/// 更新処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

public:	// Setter / Getter
	/// <summary>
	/// ブロックのSRTを取得
	/// </summary>
	/// <returns>ブロックのSRT</returns>
	const Transform& GetTransform() const { return transform; }

	/// <summary>
	/// ブロックの種類を取得
	/// </summary>
	/// <returns>ブロックの種類</returns>
	BlockType GetBlockType() const { return blockType; }

	/// <summary>
	/// ブロックのScaleをセット
	/// </summary>
	void SetScale(const Vector3& scale) { transform.scale = scale; }

	/// <summary>
	/// ブロックのRotateをセット
	/// </summary>
	void SetRotate(const Vector3& rotate) { transform.rotate = rotate; }

	/// <summary>
	/// ブロックのPositionをセット
	/// </summary>
	void SetTranslate(const Vector3& position) { transform.translate = position; }

	/// <summary>
	/// ブロックのSRTをセット
	/// </summary>
	void SetTransform(const Transform& transform) { this->transform = transform; }
private:
	/// ブロックモデル
	std::unique_ptr<Object3D> blockModel;
	/// ブロックの位置
	Transform transform;
	/// 表示するブロックの種類
	BlockType blockType;
};

