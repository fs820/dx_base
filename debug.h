//------------------------------------------
//
// デバッグ処理の定義 [debug.h]
// Author: fuma sato
//
//------------------------------------------
#pragma once
#include "main.h"
#include "renderer.h"

//------------------------
// デバッグクラス
//------------------------
class CDebugProc final
{
// 公開
public:
	CDebugProc() : m_pFont{} {};
	~CDebugProc() = default;

	HRESULT Init(void);
	void Uninit(void);
    void Draw(void);

	static void  Print(const char* format, ...);

	static void SetColor(D3DXCOLOR color) { m_color = color; };
	static void SetDebugOp(UINT flag) { m_flag = flag; }

	static void ToggleDebugDraw(void) { m_bDraw = !m_bDraw; };

private:
	static string m_sDebug;   // 表示文字列
	static D3DXCOLOR m_color; // 文字色
	static UINT m_flag;       // DrawText設定
	static bool m_bDraw;      // 表示切替

	LPD3DXFONT m_pFont;       // フォントポインタ

};