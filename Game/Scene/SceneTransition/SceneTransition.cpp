#include "SceneTransition.h"
#include "engine/bace/ImGuiManager.h"
#include "engine/2d/SpriteCommon.h"
#include "Input.h"



void SceneTransition::Initialize()
{
	spriteDate = {
		// pos
		{0.0f,50.0f},
		// size
		{0.0f,0.0f},
		// rotate
		{0.0f},
		// color
		{1.0f,1.0f,1.0f,1.0f}
	};
	// スプライトカウント分インスタンスを生成
	for (uint32_t i = 0; i < spriteCount; i++) {
		Sprite s;
		s.Initialize(textureFilePath);

		// 左端から順番に / 位置を調整
		Vector2 pos = { i * (1920.0f / spriteCount),720.0f };
		s.SetPosition(pos);

		// サイズ / 左端から右端まで均等に並べる
		s.SetSize({ 1920.0f / spriteCount,1080.0f });

		s.SetColor({ 1.0f,1.0f,1.0f,0.0f });

		// 登録
		sprites.push_back(s);
	}

}

void SceneTransition::Update()
{
	Imgui();
	static float timer = 0.0f;
	timer += 1.0f;
	

	
		if (!isTransition) {
			for (size_t i = 0; i < sprites.size(); ++i) {
				auto& s = sprites[i];

				float delay = i * 10.0f; // 段階的な遅延
				// 画面を覆うまでフェードイン
				if (timer > delay && timer < delay + 75.0f) {
					Vector2 pos = s.GetPosition();
					pos.y -= 10.0f; // 上昇
					s.SetPosition(pos);
				}
				s.Update();
			}
			if (timer > 75.0f + (spriteCount - 1) * 10.0f) {
				isTransition = true;
				timer = 0.0f;
			}
		} else {
			for (size_t i = 0; i < sprites.size(); ++i) {
				auto& s = sprites[i];
				float delay = i * 10.0f; // 段階的な遅延
				// 画面を覆うまでフェードイン
				if (timer > delay && timer < delay + 75.0f) {
					Vector2 pos = s.GetPosition();
					pos.y += 10.0f; // 下降
					s.SetPosition(pos);
				}
				s.Update();
			}
			if (timer > 75.0f + (spriteCount - 1) * 10.0f) {
				isTransition = false;
				timer = 0.0f;
			}
		}
	
		
	
}



void SceneTransition::Draw()
{
	//sprite->Draw();

	for (auto& s : sprites) {
		s.Draw();
	}
}

void SceneTransition::Imgui()
{
#ifdef _DEBUG
	// Imguiの描画処理
	ImGui::Begin("SceneTransition");
	ImGui::DragFloat("SpriteCount", reinterpret_cast<float*>(&spriteCount), 1.0f, 1.0f, 100.0f);
	ImGui::End();
#endif // DEBUG
}

float SceneTransition::RandomRange(float min, float max)
{
	static std::mt19937 mt{ std::random_device{}() };
	std::uniform_real_distribution<float> dist(min, max);
	return dist(mt);
}
