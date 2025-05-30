//------------------------------------------
//
// �}�l�[�W���[�̒�` [manager.h]
// Author: fuma sato
//
//------------------------------------------
#pragma once

#include "main.h"

class CRenderer;      // �����_���[
class CDebugProc;     // �f�o�b�N�\��
class CInputKeyboard; // �L�[�{�[�h
class CBg;            // �w�i
class CPlayer;        // �v���C���[

//----------------------------
// �}�l�[�W���[�N���X
//----------------------------
class CManager
{
// ���J
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

// ����J
private:
	static CRenderer* m_pRenderer;           // �����_���[�̃|�C���^
	static CDebugProc* m_pDebugProc;         // �f�o�b�N�\���̃|�C���^
	static CInputKeyboard* m_pInputKeyboard; // �L�[�{�[�h�̃|�C���^
	static CBg* m_pBg;                       // �w�i�̃|�C���^
	static CPlayer* m_pPlayer;               // �v���C���[�̃|�C���^

	static bool m_bPause;                    // �|�[�Y��Ԃ��ǂ���
};