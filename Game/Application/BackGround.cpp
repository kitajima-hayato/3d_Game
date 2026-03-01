#include "BackGround.h"
#ifdef USE_IMGUI
#include "engine/base/ImGuiManager.h"
#endif

void BackGround::Initialize()
{
	items_.clear();
	items_.reserve(7);

	// 旧ファイルと同じ初期値
	AddItem("skyBack", "GamePlay/BackGround/sky", Transform{
		{ 400.0f,100.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 7.5f,0.0f,250.0f }
		});
	AddItem("titleBackGround3", "GamePlay/BackGround/mountain_far", Transform{
		{ 30.0f,50.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 7.5f,-4.0f,200.0f }
		});

	AddItem("titleBackGround3_2", "GamePlay/BackGround/mountain_far", Transform{
		{ 30.0f,50.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 170.0f,-4.0f,200.0f }
		});

	AddItem("titleBackGround2", "GamePlay/BackGround/mountain_mid", Transform{
		{ 30.0f,60.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 7.5f,-4.0f,150.0f }
		});

	AddItem("titleBackGround2_2", "GamePlay/BackGround/mountain_mid", Transform{
		{ 30.0f,60.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 170.0f,-4.0f,150.0f }
		});

	AddItem("titleBackGround2_3", "GamePlay/BackGround/mountain_mid", Transform{
		{ 30.0f,60.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 350.0f,-4.0f,150.0f }
		});
	
	AddItem("soil", "GamePlay/Blocks/grassblock", Transform{
		{ 40.0f,1.0f,2.0f },
		{ 0.0f,0.0f,0.0f },
		{ 0.0f,-8.0f,20.0f }
		});

	// 花
	AddItem("flower_white_front_L", "GamePlay/BackGround/fluff", Transform{
	  { 2.0f, 2.0f, 2.0f }, { 0.0f, 0.15f, 0.0f }, { 4.1f, -7.7f, 21.0f }
		});
	AddItem("flower_pink_front_L", "GamePlay/BackGround/flower_pink", Transform{
	  { 2.0f, 2.0f, 2.0f }, { 0.0f,-0.10f, 0.0f }, { 6.2f, -8.0f, 21.7f }
		});

	AddItem("flower_orange_front_R", "GamePlay/BackGround/flower_orange", Transform{
	  { 1.5f, 1.5f, 1.5f }, { 0.0f, 0.20f, 0.0f }, { 8.2f, -7.8f, 21.0f }
		});
	AddItem("flower_blue_front_R", "GamePlay/BackGround/flower_purple", Transform{
	  { 2.0f, 2.0f, 2.0f }, { 0.0f,-0.20f, 0.0f }, { 10.6f, -8.0f, 21.2f }
		});

	AddItem("flower_pink_mid", "GamePlay/BackGround/flower_pink", Transform{
	  { 2.0f, 2.0f, 2.0f }, { 0.0f, 0.10f, 0.0f }, { 9.4f, -8.0f, 23.3f }
		});
	AddItem("flower_blue_mid", "GamePlay/BackGround/flower_purple", Transform{
	  { 2.0f, 2.0f, 2.0f }, { 0.0f,-0.15f, 0.0f }, { 13.4f, -8.0f, 21.8f }
		});
}

void BackGround::Update()
{
	
	for (auto& item : items_) {
		item.object->SetTransform(item.transform);
		item.object->Update();
	}

	DrawImgui();
}

void BackGround::Draw()
{
	for (auto& item : items_) {
		item.object->Draw();
	}
}

void BackGround::DrawImgui()
{
#ifdef USE_IMGUI
	ImGui::Begin("BackGround Transform");

	for (auto& item : items_) {
		std::string sScale = item.debugName + " Scale";
		std::string sRotate = item.debugName + " Rotate";
		std::string sTrans = item.debugName + " Translate";

		ImGui::DragFloat3(sScale.c_str(), &item.transform.scale.x, 0.1f);
		ImGui::DragFloat3(sRotate.c_str(), &item.transform.rotate.x, 0.1f);
		ImGui::DragFloat3(sTrans.c_str(), &item.transform.translate.x, 0.1f);

		// 変更を即時反映（次フレーム反映でも良いが、こっちの方が分かりやすい）
		item.object->SetTransform(item.transform);
	}

	ImGui::End();
#endif
}

void BackGround::AddItem(const char* debugName, const char* modelName, const Transform& transform)
{
	BgItem item;
	item.debugName = debugName;
	item.modelName = modelName;
	item.transform = transform;

	item.object = std::make_unique<Object3D>();
	item.object->Initialize();
	item.object->SetModel(modelName);
	item.object->SetTransform(transform);

	items_.push_back(std::move(item));
}