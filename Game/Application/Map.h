#pragma once
#include <vector>
#include <memory>
#include "BlockType.h"
#include <MyMath.h>
#include "Game/Application/Block.h"
#include "Game/Application/CsvLoader.h"
/// マップクラス

/// マップチップデータ構造体
struct MapChipData {
	std::vector<std::vector<BlockType>> mapData;
};

/// インデックス構造体
struct IndexSet {
	uint32_t xIndex;
	uint32_t yIndex;
};

/// 矩形構造体
struct Rect {
	float left;
	float right;
	float bottom;
	float top;
};

class Map
{
public:
	// １ブロックの大きさ
	static inline const float kBlockWidth = 1.0f;
	static inline const float kBlockHeight = 1.0f;
	//マップの大きさ
	// 横
	static inline const uint32_t kMapWidth = 100;	
	// 高さ
	static inline const uint32_t kMapHeight = 20;	

	// 可変サイズのマップの大きさ
	uint32_t GetWidth() const { return mapChipData_.mapData.empty() ? 0u : (uint32_t)mapChipData_.mapData[0].size(); }

	uint32_t GetHeight() const { return (uint32_t)mapChipData_.mapData.size(); }

public:
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

	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// マップブロック生成
	/// </summary>
	void GenerareMapBlock();

	/// <summary>
	/// マップデータの読み込み
	/// </summary>
	void LoadMapData(const char* filePath);


public:	// Setter / Getter
	/// <summary>
	/// 座標からマップチップのインデックスを取得
	/// </summary>
	/// <param name="position">取得したいマップのインデックス座標</param>
	IndexSet GetMapChipIndexSetByPosition(const Vector3& position);

	/// <summary>
	/// インデックスからマップチップの種類を取得
	/// </summary>
	/// <param name="xIndex/yIndex">取得したいマップの座標インデックス</param>
	/// <returns>指定したインデックスのマップチップの種類</returns>
	BlockType GetMapChipTypeByIndex(uint32_t xIndex, uint32_t yIndex);
	

	/// <summary>
	/// インデックスから矩形情報を取得
	/// </summary>
	/// <param name="xIndex/yIndex">インデックス座標</param>
	/// <returns>指定インデックス座標のブロックの矩形情報</returns>
	Rect GetRectByIndex(uint32_t xIndex, uint32_t yIndex);

	/// <summary>
	/// インデックスからマップチップの中心座標を取得
	/// </summary>
	/// <param name="xIndex/yIndex">インデックス座標</param>
	/// <returns>指定インデックス座標のブロックの中心座標</returns>
	Vector3 GetMapChipPositionByIndex(uint32_t xIndex, uint32_t yIndex);
private:
	// マップチップデータ
	MapChipData mapChipData_;

	// 変更検知
	bool isMapDataChanged_ = false;
	MapChipData previousMapData_;

	// マップブロックの配列
	std::vector<std::vector<Block*>> blockArray_;

	// 可変サイズのマップの大きさ
	uint32_t width_ = 0;
	uint32_t height_ = 0;
};

