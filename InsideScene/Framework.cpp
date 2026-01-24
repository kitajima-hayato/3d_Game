#include "Framework.h"
void Framework::Initialize()
{
	//WindowsAPIの初期化
	winAPI = make_unique<WinAPI>();
	winAPI->Initialize();
	// dxCommonの初期化
	dxCommon = make_unique<DirectXCommon>();
	dxCommon->Initialize(winAPI.get());

	// 入力処理のクラスポインタ
	
	Input::GetInstance()->Initialize(winAPI.get());

	// SRVマネージャーの初期化
	srvManager = make_unique<SrvManager>();
	srvManager->Initialize(dxCommon.get());
	// テクスチャマネージャーの初期化
	TextureManager::GetInstance()->Initialize(dxCommon.get(), srvManager.get());


#ifdef USE_IMGUI
	// ImGuiの初期化
	imGui = make_unique<ImGuiManager>();
	imGui->Initialize(winAPI.get(), dxCommon.get());
#endif

	// 3Dモデルマネージャの初期化
	ModelManager::GetInstance()->Initialize(dxCommon.get());
	// 3Dオブジェクト共通部の初期化
	Object3DCommon::GetInstance()->Initialize(dxCommon.get());

	// モデル共通部の初期化
	modelCommon = make_unique<ModelCommon>();
	modelCommon->Initialize(dxCommon.get());

	// カメラ
	camera = make_unique<Camera>();
	camera->SetRotate({ 0.0f, 0.0f, 0.0f });
	camera->SetTranslate({ 0.0f, 0.0f, -5.0f });

	// 全シーンがアクセス可能なカメラに設定
	mainCamera_ = camera.get();

	Object3DCommon::GetInstance()->SetDefaultCamera(camera.get());

	// パーティクル
	ParticleManager::GetInstance()->Initialize(dxCommon.get(), srvManager.get(), camera.get());
	
	

	/// マップに合わせる
	/*cameraTransform.translate = { 7.5f,-4.0f,0.0f };
	camera->SetTranslate(cameraTransform.translate);*/

}

void Framework::Update()
{
#pragma region WindowsAPIのメッセージ処理
	//Windowのメッセージ処理
	if (winAPI->ProcessMessage()) {
		//ゲームループを抜ける
		isEndRequest_ = true;
		return;
	}
#pragma endregion

	Input::GetInstance()->Update();
	// カメラの更新
	camera->Update();
#ifdef USE_IMGUI
	// カメラの配置回転情報の変更・表示UI
	ImGui::Begin("Camera Settings");
	ImGui::DragFloat3("Translate", &cameraTransform.translate.x, 0.1f);
	ImGui::DragFloat3("Rotate", &cameraTransform.rotate.x, 0.1f);
	ImGui::End();

	// カメラに反映させる
	/*static bool useDebugCamera = false;
	ImGui::Checkbox("Use Debug Camera", &useDebugCamera);
	if (useDebugCamera) {*/
	/*camera->SetTranslate(cameraTransform.translate);
	camera->SetRotate(cameraTransform.rotate);*/
	//}
#endif 

	SceneManager::GetInstance()->Update(dxCommon.get());
	

	
	// パーティクルの更新
	ParticleManager::GetInstance()->Update();

	// ESCキーで終了
	if (Input::GetInstance()->TriggerKey(DIK_ESCAPE))
	{
		isEndRequest_ = true;
	}
	
}


void Framework::Draw()
{
	


}


void Framework::Finalize()
{
	

	SceneManager::GetInstance()->Finalize();
	SceneManager::Deletenstance();
	Audio::GetInstance()->DeleteInstance();
	Object3DCommon::GetInstance()->DeleteInstance();
	SpriteCommon::GetInstance()->Deletenstance();
	TextureManager::GetInstance()->DeleteInstance();
	ModelManager::GetInstance()->Finalize();
	Input::GetInstance()->DeleteInstance();
	winAPI->Finalize();
	mainCamera_ = nullptr;
	ParticleManager::GetInstance()->DeleteInstance();

}

void Framework::Run()
{
	// ゲームの初期化
	Initialize();
	// ゲームのメインループ
	while (true) {
		// ゲームの更新
		Update();
		if (IsEndRequest()) {
			break;
		}
		// ゲームの描画
		Draw();
	}
	// ゲームの終了処理
	Finalize();
}

