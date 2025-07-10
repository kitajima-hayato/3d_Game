#pragma once
#include "MyMath.h"
struct AABB
{
	Vector3 min;
	Vector3 max;
	
	bool Intersects(const AABB& other)const {
		return	
			(min.x <= other.max.x && max.x >= other.min.x) &&
			(min.y <= other.max.y && max.y >= other.min.y) &&
			(min.z <= other.max.z && max.z >= other.min.z);
	}
};

class Collider
{

public:
	enum class Type
	{
		Player,
		Enemy,
	};

	virtual ~Collider() = default;

	virtual Type GetType() const = 0;
	virtual AABB GetAABB() const = 0;

	virtual void OnCollision(Collider* other) = 0;
};

