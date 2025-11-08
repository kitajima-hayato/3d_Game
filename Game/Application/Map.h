#pragma once
#include "Block.h"
#include "Game/Collision/Collider.h"
#include "BlockType.h"
struct MapIndex {
	uint32_t xIndex;
	uint32_t yIndex;
};

struct Rect {
	float left;
	float right;
	float bottom;
	float top;
};

static inline const float kBlockWidth = 1.0f; // ブロックのサイズ
static inline const float kBlockHeight = 1.0f; // ブロックのサイズ
static inline const uint32_t kNumBlockVirtical = 30; // ブロックの縦の数
static inline const uint32_t kNumBlockHorizontal = 100; // ブロックの横の数

class CsvLoader;
class Map
{
public:
	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize(std::string csvFilePath);

	/// <summary>
	/// 更新処理
	/// <summary>
	void Update();

	/// <summary>
	/// 描画処理
	/// </summary>
	void Draw();

	/// <summary>
	/// 読み込んだマップデータを基にブロックを生成し配置する
	/// </summary>
	void CreateBlocksMap();

	/// <summary>
	/// 指定した範囲内にあるブロックを取得する
	/// </summary>
	std::vector<Block*>GetNearbyBlocks(const AABB& range) const;


public:	// Setter / Getter
	/// <summary>
	/// csvファイルのパスを設定する
	/// </summary>
	void SetCsvFilePath(const std::string& path) { this->csvFilePath = path; }

	/// <summary>
	/// 指定した位置にあるマップチップのインデックスを取得する
	/// </summary>
	MapIndex GetMapChipIndexSetByPosition(const Vector3& position);

	/// <summary>
	/// 指定したインデックスにあるマップチップの種類を取得する
	/// </summary>
	BlockType GetMapChipTypeByIndex(const MapIndex& index);

	/// <summary>
	/// 指定したインデックスにあるマップチップの矩形情報を取得する
	/// </summary>
	Rect GetRectByIndex(const MapIndex& index);

	/// <summary>
	/// マップチップの位置の取得
	/// </summary>
	Vector3 GetMapChipPositionByIndex(const MapIndex& index);

private:

	/// CSVファイルを読み込む
	std::vector<std::vector<int>> mapData;

	/// CSVファイルのパス
	std::string csvFilePath;

	/// ブロックのリスト
	std::vector<std::vector<std::unique_ptr<Block>>> blocks;

};
