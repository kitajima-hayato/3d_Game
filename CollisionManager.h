#pragma once
#include "engine/math/MyMath.h"
#include "Game/Collision/Collider.h"
class CollisionManager
{
public:
	/// <summary>
	/// 判定対象追加
	/// </summary>
	/// <param name="collider"></param>
	void AddCollider(Collider* collider);
	/// <summary>
	/// 追加されたコライダーを全て削除
	/// </summary>
	void Clear();
	/// <summary>
	/// 全コライダー同士の衝突判定を行う
	/// </summary>
	void CheckAllCollisions();

private:
	/// <summary>
	/// ２つのコライダーの衝突判定
	/// </summary>
	bool IsColliding(Collider* a, Collider* b);
	/// <summary>
	/// 衝突が発生した２つのコライダーに通知を行う
	/// </summary>
	void NotifyCollision(Collider* a, Collider* b);

private:
	/// <summary>
	/// 判定対象リスト
	/// </summary>
	std::vector<Collider*>colliders;
	
	
};

