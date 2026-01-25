#include "StageSelectGraph.h"
#include <fstream>  

StageSelectGraph::StageSelectGraph()
{
}

StageSelectGraph::~StageSelectGraph()
{
}

void StageSelectGraph::Initialize()
{
	// ノードの追加
	uint32_t nodeA = AddNode({ 0,0 }, 1, true);
	uint32_t nodeB = AddNode({ 1,0 }, 2, false);
	uint32_t nodeC = AddNode({ 1,1 }, 3, false);
	uint32_t nodeD = AddNode({ 2,0 }, 4, false);
	// ノードの接続
	Link(nodeA, nodeB, Direction::Right);
	Link(nodeB, nodeC, Direction::Up);
	Link(nodeB, nodeD, Direction::Down);

}


void StageSelectGraph::Finalize()
{
}

uint32_t StageSelectGraph::AddNode(MapPos pos, uint32_t stageId, bool unlocked)
{
	// 新しいノードを作成
	StageNode newNode{};
	newNode.id = static_cast<uint32_t>(nodes_.size());
	newNode.position = pos;
	newNode.unlocked = unlocked;

	// 全方向の隣接ノードを無効に設定
	for (uint32_t i = 0; i < (uint32_t)Direction::count; i++) {
		newNode.neighbor[i] = INVALID_NODE_ID;
	}
	nodes_.push_back(newNode);
	return newNode.id;
}



void StageSelectGraph::Link(uint32_t fromNodeId, uint32_t toNodeId, Direction dir)
{
	// ノード同士を接続
	nodes_[fromNodeId].neighbor[(uint32_t)dir] = toNodeId;
	// 逆方向も接続
	Direction reverseDir{};
	switch (dir) {
	case Direction::Up:		reverseDir = Direction::Down;	break;
	case Direction::Down:	reverseDir = Direction::Up;		break;
	case Direction::Left:	reverseDir = Direction::Right;	break;
	case Direction::Right:	reverseDir = Direction::Left;	break;
	}

	// 逆方向の接続
	nodes_[toNodeId].neighbor[(uint32_t)reverseDir] = fromNodeId;
}

uint32_t StageSelectGraph::Move(uint32_t currentNodeId, Direction dir) const
{
	// 移動先ノードIDを取得
	uint32_t nextNodeId = nodes_[currentNodeId].neighbor[(uint32_t)dir];
	
	// 未接続であれば現在のノードを返す
	if (nextNodeId == INVALID_NODE_ID) {
		return currentNodeId;
	}
	// ロックされていたら移動できない
	if (!nodes_[nextNodeId].unlocked) {
		return currentNodeId;
	}

	return nextNodeId;
}

const StageNode& StageSelectGraph::GetNode(uint32_t id) const
{

	return nodes_.at(id);

}

void StageSelectGraph::LoadMapNodeFromJson(const std::string& fileName)
{
	// JSONファイルからノード情報を読み込む処理
	// filePathはリソースフォルダからの相対パス
	const std::string frontFilePath = "resources/StageSelect";
	const std::string kExtension = ".json";
	const std::string fullPath = frontFilePath + "/" + fileName + kExtension;

	// ファイル読み込み
	std::ifstream file;

	// ファイルを開く
	file.open(fullPath);

	// ファイルが開けたかどうか
	if (file.fail()){
		// エラーメッセージを表示して終了
		printf("Failed to open file: %s\n", fullPath.c_str());
		return;
	}

	// JSON文字列
	nlohmann::json deserialized;
	// パース(解凍)
	file >> deserialized;

	assert(deserialized.contains("nodes"));
	assert(deserialized["nodes"].is_array());

	//assert(jsonNode.contains("position"));
	//assert(jsonNode["position"].contains("x"));
	//assert(jsonNode["position"]["x"].is_number_integer());

	// ノード情報の読み込み
	for (const auto& jsonNode : deserialized["nodes"]) {
		// ノード情報の取得
		MapPos pos{};
		pos.x = jsonNode["position"]["x"];
		pos.y = jsonNode["position"]["y"];
		uint32_t stageId = jsonNode["stage_id"];
		bool unlocked = jsonNode["unlocked"];
		// ノードの追加
		uint32_t nodeId = AddNode(pos, stageId, unlocked);
		// 隣接ノードの設定
		for (const auto& neighbor : jsonNode["neighbors"].items()) {
			std::string dirStr = neighbor.key();
			uint32_t neighborId = neighbor.value();
			Direction dir{};
			if (dirStr == "Up") dir = Direction::Up;
			else if (dirStr == "Down") dir = Direction::Down;
			else if (dirStr == "Left") dir = Direction::Left;
			else if (dirStr == "Right") dir = Direction::Right;
			else continue; // 不明な方向はスキップ
			// 隣接ノードの設定
			SetNeighbor(nodeId, dir, neighborId);
		}
	}



}

bool StageSelectGraph::RemoveNode(uint32_t id)
{
	return false;
}

bool StageSelectGraph::SetNodePos(uint32_t id, MapPos pos)
{
	// IDのチェック / 範囲外なら失敗
	if (id >= nodes_.size()) return false;
	// 座標の設定
	nodes_[id].position = pos;
	return true;
}


bool StageSelectGraph::SetNodeStageId(uint32_t id, uint32_t stageId)
{
	return false;
}

bool StageSelectGraph::SetNodeUnlocked(uint32_t id, bool unlocked)
{
	return false;
}

bool StageSelectGraph::SetNeighbor(uint32_t from, Direction dir, uint32_t toOrInvalid)
{
	// IDのチェック / 範囲外なら失敗
	if (from >= nodes_.size()) return false;
	// toが無効なノードIDでない場合、範囲外なら失敗
	if (toOrInvalid != INVALID_NODE_ID && toOrInvalid >= nodes_.size()) return false;
	// 隣接ノードの設定
	nodes_[from].neighbor[(int)dir] = toOrInvalid;
	return true;
}


bool StageSelectGraph::ClearNeighbor(uint32_t from, Direction dir)
{
	return false;
}
