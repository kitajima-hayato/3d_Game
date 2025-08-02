#pragma once
#include "engine/math/MyMath.h"
#include "engine/3d/Object3D.h"
class LevelEnemy
{
public:
	/// <summary>
	///  初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

	void SetTransform(const Transform& transform) { this->transform = transform; }
	const Transform& GetTransform() const { return transform; }
	void SetModelName(std::string modelName) { this->modelName = modelName; }
private:
	/// 敵の位置、回転、スケールを保持する変数
	Transform transform;
	
	/// 敵の3Dオブジェクト
	std::unique_ptr<Object3D> enemyObject; 
	/// モデル名
	std::string modelName; 

};

