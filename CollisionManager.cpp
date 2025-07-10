#include "CollisionManager.h"

void CollisionManager::AddCollider(Collider* collider)
{
    /// リストに登録
	colliders.push_back(collider);
}

void CollisionManager::Clear()
{
    ///登録されているコライダーすべてを削除
	colliders.clear();
}

void CollisionManager::CheckAllCollisions()
{
	/// 全てのコライダー同士の衝突判定
    for (size_t i = 0; i < colliders.size(); ++i)
    {
        for (size_t j = i + 1; j < colliders.size(); ++j)
        {
            Collider* a = colliders[i];
            Collider* b = colliders[j];
			/// 衝突判定
            if (IsColliding(a, b))
			{
                /// 衝突している場合、通知を行う
                NotifyCollision(a, b);
            }
        }
    }
}

bool CollisionManager::IsColliding(Collider* a, Collider* b) {

    /// ２つの判定を取る
	return a->GetAABB().Intersects(b->GetAABB());

}

void CollisionManager::NotifyCollision(Collider* a, Collider* b) {
    /// 衝突判定が取れた時通知する
    a->OnCollision(b);
    b->OnCollision(a);
}