#include "StageClearScene.h"
#include "Game/Camera/Camera.h"
#include "InsideScene/Framework.h"
#include "Input.h"
#include "ImGuiManager.h"
StageClearScene::StageClearScene()
{
}

StageClearScene::~StageClearScene()
{
}

void StageClearScene::Initialize(DirectXCommon* dxCommon)
{
	// カメラの取得と設定
	camera = Framework::GetMainCamera();
	cameraTransform.translate = { 0.0f,0.0f,-10.0f };
	cameraTransform.rotate = { 0.0f,0.0f,0.0f };
	camera->SetTranslate(cameraTransform.translate);
	camera->SetRotate(cameraTransform.rotate);

	// @todo: お祝い感のあるパーティクルを降らせる


}

void StageClearScene::Update()
{
	// カメラの更新
	camera->Update();

	// タイトルへ戻る (1)
	if (Input::GetInstance()->TriggerKey(DIK_1)) {
		SceneManager::GetInstance()->ChangeScene("TITLE");
	}
	// ステージセレクトへ戻る (2)
	else if (Input::GetInstance()->TriggerKey(DIK_2)) {
		SceneManager::GetInstance()->ChangeScene("STAGESELECT");
	}
	// ゲームプレイへ戻る (3)
	else if (Input::GetInstance()->TriggerKey(DIK_3)) {
		SceneManager::GetInstance()->ChangeScene("GAMEPLAY");
	}
	// ImGuiの描画
	DrawImgui();
}

void StageClearScene::Draw()
{

}

void StageClearScene::Finalize()
{

}

void StageClearScene::DrawImgui()
{
	ImGui::Begin("StageClearScene");
	// シーン遷移の説明
	ImGui::Text("Press 1 to go to Title Scene");
	ImGui::Text("Press 2 to go to Stage Select Scene");
	ImGui::Text("Press 3 to go to Game Play Scene");

	// カメラ位置の調整
	ImGui::DragFloat3("CameraPos", &cameraTransform.translate.x, 0.1f);
	ImGui::DragFloat3("CameraRot", &cameraTransform.rotate.x, 0.01f);
	camera->SetTranslate(cameraTransform.translate);
	camera->SetRotate(cameraTransform.rotate);

	ImGui::End();
}
