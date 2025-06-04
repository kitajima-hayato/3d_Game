#include "MyGame.h"
#include "InsideScene/SceneFactory.h"
void MyGame::Initialize()
{
	Framework::Initialize();
	// シーンファクトリーの生成
	sceneFactory_ = make_unique<SceneFactory>();
	SceneManager::GetInstance()->SetSceneFactory(sceneFactory_.get());
	// シーンmanagerに最初のシーンをセット
	SceneManager::GetInstance()->ChangeScene("TITLE");

	modelList = make_unique<ModelList>();
	modelList->LoadAllModel();
}

void MyGame::Update()
{
#ifdef _DEBUG // デバッグ時のみ有効ImGuiの処理
	imGui->Begin(); 
#endif 
	Framework::Update();
#ifdef _DEBUG // デバッグ時のみ有効ImGuiの処理
	// ImGuiの処理
	ImGui::Text("Hello, world %d", 123);
	imGui->End();
#endif
}

void MyGame::Draw()
{
	// DirectXの描画準備。全ての描画に共通のグラフィックスコマンドを積む
	dxCommon->PreDraw();
	srvManager->PreDraw();
	
	// 3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
	Object3DCommon::GetInstance()->DrawSettingCommon();

	// シーンマネージャーの描画	
	SceneManager::GetInstance()->Draw();
	// 3Dオブジェクトの描画
#ifdef _DEBUG
	// ImGuiの描画
	imGui->Draw();
#endif
	dxCommon->PostDraw();
}

void MyGame::Finalize()
{
	
#pragma region  解放処理
#ifdef _DEBUG
	// ImGuiの終了処理
	imGui->Finalize();
#endif
	// CloseHandle(fenceEvent);
	TextureManager::GetInstance()->Finalize();
	// モデルマネージャの終了処理
	ModelManager::GetInstance()->Finalize();
	winAPI->Finalize();

#pragma endregion

	Framework::Finalize();
}
