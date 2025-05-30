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
// 3Dオブジェクト共通部前方宣言
class Object3DCommon;
class Object3D
{

public:	// メンバ関数
	// 初期化
	void Initialize(Object3DCommon* obj3dCommon);
	// 更新
	void Update();
	// 描画
	void Draw();
#pragma region リングエフェクト関連関数群
	// リングの頂点作成
	void CreateRingMesh(uint32_t divide, float outerRadius, float innnerRadius);

	// サンプラーステートの設定
	static const D3D12_STATIC_SAMPLER_DESC staticSampler[];
#pragma endregion
public:	// Getter/Setter
	void SetCamera(Camera* camera) { this->camera = camera; }

	void SetModel(Model* model) { this->model = model; }
	void SetModel(const std::string& filePath);
	void SetScale(const Vector3& scale) { transform.scale = scale; }
	void SetRotate(const Vector3& rotate) { transform.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transform.translate = translate; }

	const Vector3 GetScale()const { return transform.scale; }
	const Vector3 GetRotate()const { return transform.rotate; }
	const Vector3 GetTranslate()const { return transform.translate; }
private: // メンバ関数
	// 座標変換行列リソースの作成
	void CreateTransformationMatrixData();
	// 平行光源リソースの作成
	void CreateDirectionalLightResource();

	
private:// メンバ変数
	// カメラ
	Camera* camera = nullptr;
	// 3Dオブジェクト共通部
	Object3DCommon* object3DCommon = nullptr;
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
};

