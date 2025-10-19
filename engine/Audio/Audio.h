#pragma once
#include <xaudio2.h>
#pragma comment(lib, "xaudio2.lib")
#include <fstream>
#include <wrl.h>

/// <summary>
/// サウンド管理クラス
/// </summary>
/// <remarks>
/// XAudio2を用いてサウンドを管理するクラス
/// </remarks>


struct ChunkHeader
{
	char chunkID[4];
	uint32_t chunkSize;
};
struct RIFFHeader
{
	ChunkHeader chunk;
	char type[4];
};
struct FormartChunk {
	ChunkHeader chunk;
	WAVEFORMATEX format;
};
struct SoundData {
	// 波形フォーマット
	WAVEFORMATEX wfex;
	// バッファの先頭アドレス
	BYTE* pBuffer;
	// バッファのサイズ
	unsigned int bufferSize;
};
class Audio
{
public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	static Audio* GetInstance();
	/// <summary>
	/// シングルトンインスタンスの削除
	/// </summary>
	static void DeleteInstance();
	
private:
	/// <summary>
	/// シングルトンインスタンス
	/// </summary>
	static Audio* instance;
	/// <summary>
	/// コンストラクタ
	/// </summary>
	Audio() = default;
	/// <summary>
	/// デストラクタ
	/// </summary>
	~Audio() = default;
	/// <summary>
	/// コピーコンストラクタ・代入演算子の禁止
	/// </summary>
	Audio(Audio&) = delete;
	/// <summary>
	/// コピーコンストラクタ・代入演算子の禁止
	/// </summary>
	Audio& operator=(Audio&) = delete;


public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	/// <summary>
	/// サウンドの読み込み
	/// </summary>
	SoundData LoadWave(const char* filename);
	/// <summary>
	/// サウンドの解放
	///　</summary>
	void SoundUnload(SoundData* soundData);
	/// <summary>
	/// サウンドの再生
	///　</summary>
	void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData);

public:
	/// <summary>
	/// XAudio2の取得
	/// </summary>
	IXAudio2* GetXAudio2() const { return xaudio2_.Get(); }

private:
	/// XAudio2
	Microsoft::WRL::ComPtr<IXAudio2> xaudio2_;
	// ↓の型にはRelease()が無いので、ComPtrを使わない
	IXAudio2MasteringVoice* masterVoice;
};
