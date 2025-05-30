//------------------------------------------
//
//メインの処理の定義・宣言[main.h]
//Author: fuma sato
//
//------------------------------------------
#pragma once

// プロジェクト共通ヘッダ
#include <windows.h> // Windows標準
#include <chrono>    // 時間
#include <string>    // 文字列

using namespace std::chrono; // 省略
using std::string;           //

//-------------------------------------------------
// メインのクラス (インスタンス禁止) (継承禁止)
//-------------------------------------------------
class CMain final
{
	// 公開
public:
	CMain() = delete;

	static HWND GethWnd(void) { return m_hWnd; }
	static void SethWnd(HWND hWnd) { m_hWnd = hWnd; }
	static bool IsStop(void) { return m_bStop; }
	static bool IsFullScreen(void) { return m_bFullScreen; }

	static float GetElapsedTime(void) { return m_elapsedTime; }
	static float GetElapsedTimeGameSpeed(void) { return m_elapsedTime * m_gameSpeed; }
	static void SetElapsedTime(float elapsedTime) { m_elapsedTime = elapsedTime; }
	static float GetDeltaTime(void) { return m_deltaTime; }
	static float GetDeltaTimeGameSpeed(void) { return m_deltaTime * m_gameSpeed; }
	static void SetDeltaTime(float deltaTime) { m_deltaTime = deltaTime; }

	static float GetGameSpeed(void) { return m_gameSpeed; }
	static void SetGameSpeed(float gameSpeed) { m_gameSpeed = gameSpeed; }

	static string GetFilePath(void);
	static HRESULT GetClientRect(RECT* rect);

	static LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static void ToggleFullScreen(HWND hWnd);
	static void ToggleBorderless(HWND hWnd);

	//--------------------
	// プロジェクト定数
	//--------------------

	// 名前
	static constexpr const char* CLASS_NAME = "WindowClass";        // クラス
	static constexpr const char* WINDOW_NAME = "シューティング";    // ウインドウ

	// ウインドウのサイズ
	//static constexpr float SCREEN_WIDTH = 1280.0f;  // 幅
	//static constexpr float SCREEN_HEIGHT = 720.0f;  // 高さ
	static constexpr float SCREEN_WIDTH = 1920.0f;   // 幅
	static constexpr float SCREEN_HEIGHT = 1080.0f;  // 高さ
	//static constexpr float SCREEN_WIDTH = 2560.0f;  // 幅
	//static constexpr float SCREEN_HEIGHT = 1440.0f; // 高さ

	// フルスクリーンをボーダーレスで表現
	static constexpr bool ISBORDERLESS = true;

	// Fps(1秒間に何回描画するか)
	static constexpr int FPS = 60;

	// 非公開
private:
	//--------------------
	// プロジェクト変数
	//--------------------
	static HWND m_hWnd;         // ウインドウハンドル
	static bool m_bFullScreen;  // フルスクリーンフラグ
	static float m_elapsedTime; // 起動時間
	static float m_deltaTime;   // フレーム時間
	static bool m_bStop;        // ストップフラグ
	static float m_gameSpeed;   // ゲームスピード

};