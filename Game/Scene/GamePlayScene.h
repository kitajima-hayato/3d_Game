#pragma once
#include "Audio.h"
#include "SpriteCommon.h"
#include "InsideScene/BaseScene.h"
#include "engine/math/MyMath.h"
class Map;
class Camera;
class GamePlayScene :public BaseScene
{
public:
	GamePlayScene();
	~GamePlayScene();
	// 初期化
	void Initialize(DirectXCommon* dxCommon)override;
	// 更新
	void Update()override;
	// 描画
	void Draw()override;
	// 終了処理
	void Finalize()override;

private:
	

	// 
	std::unique_ptr<Map> map;

	// オーディオ
	// サウンドデータ
	SoundData soundData;
	IXAudio2* xaudio2_;


};

