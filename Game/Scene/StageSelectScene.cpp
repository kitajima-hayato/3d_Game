#include "StageSelectScene.h"
#include "InsideScene/Framework.h"
#include "Game/Camera/Camera.h"
#include "engine/3d/Object3D.h"
#ifdef USE_IMGUI
#include "engine/bace/ImGuiManager.h"
#endif

StageSelectScene::StageSelectScene()
{
}

StageSelectScene::~StageSelectScene()
{
}

void StageSelectScene::Initialize(DirectXCommon* dxCommon)
{
	// カメラの取得
	camera = Framework::GetMainCamera();
	cameraTransform.translate = { 0.0f,20.0f,0.0f };
	cameraTransform.rotate = { 0.35f,0.0f,0.0f };
	camera->SetTranslate(cameraTransform.translate);
	camera->SetRotate(cameraTransform.rotate);
	// プレイヤーオブジェクトの初期化
	playerModel = std::make_unique<Object3D>();
	playerModel->Initialize();
	playerModel->SetModel("Player.obj");
	playerTransform = {
		// scale
		{1.5f,1.5f,1.5f},
		// rotate
		{0.0f,0.0f,0.0f},
		// translate
		{-20.0f,1.0f,50.0f},
	};

	// 背景オブジェクトの初期化
	stageSelectBase1 = std::make_unique<Object3D>();
	stageSelectBase1->Initialize();
	stageSelectBase1->SetModel("SelectBaseGround.obj");
	stageSelectBase1Transform = {
		// Scale
		{ 1.5f, 1.0f, 2.5f },
		// Rotate
		{ 0.0f, 0.0f, 0.0f },
		// Translate
		{ 0.0f, 0.0f, 50.0f }
	};
	stageSelectBase1->SetTransform(stageSelectBase1Transform);

	skyBack = std::make_unique<Object3D>();
	skyBack->Initialize();
	skyBack->SetModel("BackSky.obj");
	skyBackTransform = {
		// Scale
		{ 50.0f, 50.0f, 50.0f },
		// Rotate
		{ 0.0f, 0.0f, 0.0f },
		// Translate
		{ 0.0f, 0.0f, 50.0f }
	};
	skyBack->SetTransform(skyBackTransform);

}

void StageSelectScene::Update()
{
	// 空背景の更新
	skyBack->Update();
	// ステージセレクト土台の更新
	stageSelectBase1->Update();

	// プレイヤーモデルの更新
	playerModel->Update();

	// ImGuiの描画
	DrawImgui();
}

void StageSelectScene::Draw()
{
	// 空背景の描画
	skyBack->Draw();
	// ステージセレクト土台の描画
	stageSelectBase1->Draw();
	// プレイヤーモデルの描画
	playerModel->Draw();
}

void StageSelectScene::Finalize()
{
}

void StageSelectScene::DrawImgui()
{
#ifdef USE_IMGUI
	ImGui::Begin("StageSelectScene");
	// カメラ位置の調整
	ImGui::DragFloat3("CameraPos", &cameraTransform.translate.x, 0.1f);
	ImGui::DragFloat3("CameraRot", &cameraTransform.rotate.x, 0.01f);
	camera->SetTranslate(cameraTransform.translate);
	camera->SetRotate(cameraTransform.rotate);

	ImGui::Separator();

	// プレイヤー
	playerTransform = playerModel->GetTransform();
	ImGui::DragFloat3("PlayerPos", &playerTransform.translate.x, 0.1f);
	ImGui::DragFloat3("PlayerRot", &playerTransform.rotate.x, 0.1f);
	ImGui::DragFloat3("PlayerScale", &playerTransform.scale.x, 0.1f);
	playerModel->SetTransform(playerTransform);

	ImGui::Separator();
	// ステージセレクト土台１
	stageSelectBase1Transform = stageSelectBase1->GetTransform();
	ImGui::DragFloat3("StageSelectBase1Pos", &stageSelectBase1Transform.translate.x, 0.1f);
	ImGui::DragFloat3("StageSelectBase1Rot", &stageSelectBase1Transform.rotate.x, 0.1f);
	ImGui::DragFloat3("StageSelectBase1Scale", &stageSelectBase1Transform.scale.x, 0.1f);
	stageSelectBase1->SetTransform(stageSelectBase1Transform);

	ImGui::Separator();

	// 空背景
	skyBackTransform = skyBack->GetTransform();
	ImGui::DragFloat3("SkyBackPos", &skyBackTransform.translate.x, 0.1f);
	ImGui::DragFloat3("SkyBackRot", &skyBackTransform.rotate.x, 0.1f);
	ImGui::DragFloat3("SkyBackScale", &skyBackTransform.scale.x, 0.1f);
	skyBack->SetTransform(skyBackTransform);

	ImGui::End();
#endif
}
