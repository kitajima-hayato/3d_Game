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
	/// コンストラクタ
	/// </summary>
	/// <param name="hwnd"></param>
	/// <param name="msg"></param>
	/// <param name="wparam"></param>
	/// <param name="lparam"></param>
	/// <returns></returns>
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

