//------------------------------------------
//
// �f�o�b�O�����̒�` [debug.h]
// Author: fuma sato
//
//------------------------------------------
#pragma once
#include "main.h"
#include "renderer.h"

//------------------------
// �f�o�b�O�N���X
//------------------------
class CDebugProc final
{
// ���J
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
	static string m_sDebug;   // �\��������
	static D3DXCOLOR m_color; // �����F
	static UINT m_flag;       // DrawText�ݒ�
	static bool m_bDraw;      // �\���ؑ�

	LPD3DXFONT m_pFont;       // �t�H���g�|�C���^

};