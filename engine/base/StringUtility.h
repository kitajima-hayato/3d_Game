#pragma once
#include <string>
/// <summary>
/// stringユーティリティ
/// </summary>
namespace StringUtility {

	/// <summary>
	/// stringをwstringに変換する
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	std::wstring ConvertString(const std::string& str);
	/// <summary>
	/// wstringをstringに変換する
	/// </summary>
	/// <param name="str"></param>
	/// <returns></returns>
	std::string ConvertString(const std::wstring& str);
}