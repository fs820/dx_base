//---------------------------------------
//
// 入力処理の定義 [input.h]
// Author: Fuma Sato
//
//---------------------------------------

#ifndef _INPUT_H_
#define _INPUT_H_
#include"main.h"

#define DIRECTINPUT_VERSION  0x0800
#include <dinput.h>
#include <xinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "xinput.lib")

//-----------------------------
// インプットクラス
//-----------------------------
class CInput
{
// 公開
public:
	CInput() : m_pDevice{} {};
	virtual ~CInput() = default;

	virtual HRESULT Init(HINSTANCE hInstanse, HWND hWnd);
	virtual void Uninit(void);
	virtual void Update(void) = 0;

// 家族公開
protected:
	static constexpr int RELEASE_TIME = 50; //ため時間
	static constexpr int REPEAT_TIME = 40;  //長押し判定

	static LPDIRECTINPUT8 m_pInput; // DirectInputインターフェース
	LPDIRECTINPUTDEVICE8 m_pDevice;	// デバイスインターフェース
};

//-----------------------------
// キーボードクラス
//-----------------------------
class CInputKeyboard : public CInput
{
// 公開
public:
	CInputKeyboard() : m_KeyState{}, m_KeyStateOld{}, m_bKeyPress{}, m_bKeyTrigger{}, m_bKeyRelease{}, m_bKeyRepeat{}, m_KeyPressTime{} {}
	~CInputKeyboard() = default;

	HRESULT Init(HINSTANCE hInstanse, HWND hWnd) override;
	void Uninit(void) override;
	void Update(void) override;

	bool GetPress(size_t key);
	bool GetTrigger(size_t key);
	bool GetRelease(size_t key);
	bool GetRepeat(size_t key);

// 非公開
private:
	static constexpr size_t MAX_KEY = 256; // キー数

	BYTE m_KeyState[MAX_KEY];	  // キー状態
	BYTE m_KeyStateOld[MAX_KEY];  // 前回キー状態

	bool m_bKeyPress[MAX_KEY];	  // キー押下状態
	bool m_bKeyTrigger[MAX_KEY];  // キー入力状態
	bool m_bKeyRelease[MAX_KEY];  // キー離し状態
	bool m_bKeyRepeat[MAX_KEY];   // キー長押し状態

	int m_KeyPressTime[MAX_KEY];  // キー押下時間
};

//-----------------------------
// キーボードクラス
//-----------------------------
class CInputdInputKeyboard : public CInputKeyboard
{
// 公開
public:
	CInputdInputKeyboard();
	~CInputdInputKeyboard();

	HRESULT Init(HINSTANCE hInstanse, HWND hWnd) override;
	void Uninit(void) override;
	void Update(void) override;

	bool GetPress(size_t key);
	bool GetTrigger(size_t key);
	bool GetRelease(size_t key);
	bool GetRepeat(size_t key);

// 非公開
private:
	static constexpr size_t MAX_KEY = 256; // キー数

	BYTE m_KeyState[MAX_KEY];	  // キー状態
	BYTE m_KeyStateOld[MAX_KEY];  // 前回キー状態

	bool m_bKeyPress[MAX_KEY];	  // キー押下状態
	bool m_bKeyTrigger[MAX_KEY];  // キー入力状態
	bool m_bKeyRelease[MAX_KEY];  // キー離し状態
	bool m_bKeyRepeat[MAX_KEY];   // キー長押し状態

	int m_KeyPressTime[MAX_KEY];  // キー押下時間
};

//-----------------------------
// キーボードクラス
//-----------------------------
class CInputRawInputKeyboard : public CInputKeyboard
{
// 公開
public:
	CInputRawInputKeyboard();
	~CInputRawInputKeyboard();

	HRESULT Init(HINSTANCE hInstanse, HWND hWnd) override;
	void Uninit(void) override;
	void Update(void) override;

	bool GetPress(size_t key);
	bool GetTrigger(size_t key);
	bool GetRelease(size_t key);
	bool GetRepeat(size_t key);

// 非公開
private:
	static constexpr size_t MAX_KEY = 256; // キー数

	BYTE m_KeyState[MAX_KEY];	  // キー状態
	BYTE m_KeyStateOld[MAX_KEY];  // 前回キー状態

	bool m_bKeyPress[MAX_KEY];	  // キー押下状態
	bool m_bKeyTrigger[MAX_KEY];  // キー入力状態
	bool m_bKeyRelease[MAX_KEY];  // キー離し状態
	bool m_bKeyRepeat[MAX_KEY];   // キー長押し状態

	int m_KeyPressTime[MAX_KEY];  // キー押下時間
};

//-----------------------------
// マウスクラス
//-----------------------------
class CInputMouse : public CInput
{
// 公開
public:
	//マウスボタンの種類
	using BUTTON = enum
	{
		LEFT = 0,
		RIGHT,
		SENTER,
		B1,
		B2,
		B3,
		B4,
		B5,
		BUTTON_MAX
	};

	CInputMouse() : m_MouseState{}, m_MouseStateOld{}, m_bPress{}, m_bTrigger{}, m_bRelease{}, m_bRepeat{}, m_PressTime{}, m_Move{}, m_WheelMove{} {}
	~CInputMouse() = default;

	HRESULT Init(HINSTANCE hInstanse, HWND hWnd) override;
	void Uninit(void) override;
	void Update(void) override;

	bool GetPress(BUTTON button);
	bool GetTrigger(BUTTON button);
	bool GetRelease(BUTTON button);
	bool GetRepeat(BUTTON button);
	float* GetMove(void);
	float GetWheel(void);

// 非公開
private:
	static constexpr float MOUSE_INA = 15.0f;         // マウス移動量
	static constexpr float MOUSE_WHEEL_INA = 300.0f;  // マウスホイール移動量

	DIMOUSESTATE2 m_MouseState;	   // 状態
	DIMOUSESTATE2 m_MouseStateOld; // 前回状態

	bool m_bPress[BUTTON_MAX];	  // ボタン押下状態
	bool m_bTrigger[BUTTON_MAX]; // ボタン入力状態
	bool m_bRelease[BUTTON_MAX]; // ボタン離し状態
	bool m_bRepeat[BUTTON_MAX];  // ボタン長押し状態
	float m_Move[2];      // マウス移動量
	float m_WheelMove;    // ホイール移動量

	int m_PressTime[BUTTON_MAX];     // ボタン押下時間

};

#if 0
//-----------------------------
// コントローラークラス
//-----------------------------
class CInputController : public CInput
{
// 公開
public:
	//マウスボタンの種類
	typedef enum
	{
		LEFT = 0,
		RIGHT,
		SENTER,
		B1,
		B2,
		B3,
		B4,
		B5,
		MAX
	}BUTTOM;

	CInputController();
	~CInputController();

	HRESULT Init(HINSTANCE hInstanse, HWND hWnd) override;
	void Uninit(void) override;
	void Update(void) override;

	bool GetPress(BUTTOM button);
	bool GetTrigger(BUTTOM button);
	bool GetRelease(BUTTOM button);
	bool GetRepeat(BUTTOM button);
	float* GetMove(void);
	float GetWheel(void);

	// 非公開
private:
	static constexpr float m_MouseIna = 15.0f;        // マウス移動量
	static constexpr float m_MouseWheelIna = 300.0f;  // マウスホイール移動量

	DIMOUSESTATE2 m_MouseState;	   // 状態
	DIMOUSESTATE2 m_MouseStateOld; // 前回状態

	bool m_bPress[MAX];	  // ボタン押下状態
	bool m_bTrigger[MAX]; // ボタン入力状態
	bool m_bRelease[MAX]; // ボタン離し状態
	bool m_bRepeat[MAX];  // ボタン長押し状態
	float m_Move[2];      // マウス移動量
	float m_WheelMove;    // ホイール移動量

	int m_PressTime[MAX];     // ボタン押下時間

};

#define STICK_DED (0.1f)//デッドゾーン
#define VIBRATION_MAX (65535)//バイブレーション値

//POV
#define POV_UP (0.0f)//上
#define POV_RIGHTUP (4500.0f)//右上
#define POV_RIGHT (9000.0f)//右
#define POV_RIGHTDOWN (13500.0f)//右下
#define POV_DOWN (18000.0f)//下
#define POV_LEFTDOWN (22500.0f)//左下
#define POV_LEFT (27000.0f)//左
#define POV_LEFTUP (31500.0f)//左上

#define STICK_NUM (32767.0f)
#define DSTICK_NUM (-1.0f)
#define NUM_DIJOYKEY_MAX (32)//dinputボタンの最大数
#define POV_MAX (4)
#define POV_SYS_MAX (8)
#define POV_NUM (4500.0f)

//コントローラーの種類
typedef enum
{
	CONTROLLER_1 = 0,
	CONTROLLER_2,
	CONTROLLER_3,
	CONTROLLER_4,
	CONTROLLER_MAX
}CONTROLLER;

//コントローラータイプ
typedef enum
{
	CONTYPE_X = 0,
	CONTYPE_D,
	CONTYPE_MAX
}CONTYPE;

//Xパッドボタンの種類
typedef enum
{
	JOYKEY_UP = 0,
	JOYKEY_DOWN,
	JOYKEY_LEFT,
	JOYKEY_RIGHT,
	JOYKEY_START,
	JOYKEY_BACK,
	JOYKEY_L3,
	JOYKEY_R3,
	JOYKEY_LB,
	JOYKEY_RB,
	JOYKEY_LT,
	JOYKEY_RT,
	JOYKEY_A,
	JOYKEY_B,
	JOYKEY_X,
	JOYKEY_Y,
	JOYKEY_MAX
}JOYKEY;

//ELEパッドボタンの種類
typedef enum
{
	ELEKEY_A = 0,
	ELEKEY_B,
	ELEKEY_X,
	ELEKEY_Y,
	ELEKEY_LB,
	ELEKEY_RB,
	ELEKEY_LT,
	ELEKEY_RT,
	ELEKEY_L3,
	ELEKEY_R3,
	ELEKEY_BACK,
	ELEKEY_START,
	ELEKEY_SENTER,
	ELEKEY_MAX
}ELEKEY;

//PSパッドボタンの種類
typedef enum
{
	PSKEY_SQ = 0,
	PSKEY_CR,
	PSKEY_CI,
	PSKEY_TRA,
	PSKEY_LB,
	PSKEY_RB,
	PSKEY_LT,
	PSKEY_RT,
	PSKEY_BACK,
	PSKEY_START,
	PSKEY_L3,
	PSKEY_R3,
	PSKEY_SENTER,
	PSKEY_PAD,
	PSKEY_MAX
}PSKEY;

//NINパッドボタンの種類
typedef enum
{
	NINKEY_B = 0,
	NINKEY_A,
	NINKEY_Y,
	NINKEY_X,
	NINKEY_LB,
	NINKEY_RB,
	NINKEY_LT,
	NINKEY_RT,
	NINKEY_－,
	NINKEY_＋,
	NINKEY_L3,
	NINKEY_R3,
	NINKEY_HOME,
	NINKEY_CAP,
	NINKEY_MAX
}NINKEY;

//Dパッドボタンの種類
typedef enum
{
	DKEY_A = 0,
	DKEY_B,
	DKEY_X,
	DKEY_Y,
	DKEY_LB,
	DKEY_RB,
	DKEY_LT,
	DKEY_RT,
	DKEY_L3,
	DKEY_R3,
	DKEY_BACK,
	DKEY_START,
	DKEY_SENTER,
	DKEY_B1,
	DKEY_B2,
	DKEY_MAX
}DKEY;

//パッドスティックの種類
typedef enum
{
	STICK_LEFT = 0,
	STICK_RIGHT,
	STICK_MAX
}STICK;

//スティック方向の種類
typedef enum
{
	DIRESTICK_UP = 0,
	DIRESTICK_DOWN,
	DIRESTICK_LEFT,
	DIRESTICK_RIGHT,
	DIRESTICK_MAX
}DIRESTICK;

//Xinput
HRESULT InitJoypad(void);//初期化処理
void UninitJoypad(void);//終了処理
void UpdateJoypad(void);//更新処理
bool GetJoykeyPress(JOYKEY key, CONTROLLER Controller);//キー押下処理
bool GetJoykeyTrigger(JOYKEY key, CONTROLLER Controller);//キー入力処理
bool GetJoykeyRelease(JOYKEY key, CONTROLLER Controller);//キー離し処理
bool GetJoykeyRepeat(JOYKEY key, CONTROLLER Controller);//キー長押し処理
float *GetJoyStick(STICK Stick, CONTROLLER Controller);//スティック処理
bool JoyStickPress(DIRESTICK DireStick ,STICK Stick, CONTROLLER Controller);//スティック押下処理
bool JoyStickTrigger(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller);//スティック入力処理
bool JoyStickRelease(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller);//スティック離し処理
bool JoyStickRepeat(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller);//スティック長押し処理
void VibrateController(WORD wLeftMotorSpeed, WORD wRightMotorSpeed, CONTROLLER Controller);//バイブレーション処理

//dinputパッド
HRESULT InitdJoypad(HINSTANCE hInstanse, HWND hWnd);//初期化処理
void UninitdJoypad(void);//終了処理
void UpdatedJoypad(void);//更新処理
bool GetdJoykeyPress(int nkey, CONTROLLER Controller);//キー押下処理
bool GetdJoykeyTrigger(int nkey, CONTROLLER Controller);//キー入力処理
bool GetdJoykeyRelease(int nkey, CONTROLLER Controller);//キー離し処理
bool GetdJoykeyRepeat(int nkey, CONTROLLER Controller);//キー長押し処理
float* GetdJoyStick(STICK Stick, CONTROLLER Controller);//スティック処理
bool dJoyStickPress(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller);//スティック処理
bool dJoyStickTrigger(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller);//スティック入力処理
bool dJoyStickRelease(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller);//スティック離し処理
bool dJoyStickRepeat(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller);//スティック長押し処理
float GetdJoySlider(int nSlider, CONTROLLER Controller);//スライダー軸処理
bool GetdJoyPov(float fData,int nPov, CONTROLLER Controller);//POV押下処理
bool GetdJoyPovTrigger(float fData, int nPov, CONTROLLER Controller);//POV入力処理
bool GetdJoyPovRelease(float fData, int nPov, CONTROLLER Controller);//POV離し処理
bool GetdJoyPovRepeat(float fData, int nPov, CONTROLLER Controller);//POV長押し処理
void VibratedController(LONG lMotorPower, CONTROLLER Controller);//バイブレーション処理
void VibratedControllerStop(CONTROLLER Controller);//バイブレーション停止処理

void SetVibrate(float fPower, CONTROLLER Controller);//バイブレーション設定

//接続確認
bool IsXInputControllerConnected(CONTROLLER Controller);//Xinput接続確認
bool IsDirectInputControllerConnected(CONTROLLER Controller);//dinpit接続確認
char *ControllerName(CONTROLLER Controller);//コントローラーインスタンスネーム取得
int ControllerNum(CONTYPE Contype);//コントローラー数取得
BOOL CALLBACK EnumDevicesCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);//コントローラー列挙コールバック関数
int XNum(void);//Xinput接続数取得

LPDIRECTINPUT8 GetInput(void);//入力ポインタ取得
void SetUnCon(bool bUnCon);

void InputReSet(void);

const DIDEVICEINSTANCE* GetDiInstance(CONTROLLER Controller);
XINPUT_STATE GetXstate(CONTROLLER Controller);
DIJOYSTATE Getdstate(CONTROLLER Controller);

#endif _INPUT_H_

#endif