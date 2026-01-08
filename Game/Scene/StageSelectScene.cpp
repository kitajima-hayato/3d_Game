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
	// プレイヤーオブジェクトの初期化
	playerModel = std::make_unique<Object3D>();
	playerModel->Initialize();
	playerModel->SetModel("Player.obj");
	playerModel->SetTranslate({ 0.0f, 0.0f, 0.0f });
	playerModel->SetScale({ 1.0f, 1.0f, 1.0f });

	// 背景
	backgroundModel = std::make_unique<Object3D>();
	backgroundModel->Initialize();
	backgroundModel->SetModel("back1.obj");
	backgroundModel->SetTranslate({ 0.0f, 0.0f, 50.0f });
	backgroundModel->SetScale({ 10.0f, 10.0f, 10.0f });
}

void StageSelectScene::Update()
{

	// プレイヤーモデルの更新
	playerModel->Update();
	// 背景モデルの更新
	backgroundModel->Update();
	// ImGuiの描画
	DrawImgui();
}

void StageSelectScene::Draw()
{
	// 背景モデルの描画
	backgroundModel->Draw();
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
	camera->SetTranslate(cameraTransform.translate);

	ImGui::Separator();
	// プレイヤー
	playerTransform = playerModel->GetTransform();
	ImGui::DragFloat3("PlayerPos", &playerTransform.translate.x, 0.1f);
	ImGui::DragFloat3("PlayerRot", &playerTransform.rotate.x, 0.1f);
	ImGui::DragFloat3("PlayerScale", &playerTransform.scale.x, 0.1f);
	playerModel->SetTransform(playerTransform);

	ImGui::Separator();
	// 背景
	backgroundTransform = backgroundModel->GetTransform();
	ImGui::DragFloat3("BackgroundPos", &backgroundTransform.translate.x, 0.1f);
	ImGui::DragFloat3("BackgroundRot", &backgroundTransform.rotate.x, 0.1f);
	ImGui::DragFloat3("BackgroundScale", &backgroundTransform.scale.x, 0.1f);
	backgroundModel->SetTransform(backgroundTransform);


	

	ImGui::End();
#endif
}
