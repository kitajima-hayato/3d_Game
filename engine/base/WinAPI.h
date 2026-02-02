#pragma once
#include <Windows.h>
#include <cstdint>
/// <summary>
/// WindowsAPIのラッパークラス
/// </summary>
class WinAPI
{
public:
	/// <summary>
	/// ウィンドウからのメッセージ処理関数
	/// </summary>
	/// <param name="hwnd">メッセージを受け取ったウィンドウのハンドル</param>
	/// <param name="msg">送信されたメッセージID</param>
	/// <param name="wparam">メッセージに付随する追加情報 / WPRAM </param>
	/// <param name="lparam">メッセージに付随する追加情報 / LPRAM</param>
	/// <returns>メッセージの処理結果　処理済みなら０</returns>
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
	//定数
	//クライアント領域のサイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;

public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();


	/// <summary>
	/// 終了処理
	/// </summary>
	void Finalize();
	
	
	/// <summary>
	/// メッセージ処理
	/// </summary>
	bool ProcessMessage();

	//getter
	/// <summary>
	/// ウィンドウハンドルの取得
	/// </summary>
	HWND GetHwnd() const { return hwnd; }
	/// <summary>
	/// ウィンドウクラスのインスタンスハンドルの取得
	/// </summary>
	HINSTANCE GetHInstance()const { return wc.hInstance; }
private:
	//ウィンドウハンドル
	HWND hwnd = nullptr;

	//ウィンドウクラスの設定
	WNDCLASS wc{};

};

