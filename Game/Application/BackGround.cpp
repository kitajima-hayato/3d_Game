#include "BackGround.h"
#ifdef USE_IMGUI
#include "engine/base/ImGuiManager.h"
#endif
void BackGround::Initialize()
{
	titleBackGround3 = std::make_unique<Object3D>();
	titleBackGround3->Initialize();
	titleBackGround3->SetModel("back3.obj");
	titleBackGround3Transform = {
		{ 1.0f,50.0f,30.0f },
		{ 0.0f,-1.5f,0.0f },
		{ 7.5f,-4.0f,200.0f }
	};
	titleBackGround3->SetTransform(titleBackGround3Transform);

	// 右に繋げる
	titleBackGround3_2 = std::make_unique<Object3D>();
	titleBackGround3_2->Initialize();
	titleBackGround3_2->SetModel("back3.obj");
	titleBackGround3_2Transform = {
		{ 1.0f,50.0f,30.0f },
		{ 0.0f,1.7f,0.0f },
		{ 170.0f,-4.0f,200.0f }
	};
	titleBackGround3_2->SetTransform(titleBackGround3_2Transform);

	titleBackGround2 = std::make_unique<Object3D>();
	titleBackGround2->Initialize();
	titleBackGround2->SetModel("back2.obj");
	titleBackGround2Transform = {
		{ 1.0f,60.0f,30.0f },
		{ 0.0f,-1.5f,0.0f },
		{ 7.5f,-4.0f,150.0f }
	};

	titleBackGround2->SetTransform(titleBackGround2Transform);


	titleBackGround2_2 = std::make_unique<Object3D>();
	titleBackGround2_2->Initialize();
	titleBackGround2_2->SetModel("back2.obj");
	titleBackGround2_2Transform = {
		{ 1.0f,60.0f,30.0f },
		{ 0.0f,1.5f,0.0f },
		{ 180.0f,-4.0f,150.0f }
	};
	titleBackGround2_2->SetTransform(titleBackGround2_2Transform);
	titleBackGround2_3 = std::make_unique<Object3D>();
	titleBackGround2_3->Initialize();
	titleBackGround2_3->SetModel("back2.obj");
	titleBackGround2_3Transform = {
		{ 1.0f,60.0f,30.0f },
		{ 0.0f,-1.5f,0.0f },
		{ 350.0f,-4.0f,150.0f }
	};
	titleBackGround2_3->SetTransform(titleBackGround2_3Transform);



	titleBackGround1 = std::make_unique<Object3D>();
	titleBackGround1->Initialize();
	titleBackGround1->SetModel("back1.obj");
	titleBackGround1Transform = {
		{ 1.0f,20.0f,10.0f },
		{ 0.0f,-1.5f,0.0f },
		{ 7.5f,-2.0f,50.0f }
	};
	titleBackGround1->SetTransform(titleBackGround1Transform);

	titleBackGround1_2 = std::make_unique<Object3D>();
	titleBackGround1_2->Initialize();
	titleBackGround1_2->SetModel("back1.obj");
	titleBackGround1_2Transform = {
		{ 1.0f,20.0f,10.0f },
		{ 0.0f,1.5f,0.0f },
		{ 65,-2.0f,50.0f }
	};
	titleBackGround1_2->SetTransform(titleBackGround1_2Transform);
	titleBackGround1_3 = std::make_unique<Object3D>();
	titleBackGround1_3->Initialize();
	titleBackGround1_3->SetModel("back1.obj");
	titleBackGround1_3Transform = {
		{ 1.0f,20.0f,10.0f },
		{ 0.0f,-1.5f,0.0f },
		{ 120,-2.0f,50.0f }
	};
	titleBackGround1_3->SetTransform(titleBackGround1_3Transform);

	titleBackGround1_4 = std::make_unique<Object3D>();
	titleBackGround1_4->Initialize();
	titleBackGround1_4->SetModel("back1.obj");
	titleBackGround1_4Transform = {
		{ 1.0f,20.0f,10.0f },
		{ 0.0f,1.5f,0.0f },
		{ 178,-2.0f,50.0f }
	};
	titleBackGround1_4->SetTransform(titleBackGround1_4Transform);

	skyBack = std::make_unique<Object3D>();
	skyBack->Initialize();
	skyBack->SetModel("BackSky.obj");
	skyBackTransform = {
		{ 120.0f,400.0f,1.0f },
		{ 0.0f,0.0f,-1.5f },
		{ 7.5f,0.0f,250.0f }
	};
	skyBack->SetTransform(skyBackTransform);

	// 足場　土
	soil = std::make_unique<Object3D>();
	soil->Initialize();
	soil->SetModel("soil.obj");
	soilTransform = {
		{ 7.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 0.0f,-8.6f,20.0f }
	};
	soil->SetTransform(soilTransform);
}


void BackGround::Update()
{
	skyBack->Update();

	// 背景
	// 少しづつ左に流す 更新
	

	titleBackGround3->Update();
	titleBackGround3_2->Update();
	// 2
	//titleBackGround2Transform.translate.x -= 0.05f;
	//titleBackGround2_2Transform.translate.x -= 0.05f;
	//titleBackGround2_3Transform.translate.x -= 0.05f;
	//titleBackGround2->SetTransform(titleBackGround2Transform);
	//titleBackGround2_2->SetTransform(titleBackGround2_2Transform);
	//titleBackGround2_3->SetTransform(titleBackGround2_3Transform);
	titleBackGround2->Update();
	titleBackGround2_2->Update();
	titleBackGround2_3->Update();
	// 1

	titleBackGround1->Update();
	titleBackGround1_2->Update();
	titleBackGround1_3->Update();
	titleBackGround1_4->Update();
	soil->Update();

	DrawImgui();
}


void BackGround::Draw()
{
	skyBack->Draw();
	titleBackGround3->Draw();
	titleBackGround3_2->Draw();
	titleBackGround2->Draw();
	titleBackGround2_2->Draw();
	titleBackGround2_3->Draw();
	titleBackGround1->Draw();
	titleBackGround1_2->Draw();
	titleBackGround1_3->Draw();
	titleBackGround1_4->Draw();
	soil->Draw();
}

void BackGround::DrawImgui()
{
	// ImGuiで各背景の位置調整

#ifdef USE_IMGUI

	// 設定として保存
	ImGui::Begin("BackGround Transform");
	ImGui::DragFloat3("titleBackGround3 Translate", &titleBackGround3Transform.translate.x, 0.1f);
	titleBackGround3->SetTransform(titleBackGround3Transform);
	ImGui::DragFloat3("titleBackGround3_2 Translate", &titleBackGround3_2Transform.translate.x, 0.1f);
	titleBackGround3_2->SetTransform(titleBackGround3_2Transform);
	ImGui::DragFloat3("titleBackGround2 Translate", &titleBackGround2Transform.translate.x, 0.1f);
	titleBackGround2->SetTransform(titleBackGround2Transform);

	ImGui::End();
	


#endif


}
