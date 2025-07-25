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


#ifdef _DEBUG
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
	Object3DCommon::GetInstance()->SetDefaultCamera(camera.get());

	// パーティクル
	ParticleManager::GetInstance()->Initialize(dxCommon.get(), srvManager.get(), camera.get());
	// エフェクト
	EffectManager::GetInstance()->Initialize(dxCommon.get(), srvManager.get(), camera.get());

	

}

void Framework::Update()
{
#pragma region WindowsAPIのメッセージ処理
	//Windowのメッセージ処理
	if (winAPI->ProcessMessage()) {
		//ゲームループを抜ける
		isEndRequst = true;
		return;
	}
#pragma endregion

	SceneManager::GetInstance()->Update(dxCommon.get());
	Input::GetInstance()->Update();
	camera->Update();
	ParticleManager::GetInstance()->Update();
	EffectManager::GetInstance()->Update();

#ifdef _DEBUG

	ImGui::Begin("Camera Settings");
	ImGui::DragFloat3("Translate", &cameraTransform.translate.x, 0.01f);
	ImGui::DragFloat3("Rotate", &cameraTransform.rotate.x, 0.01f);
	ImGui::End();
	camera->SetTranslate(cameraTransform.translate);
	camera->SetRotate(cameraTransform.rotate);

#endif 


	// ESCキーで終了
	if (Input::GetInstance()->TriggerKey(DIK_ESCAPE))
	{
		isEndRequst = true;
	}

}


void Framework::Draw()
{
	


}


void Framework::Finalize()
{
	// パーティクルの終了処理 / newとは逆順で

	winAPI->Finalize();

	SceneManager::GetInstance()->Finalize();
	SceneManager::Deletenstance();
	Input::GetInstance()->DeleteInstance();
	TextureManager::GetInstance()->DeleteInstance();
	ModelManager::GetInstance()->Finalize();
	ParticleManager::GetInstance()->DeleteInstance();
	EffectManager::GetInstance()->DeleteInstance();

}

void Framework::Run()
{
	// ゲームの初期化
	Initialize();
	// ゲームのメインループ
	while (true) {
		// ゲームの更新
		Update();
		if (IsEndRequst()) {
			break;
		}
		// ゲームの描画
		Draw();
	}
	// ゲームの終了処理
	Finalize();
}

