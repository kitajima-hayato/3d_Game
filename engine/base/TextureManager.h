#pragma once
#include <string>
#include <wrl/client.h>
#include <d3d12.h>
#include "DirectXCommon.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "SrvManager.h"
#include <unordered_map>
#include <cstdint>

/// <summary>
/// テクスチャ管理クラス
/// </summary>
/// <remarks>
/// テクスチャの読み込み、解放を行う
/// シングルトンクラス
/// </remarks>
class srvManager;
class TextureManager
{
	// SRVインデックスの開始番号
	static uint32_t kSRVIndexTop;
public:		// メンバ関数
	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="dxCommon"></param>
	/// <param name="srvManager"></param>
	void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager);
	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();
	/// <summary>
	/// テクスチャファイルパスの読み込み
	/// <param name ="filePath"テクスチャファイルのパス>
	/// </summary>
	void LoadTexture(const std::string& filePath);

public:	// Getter,Setter
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	static TextureManager* GetInstance();
	/// <summary>
	/// シングルトンインスタンスの解放
	/// </summary>
	static void DeleteInstance();

	/// <summary>
	/// テクスチャ情報の取得
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	const DirectX::TexMetadata& GetMetadata(const std::string& filePath);
	/// <summary>
	/// テクスチャ番号の取得
	/// </summary>
	/// <param name="filePath"></param>
	/// <returns></returns>
	uint32_t GetSrvIndex(const std::string& filePath);
	/// <summary>
	/// テクスチャ番号からGPUハンドルを取得
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePath);

	Microsoft::WRL::ComPtr<ID3D12Resource> GetTextureResource(const std::string& filePath);

	D3D12_GPU_DESCRIPTOR_HANDLE GetGpuDescriptorHandle(const std::string& filePath);
	

private: // メンバ関数/構造体


	//テクスチャ１枚分の情報
	struct TextureData
	{
		//std::string filePath;	// ファイル名
		DirectX::TexMetadata metadata;						// テクスチャの情報(幅、高さ)
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;	// テクスチャリソース
		uint32_t srvIndex;									// SRVのインデックス
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;			// シェーダリソースビューのCPUハンドル
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;			// 描画コマンドに必要なGPUハンドル
	};

private:	// シングルトン 
	static TextureManager* instance;
	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = delete;

private:	// メンバ変数
	DirectXCommon* dxCommon = nullptr;

	// テクスチャデータ
	std::unordered_map<std::string, TextureData> textureDatas;	// 
	// SRVマネージャー
	SrvManager* srvManager = nullptr;

};

