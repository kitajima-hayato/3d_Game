#include "BackGround.h"

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

	titeleBackGround1_2 = std::make_unique<Object3D>();
	titeleBackGround1_2->Initialize();
	titeleBackGround1_2->SetModel("back1.obj");
	titeleBackGround1_2Transform = {
		{ 1.0f,20.0f,10.0f },
		{ 0.0f,1.5f,0.0f },
		{ 65,-2.0f,50.0f }
	};
	titeleBackGround1_2->SetTransform(titeleBackGround1_2Transform);
	titeleBackGround1_3 = std::make_unique<Object3D>();
	titeleBackGround1_3->Initialize();
	titeleBackGround1_3->SetModel("back1.obj");
	titeleBackGround1_3Transform = {
		{ 1.0f,20.0f,10.0f },
		{ 0.0f,-1.5f,0.0f },
		{ 120,-2.0f,50.0f }
	};
	titeleBackGround1_3->SetTransform(titeleBackGround1_3Transform);

	titeleBackGround1_4 = std::make_unique<Object3D>();
	titeleBackGround1_4->Initialize();
	titeleBackGround1_4->SetModel("back1.obj");
	titeleBackGround1_4Transform = {
		{ 1.0f,20.0f,10.0f },
		{ 0.0f,1.5f,0.0f },
		{ 178,-2.0f,50.0f }
	};
	titeleBackGround1_4->SetTransform(titeleBackGround1_4Transform);

	skyBack = std::make_unique<Object3D>();
	skyBack->Initialize();
	skyBack->SetModel("BackSky.obj");
	skyBackTransform = {
		{ 1.0f,60.0f,300.0f },
		{ 0.0f,-1.5f,0.0f },
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
	// 3
	/*titleBackGround3Transform.translate.x -= 0.03f;
	titleBackGround3_2Transform.translate.x -= 0.03f;
	titleBackGround3->SetTransform(titleBackGround3Transform);
	titleBackGround3_2->SetTransform(titleBackGround3_2Transform);*/

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
	/*titleBackGround1Transform.translate.x -= 0.1f;
	titeleBackGround1_2Transform.translate.x -= 0.1f;
	titeleBackGround1_3Transform.translate.x -= 0.1f;
	titeleBackGround1_4Transform.translate.x -= 0.1f;
	titleBackGround1->SetTransform(titleBackGround1Transform);
	titeleBackGround1_2->SetTransform(titeleBackGround1_2Transform);
	titeleBackGround1_3->SetTransform(titeleBackGround1_3Transform);
	titeleBackGround1_4->SetTransform(titeleBackGround1_4Transform);*/
	titleBackGround1->Update();
	titeleBackGround1_2->Update();
	titeleBackGround1_3->Update();
	titeleBackGround1_4->Update();
	soil->Update();
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
	titeleBackGround1_2->Draw();
	titeleBackGround1_3->Draw();
	titeleBackGround1_4->Draw();
	soil->Draw();
}