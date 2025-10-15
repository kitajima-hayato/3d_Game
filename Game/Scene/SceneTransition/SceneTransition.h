#pragma once
#include <memory>
#include "engine/3d/Object3D.h"
#include "engine/2d/Sprite.h"
#include <random>


class SceneTransition
{
	public:
	enum class Type {
		kNone,
		kFadeIn,
		kFadeOut,
	};

	void Initialize();

	void Update();

	void Draw();

	void Imgui();

	float RandomRange(float min, float max);


	
private:
	SpriteData spriteDate;
	std::unique_ptr<Sprite> sprite;

	std::vector<Sprite> sprites;
	uint32_t spriteCount = 10;

	bool isTransition = false;

	bool isFadeIn = false;

	std::string textureFilePath = "resources/Black.png";

};

