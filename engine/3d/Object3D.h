#pragma once
#include "Math.h"
#include <string>
#include <d3d12.h>
#include <vector>
#include <wrl.h>
#include "TextureManager.h"
#include "Object3DCommon.h"
#include "Model.h"
#include "Game/Camera/Camera.h"

struct RingVertex {
	Vector4 position;
	Vector3 normal;
	Vector2 uv;
};

// 3Dオブジェクト
class Object3D
{

public:	// メンバ関数
	// 初期化
	void Initialize();
	// 更新
	void Update();
	// 描画
	void Draw();
	

/// <summary>
///  レベルローダーモデル生成
/// </summary>
	void Create(Model* model);

public:	// Getter/Setter
	void SetCamera(Camera* camera) { this->camera = camera; }

	void SetModel(Model* model) { this->model = model; }
	void SetModel(const std::string& filePath);
	void SetScale(const Vector3& scale) { transform.scale = scale; }
	void SetRotate(const Vector3& rotate) { transform.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transform.translate = translate; }
	void SetTransform(const Transform& transform) { this->transform = transform; }

	const Vector3 GetScale()const { return transform.scale; }
	const Vector3 GetRotate()const { return transform.rotate; }
	const Vector3 GetTranslate()const { return transform.translate; }
	const Transform& GetTransform()const { return transform; }
private: // メンバ関数
	// 座標変換行列リソースの作成
	void CreateTransformationMatrixData();
	// 平行光源リソースの作成
	void CreateDirectionalLightResource();
	// カメラリソースの作成
	void CreateCameraResource();

	
private:// メンバ変数
	// カメラ
	Camera* camera = nullptr;
	// モデルデータ
	Model* model = nullptr;
	// バッファリソース / 座標変換行列リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResource;
	// バッファリソース内のデータを指すポインタ
	TransformationMatrix* wvpData = nullptr;
	// バッファリソース / 平行光源リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
	// バッファリソース内のデータを指すポインタ
	DirectionalLight* directionalLightData = nullptr;

	// Transform
	Transform transform;


	Microsoft::WRL::ComPtr<ID3D12Resource>vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12Resource>indexBuffer;
	D3D12_VERTEX_BUFFER_VIEW vbView{};
	D3D12_INDEX_BUFFER_VIEW ibView{};
	uint32_t indexCount = 0;

	// カメラリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResource;
	CameraForGPU* cameraForGpuData = nullptr;
};

