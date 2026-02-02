#pragma once
#include "MyMath.h"
#include <d3d12.h>
#include <wrl.h>
#include "MakeMatrix.h"
class SpriteCommon;
/// <summary>
/// スプライトクラス
/// </summary>
class Sprite
{
public:	// メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(std::string_view textureFilePath);
	/// <summary>
	/// 更新
	/// </summary>
	void Update();
	/// <summary>
	/// 描画
	/// </summary>
	void Draw();

public:	// Getter/Setter
	/// <summary>
	/// 座標取得 / 設定
	/// </summary>
	/// <returns></returns>
	const Vector2& GetPosition()const { return position; }
	void SetPosition(const Vector2& position) { this->position = position; }
	/// <summary>
	/// 回転取得 / 設定
	/// </summary>
	float GetRotation()const { return rotation; }
	void SetRotation(float rotation) { this->rotation = rotation; }
	/// <summary>
	/// 色取得 / 設定
	/// </summary>
	const Vector4& GetColor()const { return materialData->color; }
	void SetColor(const Vector4& color) { materialData->color = color; }
	/// <summary>
	/// サイズ取得 / 設定
	/// </summary>
	const Vector2& GetSize()const { return size; }
	void SetSize(const Vector2& size) { this->size = size; }

	/// <summary>
	/// アンカーポイント取得 / 設定
	/// </summary>
	const Vector2& GetAnchorPoint()const { return anchorPoint; }
	void SetAnchorPoint(const Vector2& anchorPoint) { this->anchorPoint = anchorPoint; }

	/// <summary>
	/// FlipX取得 / 設定
	/// </summary>
	bool GetFlipX()const { return isFlipX; }
	void SetFlipX(bool isFlipX) { this->isFlipX = isFlipX; }
	/// <summary>
	/// FlipY取得 / 設定
	/// </summary>
	bool GetFlipY()const { return isFlipY; }
	void SetFlipY(bool isFlipY) { this->isFlipY = isFlipY; }
	/// <summary>
	/// TextureLeftTop取得 / 設定
	/// </summary>
	/// <returns></returns>
	const Vector2& GetTextureLeftTop()const { return textureLeftTop; }
	void SetTextureLeftTop(const Vector2& textureLeftTop) { this->textureLeftTop = textureLeftTop; }
	/// <summary>
	/// TextureSize取得 / 設定
	/// </summary>
	const Vector2& GetTextureSize()const { return textureSize; }
	void SetTextureSize(const Vector2& textureSize) { this->textureSize = textureSize; }


private:	// メンバ関数
	/// <summary>
	/// 頂点リソースデータの作成
	/// </summary>
	void CreateVertexResourceData();
	/// <summary>
	/// インデックスリソースデータの作成
	/// </summary>
	void CreateMaterialResource();
	/// <summary>
	/// 変換行列リソースデータの作成
	/// </summary>
	void CreateTransformationMatrixData();
	/// <summary>
	/// テクスチャサイズ調整
	/// </summary>
	void AdjustTextureSize();

	//void DrawSetting();
private:	// メンバ変数
	std::string textureFilePath_;
	
	// バッファリソース
	Microsoft::WRL::ComPtr <ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr <ID3D12Resource> indexResource;
	// バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;
	// バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	// バッファリソース
	Microsoft::WRL::ComPtr <ID3D12Resource> materialResource = nullptr;
	// バッファリソース内のデータを示すポインタ
	Material* materialData = nullptr;

	// バッファリソース
	Microsoft::WRL::ComPtr <ID3D12Resource>transformationMatrixResource;
	// バッファリソース内のデータを示すポインタ
	TransformationMatrix* transformationMatrix = nullptr;

	// 座標
	Vector2 position = { 0.0f,0.0f };
	float rotation = 0.0f;
	// 色はmaterialDataに格納されているのでそちらを参照
	Vector2 size = { 640.0f,360.0f };

	// テクスチャ番号
	uint32_t textureIndex = 0;

	// テクスチャトランスフォーム
	Transform transform;

	// アンカーポイント
	Vector2 anchorPoint = { 0.0f,0.0f };
	// 左右上下の反転フラグ
	// 左右フリップ
	bool isFlipX = false;
	// 上下フリップ
	bool isFlipY = false;
	// テクスチャ左上座標
	Vector2 textureLeftTop = { 0.0f,0.0f };
	// テクスチャ切り出しサイズ
	Vector2 textureSize = { 100.0f,100.0f };
};

