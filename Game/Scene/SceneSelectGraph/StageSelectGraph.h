#pragma once
#include "MyMath.h"
#include <vector>
#include <cstdint>
#include <resources/json/json.hpp>
struct MapPos
{
	uint32_t x;
	uint32_t y;
};

enum class Direction
{
	Up,
	Down,
	Left,
	Right,
	count
};

struct StageNode {
	// ノードID
	uint32_t id;
	// ノードの位置
	MapPos position;
	// ステージID
	uint32_t stageId;
	// アンロック状態
	bool unlocked;
	// 隣接ノードID配列 / 読み方はネイバー
	uint32_t neighbor[(uint32_t)Direction::count];
};

/// <summary>
/// ステージセレクトグラフ
/// </summary>
class StageSelectGraph
{
	
public:
	// 無効なノードID
	static const uint32_t INVALID_NODE_ID = UINT32_MAX;
	// コンストラクタ・デストラクタ
	StageSelectGraph();
	~StageSelectGraph();

	/// <summary>
	/// 初期化処理
	/// </summary>
	void Initialize();


	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();

	/// <summary>
	/// ノード追加
	/// </summary>
	/// <param name="node">  </param>
	uint32_t AddNode(MapPos pos,uint32_t stageId,bool unlocked);

	/// <summary>
	/// ノード接続
	/// </summary>
	/// <param name="fromNodeId"></param>
	/// <param name="toNodeId"></param>
	/// <param name="dir"></param>
	void Link(uint32_t fromNodeId, uint32_t toNodeId,Direction dir);
	
	/// <summary>
	/// ノード移動
	/// </summary>
	/// <param name="currentNodeId">移動前</param>
	/// <param name="dir">移動の方向</param>
	/// <returns></returns>
	uint32_t Move(uint32_t currentNodeId, Direction dir) const;
	
	/// <summary>
	/// ノード取得
	/// </summary>
	/// <param name="id"></param>
	/// <returns></returns>
	const StageNode& GetNode(uint32_t id)const;

	/// <summary>
	/// jsonファイルからノード情報の読み込み
	/// </summary>
	/// <param name="jsonNode"></param>
	void LoadMapNodeFromJson(const std::string& fileName);


	nlohmann::json ToJson() const;

	std::string ToJsonString(int indent = 2) const;

	bool SaveToJsonFile(const std::string& fileName) const;
private:
	// ノードリスト
	std::vector<StageNode> nodes_;

	
public:
	// --- 編集API（ImGui用） ---
	bool RemoveNode(uint32_t id);
	// ノード情報編集
	bool SetNodePos(uint32_t id, MapPos pos);
	bool SetNodeStageId(uint32_t id, uint32_t stageId);
	bool SetNodeUnlocked(uint32_t id, bool unlocked);
	// 隣接ノード編集 / 追加・削除
	bool SetNeighbor(uint32_t from, Direction dir, uint32_t toOrInvalid);
	bool ClearNeighbor(uint32_t from, Direction dir);
	// ノード数取得
	uint32_t GetNodeCount() const { return (uint32_t)nodes_.size(); }


};

