//------------------------------------------
//
//���C���̏����̒�`�E�錾[main.h]
//Author: fuma sato
//
//------------------------------------------
#pragma once

// �v���W�F�N�g���ʃw�b�_
#include <windows.h> // Windows�W��
#include <chrono>    // ����
#include <string>    // ������

using namespace std::chrono; // �ȗ�
using std::string;           //

//-------------------------------------------------
// ���C���̃N���X (�C���X�^���X�֎~) (�p���֎~)
//-------------------------------------------------
class CMain final
{
	// ���J
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
	// �v���W�F�N�g�萔
	//--------------------

	// ���O
	static constexpr const char* CLASS_NAME = "WindowClass";        // �N���X
	static constexpr const char* WINDOW_NAME = "�V���[�e�B���O";    // �E�C���h�E

	// �E�C���h�E�̃T�C�Y
	//static constexpr float SCREEN_WIDTH = 1280.0f;  // ��
	//static constexpr float SCREEN_HEIGHT = 720.0f;  // ����
	static constexpr float SCREEN_WIDTH = 1920.0f;   // ��
	static constexpr float SCREEN_HEIGHT = 1080.0f;  // ����
	//static constexpr float SCREEN_WIDTH = 2560.0f;  // ��
	//static constexpr float SCREEN_HEIGHT = 1440.0f; // ����

	// �t���X�N���[�����{�[�_�[���X�ŕ\��
	static constexpr bool ISBORDERLESS = true;

	// Fps(1�b�Ԃɉ���`�悷�邩)
	static constexpr int FPS = 60;

	// ����J
private:
	//--------------------
	// �v���W�F�N�g�ϐ�
	//--------------------
	static HWND m_hWnd;         // �E�C���h�E�n���h��
	static bool m_bFullScreen;  // �t���X�N���[���t���O
	static float m_elapsedTime; // �N������
	static float m_deltaTime;   // �t���[������
	static bool m_bStop;        // �X�g�b�v�t���O
	static float m_gameSpeed;   // �Q�[���X�s�[�h

};