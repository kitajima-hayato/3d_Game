#pragma once
#include "MyMath.h"
#include "MakeMatrix.h"
#include "WinAPI.h"

/// カメラクラス
class Camera
{
public:	// メンバ関数
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Camera();
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();
		
public:	
	// setter
	/// <summary>
	/// 回転・平行移動の設定
	/// </summary>
	void SetRotate(const Vector3& rotate) { transform.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { transform.translate = translate; }
	
	/// <summary>
	/// 視野角の設定
	/// </summary>
	/// <param name="fov"></param>
	void SetFovY(float fov) { this->fovY = fov; }

	/// <summary>
	/// アスペクト比の設定
	/// </summary>
	/// <param name="aspect"></param>
	void SetAspectRatio(float aspect) { this->aspectRatio = aspect; }

	/// <summary>
	/// ニアクリップの設定
	/// </summary>
	/// <param name="nearClip"></param>
	void SetNearClip(float nearClip) { this->nearClip = nearClip; }

	/// <summary>
	/// ファークリップの設定
	/// </summary>
	/// <param name="farClip"></param>
	void SetFarClip(float farClip) { this->farClip = farClip; }

	// getter
	/// <summary>
	/// 各種行列の取得
	/// </summary>
	const Matrix4x4& GetWorldMatrix()const { return worldMatrix; }
	
	/// <summary>
	/// ビュー行列の取得
	/// </summary>
	/// <returns></returns>
	const Matrix4x4& GetViewMatrix()const { return viewMatrix; }
	
	/// <summary>
	/// プロジェクション行列の取得
	/// </summary>
	const Matrix4x4& GetProjectionMatrix()const { return projectionMatrix; }
	
	/// <summary>
	/// ビュープロジェクション行列の取得
	/// </summary>
	/// <returns></returns>
	const Matrix4x4& GetViewProjectionMatrix()const { return viewProjectionMatrix; }
	
	/// <summary> 
	/// 拡縮・回転・平行移動の取得
	/// </summary>
	const Vector3& GetScale()const { return transform.scale; }
	const Vector3& GetRotate()const { return transform.rotate; }
	const Vector3& GetTranslate()const { return transform.translate; }


private:
	// カメラの変換情報
	Transform transform;
	// 水平方向視野角
	float fovY = 0.45f;
	// アスペクト比
	float aspectRatio = float(WinAPI::kClientWidth) / float(WinAPI::kClientHeight);
	// ニアクリップ
	float nearClip = 0.1f;
	// ファークリップ
	float farClip = 100.0f;
	// ワールド行列
	Matrix4x4 worldMatrix;
	Matrix4x4 viewMatrix;

	Matrix4x4 projectionMatrix;

	// ビュープロジェクション行列
	Matrix4x4 viewProjectionMatrix;


};

