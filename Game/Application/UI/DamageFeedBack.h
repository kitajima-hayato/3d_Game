#pragma once
#include "Game/Camera/Camera.h"
#include "Game/Application/Player/Player.h"
class DamageFeedBack
{

public:
	void Initialize();

	void Update(float dt, Camera& cam,Player& player);

	void Draw();
};

