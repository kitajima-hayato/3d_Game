#include "StageClearScene.h"
#include "Game/Camera/Camera.h"
#include "InsideScene/Framework.h"

void StageClearScene::Initialize(DirectXCommon* dxCommon)
{
	// カメラの取得と設定
	camera = Framework::GetMainCamera();
	camera->SetTranslate({ 0.0f,0.0f,-10.0f });
	camera->SetRotate({ 0.0f,0.0f,0.0f });


}

void StageClearScene::Update()
{

}

void StageClearScene::Draw()
{

}

void StageClearScene::Finalize()
{

}

void StageClearScene::DrawImgui()
{

}
