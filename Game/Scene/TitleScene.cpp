#include <thread>
#include "TitleScene.h"
#include "Input.h"
#include "Object3D.h"
#include "engine/3d/ModelManager.h"
#include "Game/Camera/camera.h"
TitleScene::TitleScene()
{

}

TitleScene::~TitleScene()
{
}

void TitleScene::Initialize(DirectXCommon* dxCommon)
{
	// スプライトの初期化
	SpriteCommon::GetInstance()->Initialize(dxCommon);


	//// パーティクルグループを作成
	ParticleManager::GetInstance()->CreateParticleGroup("Particle", "resources/monsterball.png");

	// マルチスレッドでの読み込み
	LoadAudio();
	LoadSprite();


	ParticleManager::GetInstance()->CreateParticleGroup("neo", "resources/back1.png");
	particleEmitter2 = make_unique<ParticleEmitter>();
	particleEmitter2->SetTransform({ { 0.0f,0.0f,0.0f },{ 0.0f,0.0f,0.0f },{ 5.0f,0.0f,20.0f } });
	particleEmitter2->SetParticleName("neo");

	object3D = make_unique<Object3D>();
	object3D->Initialize();

	object3D->SetModel("plane.obj");
	object3D->SetTranslate(Vector3(-4.0f, 0.0f, 10.0f));

	object3D->SetScale(Vector3(0.2f, 0.2f, 0.2f));
	speed = object3D->GetTranslate();




	playerObject = std::make_unique<Object3D>();
	playerObject->Initialize();
	playerObject->SetModel("Player.obj");
	playerTransform = {
		{ 1.0f,1.0f,1.0f },
		{ 0.0f,0.0f,0.0f },
		{ 1.0f,-7.0f,20.0f }
	};
	playerObject->SetTransform(playerTransform);


	titleLogo = std::make_unique<Object3D>();
	titleLogo->Initialize();
	titleLogo->SetModel("title.obj");
	titleLogoTransform = {
		{ 1.0f,1.0f,1.0f },
		{ 1.6f,3.2f,0.0f },
		{ 7.4f,-3.1f,10.1f }
	};

	titleLogo->SetTransform(titleLogoTransform);




	//sceneTransition = std::make_unique<SceneTransition>();
	//sceneTransition->Initialize();

	background = std::make_unique<BackGround>();
	background->Initialize();


	

	
	// パーティクルエミッターの初期化
	particleEmitter = make_unique<ParticleEmitter>();
	particleEmitter->SetParticleName("Particle");


#pragma region 演出
	/*EffectManager::GetInstance()->CreateEffectGroup("Ring", "resources/monsterball.png");
	effectEmitter = make_unique<EffectEmitter>();

	effectTransform = effectEmitter->GetTransform();
	effectTransform = {
		{0.5f,0.5f,0.5f},
		{0.0f,0.0f,0.0f},
		{-1.0f,0.0f,10.0f},
	};
	effectEmitter->SetTransform(effectTransform);
	effectEmitter->SetEffectName("Ring");

	EffectManager::GetInstance()->CreateEffectGroup("Cylinder", "resources/gradationLine.png");
	cylinder = make_unique<EffectEmitter>();
	cylinderTransform = cylinder->GetTransform();

	cylinderTransform = {
		{0.5f,0.5f,0.5f},
		{0.0f,0.0f,0.0f},
		{0.0f,0.0f,10.0f},
	};
	cylinder->SetTransform(cylinderTransform);
	cylinder->SetEffectName("Cylinder");*/
#pragma endregion



}

void TitleScene::Update()
{

	//sprite_->Update();

	background->Update();

	object3D->Update();
	titleLogo->Update();

	// 段々薄く

	




	//sceneTransition->Update();



	/*particleEmitter->SetTransform({
	emitterScale,
	emitterRotate,
	emitterTranslate
		});*/
#ifdef USE_IMGUI
	DrawImgui();
#endif


	if (Input::GetInstance()->TriggerKey(DIK_SPACE)) {
		// 
		isStart = !isStart;
		effectEmitter->EmitCylinder();
		cylinder->EmitRing();
	}

	if (isStart) {
		// object3dをひだりから右に
		speed.x += 0.07f;
		object3D->SetTranslate(speed);

	}
	// エミッタの位置を更新（プレイヤーに追従させるなど）
	Transform transform;
	transform.translate = { 1.0f, -5.0f, 15.0f };
	particleEmitter->SetTransform(transform);
	particleEmitter->Update();
	particleEmitter2->Update();



	//levelData->Update();
	// プレイヤーを回転
	playerTransform.rotate.z -= 0.05f; // 回転速度は調整可能
	playerObject->SetTransform(playerTransform);
	playerObject->Update();


	// Rainbow回転（ここを追加）
	//rainbowTransform.rotate.y += 0.005f; // 回転速度は調整可能
	//Rainbow->SetTransform(rainbowTransform);

	//Rainbow->Update();


	// ENTERキーが押されたら
	if (Input::GetInstance()->TriggerKey(DIK_RETURN))
	{
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
}

void TitleScene::Draw()
{

#pragma region 3Dオブジェクトの描画準備

	//sprite_->Draw();
#pragma endregion

#pragma region 3Dオブジェクトの描画

	background->Draw();

	//object3D->Draw();
	//levelData->Draw();
	playerObject->Draw();
	//Rainbow->Draw();

	

	// パーティクルの描画
	//ParticleManager::GetInstance()->Draw();
	ParticleManager::GetInstance()->Draw();
	//particleEmitter->Emit();
	particleEmitter2->Emit();
	// エフェクトの描画


	//EffectManager::GetInstance()->DrawRing();
	//EffectManager::GetInstance()->DrawCylinder();
	titleLogo->Draw();


	//sceneTransition->Draw();


#pragma endregion

}

void TitleScene::Finalize()
{


	// オーディオの終了処理
	//Audio::GetInstance()->SoundUnload(&soundData);
	/*ParticleManager::GetInstance()->DeleteParticleGroup("Particle");
	EffectManager::GetInstance()->DeleteEffectGroup("Cylinder");
	EffectManager::GetInstance()->DeleteEffectGroup("Ring");*/

	// スプライトの終了処理
	SpriteCommon::GetInstance()->Deletenstance();
}

void TitleScene::LoadAudio()
{
	// オーディオの初期化
	//Audio::GetInstance()->Initialize();
	//soundData = Audio::GetInstance()->LoadWave("resources/fanfare.wav");
	//xaudio2_ = Audio::GetInstance()->GetXAudio2();
	//Audio::GetInstance()->SoundPlayWave(xaudio2_, soundData);

}

void TitleScene::LoadSprite() 
{
	sprite_ = make_unique<Sprite>();
	sprite_->Initialize("resources/gradationLine.png");
	sprite_->SetPosition({ 0.0f,0.0f });
	sprite_->SetRotation(0.0f);
}

void TitleScene::DrawImgui() {
#ifdef USE_IMGUI

	ImGui::Begin("playerObject");

	Transform pTransform = playerObject->GetTransform();
	ImGui::DragFloat3("Scale", &pTransform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotate", &pTransform.rotate.x, 0.1f);
	ImGui::DragFloat3("Translate", &pTransform.translate.x, 0.1f);

	playerObject->SetTransform(pTransform);

	ImGui::End();

	/*ImGui::Begin("BackGround");

	ImGui::Text("Soil");
	ImGui::DragFloat3("Scales", &soilTransform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotates", &soilTransform.rotate.x, 0.1f);
	ImGui::DragFloat3("Translates", &soilTransform.translate.x, 0.1f);
	soil->SetTransform(soilTransform);
	ImGui::Separator();

	ImGui::Text("Player");
	ImGui::DragFloat3("Scalep", &playerTransform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotatep", &playerTransform.rotate.x, 0.1f);
	ImGui::DragFloat3("Translatep", &playerTransform.translate.x, 0.1f);
	playerObject->SetTransform(playerTransform);
	ImGui::Separator();
	// 背景
	ImGui::Text("BackGround3");
	ImGui::DragFloat3("Scaleb", &titleBackGround3Transform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotateb", &titleBackGround3Transform.rotate.x, 0.1f);
	ImGui::DragFloat3("Translateb", &titleBackGround3Transform.translate.x, 0.1f);
	titleBackGround3->SetTransform(titleBackGround3Transform);

	ImGui::Separator();

	ImGui::Text("BackGround3_2");
	ImGui::DragFloat3("Scalec3_2", &titleBackGround3_2Transform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotatec3_2", &titleBackGround3_2Transform.rotate.x, 0.1f);
	ImGui::DragFloat3("Translatec3_2", &titleBackGround3_2Transform.translate.x, 0.1f);
	titleBackGround3_2->SetTransform(titleBackGround3_2Transform);
	ImGui::Separator();

	ImGui::Text("BackGround2");
	ImGui::DragFloat3("Scalec", &titleBackGround2Transform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotatec", &titleBackGround2Transform.rotate.x, 0.1f);
	ImGui::DragFloat3("Translatec", &titleBackGround2Transform.translate.x, 0.1f);
	titleBackGround2->SetTransform(titleBackGround2Transform);

	ImGui::Separator();

	ImGui::Text("BackGround2_2");
	ImGui::DragFloat3("Scaled2_2", &titleBackGround2_2Transform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotated2_2", &titleBackGround2_2Transform.rotate.x, 0.1f);
	ImGui::DragFloat3("Translated2_2", &titleBackGround2_2Transform.translate.x, 0.1f);
	titleBackGround2_2->SetTransform(titleBackGround2_2Transform);
	ImGui::Separator();
	ImGui::Text("BackGround2_3");
	ImGui::DragFloat3("Scaled2_3", &titleBackGround2_3Transform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotated2_3", &titleBackGround2_3Transform.rotate.x, 0.1f);
	ImGui::DragFloat3("Translated2_3", &titleBackGround2_3Transform.translate.x, 0.1f);
	titleBackGround2_3->SetTransform(titleBackGround2_3Transform);
	ImGui::Separator();

	ImGui::Text("BackGround1");
	ImGui::DragFloat3("Scaled", &titleBackGround1Transform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotated", &titleBackGround1Transform.rotate.x, 0.1f);
	ImGui::DragFloat3("Translated", &titleBackGround1Transform.translate.x, 0.1f);
	titleBackGround1->SetTransform(titleBackGround1Transform);

	ImGui::Separator();

	ImGui::Text("BackGround1_2");
	ImGui::DragFloat3("Scale1_2", &titeleBackGround1_2Transform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotate1_2", &titeleBackGround1_2Transform.rotate.x, 0.1f);
	ImGui::DragFloat3("Translate1_2", &titeleBackGround1_2Transform.translate.x, 0.1f);
	titeleBackGround1_2->SetTransform(titeleBackGround1_2Transform);
	ImGui::Separator();
	ImGui::Text("BackGround1_3");
	ImGui::DragFloat3("Scale1_3", &titeleBackGround1_3Transform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotate1_3", &titeleBackGround1_3Transform.rotate.x, 0.1f);
	ImGui::DragFloat3("Translate1_3", &titeleBackGround1_3Transform.translate.x, 0.1f);
	titeleBackGround1_3->SetTransform(titeleBackGround1_3Transform);
	ImGui::Separator();
	ImGui::Text("BackGround1_4");
	ImGui::DragFloat3("Scale1_4", &titeleBackGround1_4Transform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotate1_4", &titeleBackGround1_4Transform.rotate.x, 0.1f);
	ImGui::DragFloat3("Translate1_4", &titeleBackGround1_4Transform.translate.x, 0.1f);
	titeleBackGround1_4->SetTransform(titeleBackGround1_4Transform);
	ImGui::Separator();
	ImGui::Text("SkyBack");
	ImGui::DragFloat3("Scalee", &skyBackTransform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotatee", &skyBackTransform.rotate.x, 0.1f);
	ImGui::DragFloat3("Translatee", &skyBackTransform.translate.x, 0.1f);
	skyBack->SetTransform(skyBackTransform);


	ImGui::End();*/


	//if (levelData->HasPlayerSpawn()) {
	//	const auto& playerSpawn = levelData->getPlayerSpawns()[0];

	//	// プレイヤーの位置と回転を Object3D にセット
	//	playerObject->SetTranslate(playerSpawn.transform.translate);
	//	playerObject->SetRotate(playerSpawn.transform.rotate);

	//	// ImGui ウィンドウで表示
	//	ImGui::Begin("PlayerSpawn Info");

	//	const Vector3& pos = playerSpawn.transform.translate;
	//	const Vector3& rot = playerSpawn.transform.rotate;

	//	ImGui::Text("Translate: X = %.2f, Y = %.2f, Z = %.2f", pos.x, pos.y, pos.z);
	//	ImGui::Text("Rotate:    X = %.2f, Y = %.2f, Z = %.2f", rot.x, rot.y, rot.z);

	//	ImGui::End();
	//}




	//ImGui::Begin("Particle");

	//ImGui::Text("Transform");
	//ImGui::DragFloat3("Scalea", &emitterScale.x, 0.1f);
	//ImGui::DragFloat3("Rotatea", &emitterRotate.x, 0.1f);
	//ImGui::DragFloat3("Translatea", &emitterTranslate.x, 1.0f); // ここでtest.xを操作可能に

	//ImGui::End();

	/*ImGui::Begin("effect");

	ImGui::Text("Transform");
	ImGui::DragFloat3("Scale", &effectTransform.scale.x, 0.1f);
	ImGui::DragFloat3("Rotate", &effectTransform.rotate.x, 0.1f);
	ImGui::DragFloat3("Translate", &effectTransform.translate.x, 1.0f);
	effectEmitter->SetTransform(effectTransform);

	ImGui::Text("asdf");
	ImGui::DragFloat3("CylinderScale", &cylinderTransform.scale.x, 0.1f);
	ImGui::DragFloat3("CylinderRotate", &cylinderTransform.rotate.x, 0.1f);
	ImGui::DragFloat3("CylinderTranslate", &cylinderTransform.translate.x, 1.0f);
	cylinder->SetTransform(cylinderTransform);
	ImGui::End();*/


#endif // _DEBUG

}

