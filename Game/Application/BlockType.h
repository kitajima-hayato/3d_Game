#pragma once

enum class BlockType
{
	Air = 0,
	NormalBlock = 1,
	testBlock = 2,
	kGoalUp = 3,
	kGoalDown = 4,
	// 範囲検索の最終値
	/// 検索を行う際にCountを指定することで範囲検索を可能にできる
	Count,	
};