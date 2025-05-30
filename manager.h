//------------------------------------------
//
// マネージャーの定義 [manager.h]
// Author: fuma sato
//
//------------------------------------------
#pragma once

#include "main.h"

class CRenderer;      // レンダラー
class CDebugProc;     // デバック表示
class CInputKeyboard; // キーボード
class CBg;            // 背景
class CPlayer;        // プレイヤー

//----------------------------
// マネージャークラス
//----------------------------
class CManager
{
// 公開
public:
	CManager() = default;
	~CManager() = default;

	HRESULT Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow);
	void Update(void);
	void Draw(void);
	void Uninit(void);

	static CRenderer* GetRenderer(void) { return m_pRenderer; }
	static HRESULT RenererReset(const BOOL bWindow);

	static HRESULT GameLoad(void);
	static HRESULT GameStart(void);
	static void GameEnd(void);
	static void GameUnload(void);

	static CDebugProc* GetDebugProc(void) { return m_pDebugProc; }
	static CInputKeyboard* GetInputKeyboard(void) { return m_pInputKeyboard; }
	static CBg* GetBg(void) { return m_pBg; }
	static CPlayer* GetPlayer(void) { return m_pPlayer; }

	static bool IsPause(void) { return m_bPause; }

// 非公開
private:
	static CRenderer* m_pRenderer;           // レンダラーのポインタ
	static CDebugProc* m_pDebugProc;         // デバック表示のポインタ
	static CInputKeyboard* m_pInputKeyboard; // キーボードのポインタ
	static CBg* m_pBg;                       // 背景のポインタ
	static CPlayer* m_pPlayer;               // プレイヤーのポインタ

	static bool m_bPause;                    // ポーズ状態かどうか
};