#include "SceneTransition.h"
#include "engine/bace/ImGuiManager.h"
#include "engine/2d/SpriteCommon.h"
#include "Input.h"

static inline float Clamp01(float x) { return x < 0.f ? 0.f : (x > 1.f ? 1.f : x); }
static inline float Lerp(float a, float b, float t) { return a + (b - a) * t; }
static inline float EaseInOut(float t) {
	return (t < 0.5f) ? (4.f * t * t * t) : (1.f - powf(-2.f * t + 2.f, 3.f) / 2.f);
}
static inline float EaseIn(float t) { return t * t * t; }
void SceneTransition::Initialize(uint32_t division, uint32_t totalFrame)
{
	// スプライト分割数を設定
	spriteCount = division;

	totalFrames_ = std::max<uint32_t>(2, totalFrame);

	// In / Out を半分ずつ
	inFrames_ = totalFrames_ / 2 + (totalFrames_ % 2);
	outFrames_ = totalFrames_ / 2;

	phase_ = Phase::In;
	frame_ = 0;

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

	sprites.clear();
	sprites.reserve(spriteCount);

	const float w = 1280.0f / spriteCount;

	// スプライトカウント分インスタンスを生成
	for (uint32_t i = 0; i < spriteCount; i++) {
		Sprite s;
		s.Initialize(textureFilePath);
		s.SetPosition({ i * w, 720.0f });

		// サイズ / 左端から右端まで均等に並べる
		s.SetSize({ w,720.0f });

		s.SetColor({ 1.0f,1.0f,1.0f,1.0f });

		// 登録
		sprites.push_back(s);
	}

}

void SceneTransition::Update() {
	//// フェードイン・フェードアウトの処理
	//if (!isTransition) {
	//	for (size_t i = 0; i < sprites.size(); ++i) {
	//		auto& s = sprites[i];
	//		float delay = i * 10.0f; // 段階的な遅延
	//		// 画面を覆うまでフェードイン
	//		if (timer > delay && timer < delay + 75.0f) {
	//			Vector2 pos = s.GetPosition();
	//			pos.y -= 10.0f; // 上昇
	//			s.SetPosition(pos);
	//		}
	//		s.Update();
	//	}
	//	if (timer > 75.0f + (spriteCount - 1) * 10.0f) {
	//		isTransition = true;
	//		timer = 0.0f;
	//	}
	//} else {
	//	for (size_t i = 0; i < sprites.size(); ++i) {
	//		auto& s = sprites[i];
	//		float delay = i * 10.0f; // 段階的な遅延
	//		// 画面を覆うまでフェードイン
	//		if (timer > delay && timer < delay + 75.0f) {
	//			Vector2 pos = s.GetPosition();
	//			pos.y += 10.0f; // 下降
	//			s.SetPosition(pos);
	//		}
	//		s.Update();
	//	}
	//	if (timer > 75.0f + (spriteCount - 1) * 10.0f) {
	//		isTransition = false;
	//		timer = 0.0f;
	//	}
	//}

	Imgui();

	auto updatePhase = [&](uint32_t phaseFrames, bool reverse) {
		float phaseT = (phaseFrames == 0) ? 1.0f :
			static_cast<float>(frame_) / static_cast<float>(phaseFrames);
		phaseT = Clamp01(phaseT);

		for (uint32_t i = 0; i < spriteCount; ++i) {
			auto& s = sprites[i];

			float p = Clamp01(phaseT * spriteCount - static_cast<float>(i));
			p = EaseInOut(p);

			// ここを修正 ↓
			float startY = reverse ? 0.0f : 720.0f;
			float endY = reverse ? 720.0f : 0.0f;

			float y = Lerp(startY, endY, p);

			Vector2 pos = s.GetPosition();
			pos.y = y;
			s.SetPosition(pos);


			s.Update();
		}

		if (frame_ >= phaseFrames) { frame_ = 0; return true; }
		++frame_; return false;
		};
	switch (phase_) {
	case Phase::In:
		// 上方向へ上がる演出（今まで通り）
		if (updatePhase(inFrames_, /*reverse=*/false)) {
			// 上がりきったら一気に下げるフェーズへ
			phase_ = Phase::Out;
			frame_ = 0;
		}
		break;

	case Phase::Out:
	{
		// t: 0→1 を outFrames_ で進める
		float t = (outFrames_ == 0) ? 1.0f
			: static_cast<float>(frame_) / static_cast<float>(outFrames_);
		t = Clamp01(t);
		float te = EaseIn(t); // 落下は加速させる

		// 全スプライト同時に「上(0) → 下(720)」へ落下
		for (auto& s : sprites) {
			Vector2 pos = s.GetPosition();
			float startY = pos.y;         // 直前の位置（通常は 0）
			float endY = 720.0f;        // 画面下
			// startY が 0 でない状況でもスムーズに落ちるよう、Lerp を毎フレーム適用
			pos.y = Lerp(startY, endY, te);
			s.SetPosition(pos);
			s.Update();
		}

		if (frame_ >= outFrames_) {
			phase_ = Phase::Done; // totalFrame 内は Draw 継続（Done でも描画OKにしているならそのまま）
			frame_ = 0;
		} else {
			++frame_;
		}
	}
	break;
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
	ImGui::Begin("SceneTransition Debug");

	ImGui::Text("=== 状態 ===");
	const char* phaseNames[] = { "In", "Out", "Done" };
	ImGui::Text("Phase : %s", phaseNames[static_cast<int>(phase_)]);
	ImGui::Text("Frame : %u / Total %u", frame_, totalFrames_);
	ImGui::Text("In  : %u  Out : %u", inFrames_, outFrames_);
	ImGui::Separator();

	// 進行度バー
	float normalized = 0.0f;
	switch (phase_) {
	case Phase::In:  normalized = static_cast<float>(frame_) / inFrames_;  break;
	case Phase::Out: normalized = static_cast<float>(frame_) / outFrames_; break;
	default: break;
	}
	ImGui::ProgressBar(normalized, ImVec2(200, 20));
	ImGui::Text("Progress : %.2f %%", normalized * 100.0f);

	ImGui::Separator();
	ImGui::Text("=== パラメータ ===");
	ImGui::DragInt("SpriteCount", reinterpret_cast<int*>(&spriteCount), 1, 1, 100);
	ImGui::DragInt("TotalFrames", reinterpret_cast<int*>(&totalFrames_), 1, 10, 600);
	ImGui::DragFloat("BaseY", &baseY_, 1.0f, -2000.0f, 2000.0f);
	ImGui::DragFloat("TravelY", &travelY_, 1.0f, 0.0f, 3000.0f);

	// 動きの確認をしやすくするための手動コントロール
	static float speed = 1.0f;
	ImGui::DragFloat("Speed Multiplier", &speed, 0.01f, 0.1f, 5.0f, "%.2fx");

	// リセットボタン
	if (ImGui::Button("Reset Transition")) {
		Initialize(spriteCount, totalFrames_);
	}

	// 強制フェーズ切り替えボタン
	ImGui::Separator();
	if (ImGui::Button("Force -> In")) { phase_ = Phase::In;  frame_ = 0; }
	ImGui::SameLine();
	if (ImGui::Button("Force -> Out")) { phase_ = Phase::Out; frame_ = 0; }
	ImGui::SameLine();
	if (ImGui::Button("Force -> Done")) { phase_ = Phase::Done; }

	ImGui::Separator();
	ImGui::Text("=== 個別スプライト情報 ===");
	for (size_t i = 0; i < sprites.size(); ++i) {
		const Vector2& pos = sprites[i].GetPosition();
		ImGui::Text("Sprite[%zu] : Y = %.2f", i, pos.y);
	}

	ImGui::End();
#endif
}


float SceneTransition::RandomRange(float min, float max)
{
	static std::mt19937 mt{ std::random_device{}() };
	std::uniform_real_distribution<float> dist(min, max);
	return dist(mt);
}
