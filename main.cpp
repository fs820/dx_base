//------------------------------------------
//
//���C���̏���[main.cpp]
//Author: fuma sato
//
//------------------------------------------
#include "main.h"
#include "manager.h"
#include "debug.h"

#include <ShellScalingApi.h>  // SetProcessDpiAwareness�p
#include <Shlwapi.h> // �p�X����p (PathRelativePathTo�Ȃ�)

#pragma comment(lib, "winmm.lib")

// C++�̕W�����C�u����
#pragma comment(lib, "Shcore.lib")  // SetProcessDpiAwareness�p
#pragma comment(lib, "Shlwapi.lib") // Shlwapi���C�u�����������N

// CMain�N���X�̐ÓI�����o�ϐ��̒�`
HWND CMain::m_hWnd = nullptr;        // �E�C���h�E�n���h��
bool CMain::m_bFullScreen = false;   // �t���X�N���[���t���O
float CMain::m_elapsedTime = 0.0f;   // ���s����
float CMain::m_deltaTime = 0.0f;     // �t���[������
bool CMain::m_bStop = false;         // �X�g�b�v�t���O
float CMain::m_gameSpeed = 1.0f;     // �Q�[���X�s�[�h

//------------------------
//���C���֐�
//------------------------
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hInstancePrev, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	//�E�C���h�E�ݒ�
	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),//�������T�C�Y
		CS_CLASSDC,//�X�^�C��
		CMain::WindowProc,//�v���V�[�W��
		0,//0
		0,//0
		hInstance,//�C���X�^���X�n���h��
		LoadIcon(NULL,IDI_APPLICATION),//�^�X�N�o�[�A�C�R��
		LoadCursor(NULL,IDC_ARROW),//�}�E�X�J�[�\��
		(HBRUSH)(COLOR_WINDOW + 23),//�w�i�F
		NULL,//���j���[�o�[												
		CMain::CLASS_NAME,//�N���X�̖��O
		LoadIcon(NULL,IDI_APPLICATION)//�A�C�R��
	};

	MSG msg = { 0 };//���b�Z�[�W
	RECT rect = { 0,0,(LONG)CMain::SCREEN_WIDTH,(LONG)CMain::SCREEN_HEIGHT};//�E�C���h�E�T�C�Y�̐ݒ�

	//�E�C���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// DPI�X�P�[�����O�Ή�
	if (FAILED(SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE)))
	{
		SetProcessDPIAware();  // Windows 7 �݊��p
	}

	//�N���C�A���g�̈�̃T�C�Y����
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	// �E�C���h�E�̃X�^�C����ݒ�
	DWORD style = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;

	//�E�C���h�E�𐶐�
	HWND hWnd = NULL;
	hWnd = CreateWindowEx
	(
		0,//�X�^�C��
		CMain::CLASS_NAME,      //�E�C���h�E�N���X�̖��O
		CMain::WINDOW_NAME,     //�E�C���h�E�̖��O
		style,                    //�X�^�C��
		CW_USEDEFAULT,            //������W
		CW_USEDEFAULT,            //�E�����W
		(rect.right - rect.left), //��
		(rect.bottom - rect.top), //����
		NULL,                     //�e
		NULL,                     //���j���[ID
		hInstance,                //�C���X�^���X�n���h��
		NULL                      //�쐬�f�[�^
	);

	//�\��
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	CMain::SethWnd(hWnd);

	// �}�l�[�W���[�̐���
	CManager* pManager = NULL;
	if (pManager == NULL)
	{
		pManager = new CManager;
	}
	
	// �}�l�[�W���[�̏�����
	if (pManager != NULL)
	{
		if (FAILED(pManager->Init(hInstance, hWnd, (CMain::ISBORDERLESS || !CMain::IsFullScreen()))))
		{
			return E_FAIL;
		}
	}

	timeBeginPeriod(1);//����\�̐ݒ�

	float Frame = (1.0f / (float)CMain::FPS);                    // �t���[��(�b)
	steady_clock::time_point startTime = steady_clock::now();    // �J�n���� (�����P��)
	steady_clock::time_point exceLastTime = steady_clock::now(); // �O�񎞍� (�����P��)
	steady_clock::time_point currntTime = steady_clock::now();   // ���ݎ��� (�����P��)

	//���b�Z�[�W���[�v
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{
			if (msg.message == WM_QUIT)//�I�����b�Z�[�W
			{
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		currntTime = steady_clock::now();                                     // �������擾 (�����P��)

		float elapsedTime = duration<float>(currntTime - startTime).count();  // �o�ߎ��� (�b)
		float deltaTime = duration<float>(currntTime - exceLastTime).count(); // �t���[���Ԋu (�b)

		CMain::SetElapsedTime(elapsedTime);                                   // �o�^
		CMain::SetDeltaTime(deltaTime);                                       //

		if (deltaTime >= Frame)//�ݒ肵���Ԋu���󂢂���
		{// �X�V
			exceLastTime = currntTime; //����̍X�V�������L�^

			// �f�o�b�N�\��
			CDebugProc::Print("�o�ߎ���:%f\n", elapsedTime);
			CDebugProc::Print("�t���[������:%f\n", deltaTime);
			CDebugProc::Print("FPS:%f\n", 1.0f / deltaTime);
			CDebugProc::Print("�I��:[ESC]\n");
			CDebugProc::Print("�t���X�N���[��:[F11]\n");

			if (!CMain::IsStop())
			{// �X�V��~��ԂłȂ�
				pManager->Update(); //�X�V����
			}
			pManager->Draw();   //�`�揈��
		}
	}

	// �}�l�[�W���[�̔j��
	if (pManager != NULL)
	{
		pManager->Uninit();
		delete pManager;
		pManager = NULL;
	}

	//�E�C���h�E�N���X�̓o�^����
	UnregisterClass(CMain::CLASS_NAME, wcex.hInstance);

	timeEndPeriod(1);//����\��߂�

	//�I��
	return (int)msg.wParam;
}

//--------------------------
//�E�C���h�E�v���V�[�W��
//--------------------------
LRESULT CALLBACK CMain::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nID = 0;//�Ԃ�l�̊i�[

	switch (uMsg)
	{
	case WM_CREATE://����
	{
		// ����������
		DragAcceptFiles(hWnd, TRUE);

		RAWINPUTDEVICE RawDevice[2] =
		{
		{ 0x01, 0x06, RIDEV_INPUTSINK, hWnd }, // �L�[�{�[�h
		{ 0x01, 0x02, RIDEV_INPUTSINK, hWnd }  // �}�E�X
		};
		RegisterRawInputDevices(RawDevice, 2, sizeof(RAWINPUTDEVICE));
		break;
	}
	case WM_DESTROY://�j��
		if (m_bFullScreen && !ISBORDERLESS)
		{// �t���X�N���[�����[�h�̎�
			ChangeDisplaySettings(NULL, 0); // �E�C���h�E���[�h��߂�
		}
		//WM_QUIT���b�Z�[�W
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN://�L�[����
		switch (wParam)
		{
		case VK_ESCAPE://ESC�L�[
			nID = MessageBox(hWnd, "�I�����܂���?", "�I�����b�Z�[�W", MB_YESNO | MB_ICONQUESTION);
			if (nID == IDYES)
			{
				//�j��(WM_DESTROY)
				DestroyWindow(hWnd);
			}
			break;
		case VK_F11://F11�L�[
			if (ISBORDERLESS)
			{// �{�[�_�[���X�E�B���h�E���[�h�̎�
				ToggleBorderless(hWnd);
			}
			else
			{// �t���X�N���[�����[�h�̎�
				ToggleFullScreen(hWnd);
			}
			break;
		}
		break;
	case WM_SIZE:
		switch (wParam)
		{
		case SIZE_MINIMIZED:
			m_bStop = true;
			break;
		default:
			if (FAILED(CManager::RenererReset((CMain::ISBORDERLESS || !m_bFullScreen))))
			{
				return E_FAIL; // �����_���[�̃��Z�b�g
			}
			m_bStop = false;
			break;
		}
		break;
	case WM_ENTERSIZEMOVE:
		m_bStop = true;
		break;
	case WM_EXITSIZEMOVE:
		m_bStop = false;
		break;
	case WM_KILLFOCUS:
		m_bStop = true;
		break;
	case WM_SETFOCUS:
		m_bStop = false;
		break;
	case WM_ACTIVATEAPP:
		if (wParam == TRUE)
		{
			m_bStop = false;
		}
		else
		{
			m_bStop = true;
		}
		break;
	case WM_SYSCOMMAND:
		if ((wParam & 0xFFF0) == SC_CLOSE)//�E�C���h�E�́~�{�^��
		{

		}
		else if ((wParam & 0xFFF0) == SC_MAXIMIZE)//�ő剻�{�^��
		{
			m_bStop = false;
		}
		else if ((wParam & 0xFFF0) == SC_MINIMIZE)//�ŏ����{�^��
		{
			m_bStop = true;
		}
		break;
	case WM_CLOSE://�E�C���h�E�́~�{�^��
		nID = MessageBox(hWnd, "�I�����܂���?", "�I�����b�Z�[�W", MB_YESNO | MB_ICONQUESTION);
		if (nID == IDYES)
		{
			//�j��(WM_DESTROY)
			DestroyWindow(hWnd);
		}
		break;
	case WM_DISPLAYCHANGE://�f�B�X�v���C�̕ύX
		break;
	case WM_DEVICECHANGE:
		// Input�̍Ď擾
		break;
	case WM_POWERBROADCAST:
		break;
	case WM_DROPFILES:
	{
		HDROP hDrop = (HDROP)wParam;
		char filePath[MAX_PATH] = { 0 };
		DragQueryFile(hDrop, 0, filePath, MAX_PATH);
		DragFinish(hDrop);
		// �h���b�v���ꂽ�t�@�C���̃p�X���擾
		string droppedFilePath = filePath;
		if (!droppedFilePath.empty())
		{
			// �t�@�C���p�X���g�p���鏈���������ɒǉ�
			MessageBox(hWnd, droppedFilePath.c_str(), "Dropped File", MB_OK);
		}
		break;
	}
	case WM_INPUT:
	{
		// RAW���͂̏����������ɒǉ�
		RAWINPUT rawInput;
		UINT dwSize = sizeof(rawInput);
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &rawInput, &dwSize, sizeof(RAWINPUTHEADER));
		if (rawInput.header.dwType == RIM_TYPEKEYBOARD)
		{
			// �L�[�{�[�h�̓��͏����������ɒǉ�
			if (rawInput.data.keyboard.VKey == VK_F1)
			{
				SetGameSpeed(0.2f); // �Q�[���X�s�[�h��ݒ�
			}
		}
		break;
	}
	}
	//�K�v�f�[�^��Ԃ�
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//----------------------------
// �t���X�N���[��
//----------------------------
void CMain::ToggleFullScreen(HWND hWnd)
{
	m_bFullScreen = !m_bFullScreen;
	static RECT windowRect = {};

	// �E�B���h�E�X�^�C��
	static LONG style = GetWindowLong(hWnd, GWL_STYLE);
	static LONG exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

	if (m_bFullScreen)
	{
		style = GetWindowLong(hWnd, GWL_STYLE);
		exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
		GetWindowRect(hWnd, &windowRect);

		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		DEVMODE dm = {};
		dm.dmSize = sizeof(dm);
		dm.dmPelsWidth = (DWORD)screenWidth;    // �𑜓x
		dm.dmPelsHeight = (DWORD)screenHeight;
		dm.dmBitsPerPel = 32; // �t���J���[
		dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

		ChangeDisplaySettings(&dm, CDS_FULLSCREEN);

		LONG newStyle = style & ~(WS_OVERLAPPEDWINDOW);
		SetWindowLong(hWnd, GWL_STYLE, newStyle);

		SetWindowPos
		(
			hWnd,
			HWND_TOP,
			0, 0,
			screenWidth, screenHeight,
			SWP_FRAMECHANGED
		);
	}
	else
	{
		ChangeDisplaySettings(NULL, 0); // �f�B�X�v���C�ݒ�����ɖ߂�

		SetWindowLong(hWnd, GWL_STYLE, style);

		SetWindowPos
		(
			hWnd,
			NULL,
			windowRect.left, windowRect.top,
			windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
			SWP_FRAMECHANGED
		);
	}

	//�\��
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
}

//----------------------------
// �{�[�_�[���X�E�B���h�E
//----------------------------
void CMain::ToggleBorderless(HWND hWnd)
{
	m_bFullScreen = !m_bFullScreen;
	static RECT windowRect = {};

	// �E�B���h�E�X�^�C��
	static LONG style = GetWindowLong(hWnd, GWL_STYLE);
	static LONG exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

	if (m_bFullScreen)
	{
		// ���݂̃E�B���h�E�ʒu�ƃT�C�Y�ƃX�^�C����ۑ�
		style = GetWindowLong(hWnd, GWL_STYLE);
		exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
		GetWindowRect(hWnd, &windowRect);

		// �t���X�N���[�����[�h�p�̃X�^�C���ɕύX
		// �{�[�_���X�E�B���h�E�������̗p

		LONG newStyle = style & ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
		LONG newExStyle = exStyle & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);

		// �X�^�C����K�p
		SetWindowLong(hWnd, GWL_STYLE, newStyle);
		SetWindowLong(hWnd, GWL_EXSTYLE, newExStyle);

		// ��ʃT�C�Y���擾
		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		// �E�B���h�E���X�N���[���T�C�Y�ɍ��킹��
		SetWindowPos(
			hWnd,
			HWND_TOP,
			0, 0,
			screenWidth, screenHeight,
			SWP_FRAMECHANGED
		);
	}
	else
	{
		// �ʏ�E�B���h�E���[�h�ɖ߂�
		SetWindowLong(hWnd, GWL_STYLE, style);
		SetWindowLong(hWnd, GWL_EXSTYLE, exStyle);

		// ���̃E�B���h�E�T�C�Y�ƈʒu�ɖ߂�
		SetWindowPos(
			hWnd,
			HWND_TOP,
			windowRect.left, windowRect.top,
			windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
			SWP_FRAMECHANGED
		);
	}

	// �\���ƍX�V
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
}

//----------------------------
// �t�@�C���p�X�擾
//----------------------------
string CMain::GetFilePath(void)
{
	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";
	char currentDir[MAX_PATH] = "";

	// ���݂̃f�B���N�g�����擾
	GetCurrentDirectory(MAX_PATH, currentDir);
	strcat_s(currentDir, MAX_PATH, "\\data\0");

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "�摜�t�@�C��\0*.jpg;*.jpeg;*.png;*.bmp\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "";
	ofn.lpstrInitialDir = currentDir; // �����f�B���N�g����ݒ�

	if (GetOpenFileName(&ofn))
	{
		return string(fileName);
	}

	return "";
}

//----------------------------
// �N���C�A���g�̈�擾
//----------------------------
HRESULT CMain::GetClientRect(RECT* rect)
{
	if (rect == NULL)
	{
		return E_FAIL;
	}
	// �E�B���h�E�̃N���C�A���g�̈���擾
	if (!::GetClientRect(m_hWnd, rect))
	{
		return E_FAIL;
	}
	return S_OK;
}