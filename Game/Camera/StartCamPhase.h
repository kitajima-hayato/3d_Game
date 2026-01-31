#pragma once
#include "MyMath.h"
#include "Game/Camera/Camera.h"
class StartCamPhase
{


public:
	void Start(const Vector3& startPos);

	void Update(float dt,Camera&cam);

	bool IsRunning()const;

	void DrawImgui();

};

