//------------------------------------------
//
//メインの処理[main.cpp]
//Author: fuma sato
//
//------------------------------------------
#include "main.h"
#include "manager.h"
#include "debug.h"

#include <ShellScalingApi.h>  // SetProcessDpiAwareness用
#include <Shlwapi.h> // パス操作用 (PathRelativePathToなど)

#pragma comment(lib, "winmm.lib")

// C++の標準ライブラリ
#pragma comment(lib, "Shcore.lib")  // SetProcessDpiAwareness用
#pragma comment(lib, "Shlwapi.lib") // Shlwapiライブラリをリンク

// CMainクラスの静的メンバ変数の定義
HWND CMain::m_hWnd = nullptr;        // ウインドウハンドル
bool CMain::m_bFullScreen = false;   // フルスクリーンフラグ
float CMain::m_elapsedTime = 0.0f;   // 実行時間
float CMain::m_deltaTime = 0.0f;     // フレーム時間
bool CMain::m_bStop = false;         // ストップフラグ
float CMain::m_gameSpeed = 1.0f;     // ゲームスピード

//------------------------
//メイン関数
//------------------------
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hInstancePrev, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
	//ウインドウ設定
	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),//メモリサイズ
		CS_CLASSDC,//スタイル
		CMain::WindowProc,//プロシージャ
		0,//0
		0,//0
		hInstance,//インスタンスハンドル
		LoadIcon(NULL,IDI_APPLICATION),//タスクバーアイコン
		LoadCursor(NULL,IDC_ARROW),//マウスカーソル
		(HBRUSH)(COLOR_WINDOW + 23),//背景色
		NULL,//メニューバー												
		CMain::CLASS_NAME,//クラスの名前
		LoadIcon(NULL,IDI_APPLICATION)//アイコン
	};

	MSG msg = { 0 };//メッセージ
	RECT rect = { 0,0,(LONG)CMain::SCREEN_WIDTH,(LONG)CMain::SCREEN_HEIGHT};//ウインドウサイズの設定

	//ウインドウクラスの登録
	RegisterClassEx(&wcex);

	// DPIスケーリング対応
	if (FAILED(SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE)))
	{
		SetProcessDPIAware();  // Windows 7 互換用
	}

	//クライアント領域のサイズ調整
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	// ウインドウのスタイルを設定
	DWORD style = WS_OVERLAPPEDWINDOW & ~WS_THICKFRAME & ~WS_MAXIMIZEBOX;

	//ウインドウを生成
	HWND hWnd = NULL;
	hWnd = CreateWindowEx
	(
		0,//スタイル
		CMain::CLASS_NAME,      //ウインドウクラスの名前
		CMain::WINDOW_NAME,     //ウインドウの名前
		style,                    //スタイル
		CW_USEDEFAULT,            //左上座標
		CW_USEDEFAULT,            //右下座標
		(rect.right - rect.left), //幅
		(rect.bottom - rect.top), //高さ
		NULL,                     //親
		NULL,                     //メニューID
		hInstance,                //インスタンスハンドル
		NULL                      //作成データ
	);

	//表示
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	CMain::SethWnd(hWnd);

	// マネージャーの生成
	CManager* pManager = NULL;
	if (pManager == NULL)
	{
		pManager = new CManager;
	}
	
	// マネージャーの初期化
	if (pManager != NULL)
	{
		if (FAILED(pManager->Init(hInstance, hWnd, (CMain::ISBORDERLESS || !CMain::IsFullScreen()))))
		{
			return E_FAIL;
		}
	}

	timeBeginPeriod(1);//分解能の設定

	float Frame = (1.0f / (float)CMain::FPS);                    // フレーム(秒)
	steady_clock::time_point startTime = steady_clock::now();    // 開始時刻 (時刻単位)
	steady_clock::time_point exceLastTime = steady_clock::now(); // 前回時刻 (時刻単位)
	steady_clock::time_point currntTime = steady_clock::now();   // 現在時刻 (時刻単位)

	//メッセージループ
	while (true)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{
			if (msg.message == WM_QUIT)//終了メッセージ
			{
				break;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		currntTime = steady_clock::now();                                     // 時刻を取得 (時刻単位)

		float elapsedTime = duration<float>(currntTime - startTime).count();  // 経過時間 (秒)
		float deltaTime = duration<float>(currntTime - exceLastTime).count(); // フレーム間隔 (秒)

		CMain::SetElapsedTime(elapsedTime);                                   // 登録
		CMain::SetDeltaTime(deltaTime);                                       //

		if (deltaTime >= Frame)//設定した間隔が空いたら
		{// 更新
			exceLastTime = currntTime; //今回の更新時刻を記録

			// デバック表示
			CDebugProc::Print("経過時間:%f\n", elapsedTime);
			CDebugProc::Print("フレーム時間:%f\n", deltaTime);
			CDebugProc::Print("FPS:%f\n", 1.0f / deltaTime);
			CDebugProc::Print("終了:[ESC]\n");
			CDebugProc::Print("フルスクリーン:[F11]\n");

			if (!CMain::IsStop())
			{// 更新停止状態でない
				pManager->Update(); //更新処理
			}
			pManager->Draw();   //描画処理
		}
	}

	// マネージャーの破棄
	if (pManager != NULL)
	{
		pManager->Uninit();
		delete pManager;
		pManager = NULL;
	}

	//ウインドウクラスの登録解除
	UnregisterClass(CMain::CLASS_NAME, wcex.hInstance);

	timeEndPeriod(1);//分解能を戻す

	//終了
	return (int)msg.wParam;
}

//--------------------------
//ウインドウプロシージャ
//--------------------------
LRESULT CALLBACK CMain::WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	int nID = 0;//返り値の格納

	switch (uMsg)
	{
	case WM_CREATE://生成
	{
		// 初期化処理
		DragAcceptFiles(hWnd, TRUE);

		RAWINPUTDEVICE RawDevice[2] =
		{
		{ 0x01, 0x06, RIDEV_INPUTSINK, hWnd }, // キーボード
		{ 0x01, 0x02, RIDEV_INPUTSINK, hWnd }  // マウス
		};
		RegisterRawInputDevices(RawDevice, 2, sizeof(RAWINPUTDEVICE));
		break;
	}
	case WM_DESTROY://破棄
		if (m_bFullScreen && !ISBORDERLESS)
		{// フルスクリーンモードの時
			ChangeDisplaySettings(NULL, 0); // ウインドウモードを戻す
		}
		//WM_QUITメッセージ
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN://キー押下
		switch (wParam)
		{
		case VK_ESCAPE://ESCキー
			nID = MessageBox(hWnd, "終了しますか?", "終了メッセージ", MB_YESNO | MB_ICONQUESTION);
			if (nID == IDYES)
			{
				//破棄(WM_DESTROY)
				DestroyWindow(hWnd);
			}
			break;
		case VK_F11://F11キー
			if (ISBORDERLESS)
			{// ボーダーレスウィンドウモードの時
				ToggleBorderless(hWnd);
			}
			else
			{// フルスクリーンモードの時
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
				return E_FAIL; // レンダラーのリセット
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
		if ((wParam & 0xFFF0) == SC_CLOSE)//ウインドウの×ボタン
		{

		}
		else if ((wParam & 0xFFF0) == SC_MAXIMIZE)//最大化ボタン
		{
			m_bStop = false;
		}
		else if ((wParam & 0xFFF0) == SC_MINIMIZE)//最小化ボタン
		{
			m_bStop = true;
		}
		break;
	case WM_CLOSE://ウインドウの×ボタン
		nID = MessageBox(hWnd, "終了しますか?", "終了メッセージ", MB_YESNO | MB_ICONQUESTION);
		if (nID == IDYES)
		{
			//破棄(WM_DESTROY)
			DestroyWindow(hWnd);
		}
		break;
	case WM_DISPLAYCHANGE://ディスプレイの変更
		break;
	case WM_DEVICECHANGE:
		// Inputの再取得
		break;
	case WM_POWERBROADCAST:
		break;
	case WM_DROPFILES:
	{
		HDROP hDrop = (HDROP)wParam;
		char filePath[MAX_PATH] = { 0 };
		DragQueryFile(hDrop, 0, filePath, MAX_PATH);
		DragFinish(hDrop);
		// ドロップされたファイルのパスを取得
		string droppedFilePath = filePath;
		if (!droppedFilePath.empty())
		{
			// ファイルパスを使用する処理をここに追加
			MessageBox(hWnd, droppedFilePath.c_str(), "Dropped File", MB_OK);
		}
		break;
	}
	case WM_INPUT:
	{
		// RAW入力の処理をここに追加
		RAWINPUT rawInput;
		UINT dwSize = sizeof(rawInput);
		GetRawInputData((HRAWINPUT)lParam, RID_INPUT, &rawInput, &dwSize, sizeof(RAWINPUTHEADER));
		if (rawInput.header.dwType == RIM_TYPEKEYBOARD)
		{
			// キーボードの入力処理をここに追加
			if (rawInput.data.keyboard.VKey == VK_F1)
			{
				SetGameSpeed(0.2f); // ゲームスピードを設定
			}
		}
		break;
	}
	}
	//必要データを返す
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

//----------------------------
// フルスクリーン
//----------------------------
void CMain::ToggleFullScreen(HWND hWnd)
{
	m_bFullScreen = !m_bFullScreen;
	static RECT windowRect = {};

	// ウィンドウスタイル
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
		dm.dmPelsWidth = (DWORD)screenWidth;    // 解像度
		dm.dmPelsHeight = (DWORD)screenHeight;
		dm.dmBitsPerPel = 32; // フルカラー
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
		ChangeDisplaySettings(NULL, 0); // ディスプレイ設定を元に戻す

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

	//表示
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
}

//----------------------------
// ボーダーレスウィンドウ
//----------------------------
void CMain::ToggleBorderless(HWND hWnd)
{
	m_bFullScreen = !m_bFullScreen;
	static RECT windowRect = {};

	// ウィンドウスタイル
	static LONG style = GetWindowLong(hWnd, GWL_STYLE);
	static LONG exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);

	if (m_bFullScreen)
	{
		// 現在のウィンドウ位置とサイズとスタイルを保存
		style = GetWindowLong(hWnd, GWL_STYLE);
		exStyle = GetWindowLong(hWnd, GWL_EXSTYLE);
		GetWindowRect(hWnd, &windowRect);

		// フルスクリーンモード用のスタイルに変更
		// ボーダレスウィンドウ方式を採用

		LONG newStyle = style & ~(WS_CAPTION | WS_THICKFRAME | WS_MINIMIZE | WS_MAXIMIZE | WS_SYSMENU);
		LONG newExStyle = exStyle & ~(WS_EX_DLGMODALFRAME | WS_EX_WINDOWEDGE | WS_EX_CLIENTEDGE | WS_EX_STATICEDGE);

		// スタイルを適用
		SetWindowLong(hWnd, GWL_STYLE, newStyle);
		SetWindowLong(hWnd, GWL_EXSTYLE, newExStyle);

		// 画面サイズを取得
		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		// ウィンドウをスクリーンサイズに合わせる
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
		// 通常ウィンドウモードに戻す
		SetWindowLong(hWnd, GWL_STYLE, style);
		SetWindowLong(hWnd, GWL_EXSTYLE, exStyle);

		// 元のウィンドウサイズと位置に戻す
		SetWindowPos(
			hWnd,
			HWND_TOP,
			windowRect.left, windowRect.top,
			windowRect.right - windowRect.left, windowRect.bottom - windowRect.top,
			SWP_FRAMECHANGED
		);
	}

	// 表示と更新
	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);
}

//----------------------------
// ファイルパス取得
//----------------------------
string CMain::GetFilePath(void)
{
	OPENFILENAME ofn;
	char fileName[MAX_PATH] = "";
	char currentDir[MAX_PATH] = "";

	// 現在のディレクトリを取得
	GetCurrentDirectory(MAX_PATH, currentDir);
	strcat_s(currentDir, MAX_PATH, "\\data\0");

	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = NULL;
	ofn.lpstrFilter = "画像ファイル\0*.jpg;*.jpeg;*.png;*.bmp\0";
	ofn.lpstrFile = fileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
	ofn.lpstrDefExt = "";
	ofn.lpstrInitialDir = currentDir; // 初期ディレクトリを設定

	if (GetOpenFileName(&ofn))
	{
		return string(fileName);
	}

	return "";
}

//----------------------------
// クライアント領域取得
//----------------------------
HRESULT CMain::GetClientRect(RECT* rect)
{
	if (rect == NULL)
	{
		return E_FAIL;
	}
	// ウィンドウのクライアント領域を取得
	if (!::GetClientRect(m_hWnd, rect))
	{
		return E_FAIL;
	}
	return S_OK;
}