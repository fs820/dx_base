//---------------------------------------
//
// ���͏����̒�` [input.h]
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
// �C���v�b�g�N���X
//-----------------------------
class CInput
{
// ���J
public:
	CInput() : m_pDevice{} {};
	virtual ~CInput() = default;

	virtual HRESULT Init(HINSTANCE hInstanse, HWND hWnd);
	virtual void Uninit(void);
	virtual void Update(void) = 0;

// �Ƒ����J
protected:
	static constexpr int RELEASE_TIME = 50; //���ߎ���
	static constexpr int REPEAT_TIME = 40;  //����������

	static LPDIRECTINPUT8 m_pInput; // DirectInput�C���^�[�t�F�[�X
	LPDIRECTINPUTDEVICE8 m_pDevice;	// �f�o�C�X�C���^�[�t�F�[�X
};

//-----------------------------
// �L�[�{�[�h�N���X
//-----------------------------
class CInputKeyboard : public CInput
{
// ���J
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

// ����J
private:
	static constexpr size_t MAX_KEY = 256; // �L�[��

	BYTE m_KeyState[MAX_KEY];	  // �L�[���
	BYTE m_KeyStateOld[MAX_KEY];  // �O��L�[���

	bool m_bKeyPress[MAX_KEY];	  // �L�[�������
	bool m_bKeyTrigger[MAX_KEY];  // �L�[���͏��
	bool m_bKeyRelease[MAX_KEY];  // �L�[�������
	bool m_bKeyRepeat[MAX_KEY];   // �L�[���������

	int m_KeyPressTime[MAX_KEY];  // �L�[��������
};

//-----------------------------
// �L�[�{�[�h�N���X
//-----------------------------
class CInputdInputKeyboard : public CInputKeyboard
{
// ���J
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

// ����J
private:
	static constexpr size_t MAX_KEY = 256; // �L�[��

	BYTE m_KeyState[MAX_KEY];	  // �L�[���
	BYTE m_KeyStateOld[MAX_KEY];  // �O��L�[���

	bool m_bKeyPress[MAX_KEY];	  // �L�[�������
	bool m_bKeyTrigger[MAX_KEY];  // �L�[���͏��
	bool m_bKeyRelease[MAX_KEY];  // �L�[�������
	bool m_bKeyRepeat[MAX_KEY];   // �L�[���������

	int m_KeyPressTime[MAX_KEY];  // �L�[��������
};

//-----------------------------
// �L�[�{�[�h�N���X
//-----------------------------
class CInputRawInputKeyboard : public CInputKeyboard
{
// ���J
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

// ����J
private:
	static constexpr size_t MAX_KEY = 256; // �L�[��

	BYTE m_KeyState[MAX_KEY];	  // �L�[���
	BYTE m_KeyStateOld[MAX_KEY];  // �O��L�[���

	bool m_bKeyPress[MAX_KEY];	  // �L�[�������
	bool m_bKeyTrigger[MAX_KEY];  // �L�[���͏��
	bool m_bKeyRelease[MAX_KEY];  // �L�[�������
	bool m_bKeyRepeat[MAX_KEY];   // �L�[���������

	int m_KeyPressTime[MAX_KEY];  // �L�[��������
};

//-----------------------------
// �}�E�X�N���X
//-----------------------------
class CInputMouse : public CInput
{
// ���J
public:
	//�}�E�X�{�^���̎��
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

// ����J
private:
	static constexpr float MOUSE_INA = 15.0f;         // �}�E�X�ړ���
	static constexpr float MOUSE_WHEEL_INA = 300.0f;  // �}�E�X�z�C�[���ړ���

	DIMOUSESTATE2 m_MouseState;	   // ���
	DIMOUSESTATE2 m_MouseStateOld; // �O����

	bool m_bPress[BUTTON_MAX];	  // �{�^���������
	bool m_bTrigger[BUTTON_MAX]; // �{�^�����͏��
	bool m_bRelease[BUTTON_MAX]; // �{�^���������
	bool m_bRepeat[BUTTON_MAX];  // �{�^�����������
	float m_Move[2];      // �}�E�X�ړ���
	float m_WheelMove;    // �z�C�[���ړ���

	int m_PressTime[BUTTON_MAX];     // �{�^����������

};

#if 0
//-----------------------------
// �R���g���[���[�N���X
//-----------------------------
class CInputController : public CInput
{
// ���J
public:
	//�}�E�X�{�^���̎��
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

	// ����J
private:
	static constexpr float m_MouseIna = 15.0f;        // �}�E�X�ړ���
	static constexpr float m_MouseWheelIna = 300.0f;  // �}�E�X�z�C�[���ړ���

	DIMOUSESTATE2 m_MouseState;	   // ���
	DIMOUSESTATE2 m_MouseStateOld; // �O����

	bool m_bPress[MAX];	  // �{�^���������
	bool m_bTrigger[MAX]; // �{�^�����͏��
	bool m_bRelease[MAX]; // �{�^���������
	bool m_bRepeat[MAX];  // �{�^�����������
	float m_Move[2];      // �}�E�X�ړ���
	float m_WheelMove;    // �z�C�[���ړ���

	int m_PressTime[MAX];     // �{�^����������

};

#define STICK_DED (0.1f)//�f�b�h�]�[��
#define VIBRATION_MAX (65535)//�o�C�u���[�V�����l

//POV
#define POV_UP (0.0f)//��
#define POV_RIGHTUP (4500.0f)//�E��
#define POV_RIGHT (9000.0f)//�E
#define POV_RIGHTDOWN (13500.0f)//�E��
#define POV_DOWN (18000.0f)//��
#define POV_LEFTDOWN (22500.0f)//����
#define POV_LEFT (27000.0f)//��
#define POV_LEFTUP (31500.0f)//����

#define STICK_NUM (32767.0f)
#define DSTICK_NUM (-1.0f)
#define NUM_DIJOYKEY_MAX (32)//dinput�{�^���̍ő吔
#define POV_MAX (4)
#define POV_SYS_MAX (8)
#define POV_NUM (4500.0f)

//�R���g���[���[�̎��
typedef enum
{
	CONTROLLER_1 = 0,
	CONTROLLER_2,
	CONTROLLER_3,
	CONTROLLER_4,
	CONTROLLER_MAX
}CONTROLLER;

//�R���g���[���[�^�C�v
typedef enum
{
	CONTYPE_X = 0,
	CONTYPE_D,
	CONTYPE_MAX
}CONTYPE;

//X�p�b�h�{�^���̎��
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

//ELE�p�b�h�{�^���̎��
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

//PS�p�b�h�{�^���̎��
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

//NIN�p�b�h�{�^���̎��
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
	NINKEY_�|,
	NINKEY_�{,
	NINKEY_L3,
	NINKEY_R3,
	NINKEY_HOME,
	NINKEY_CAP,
	NINKEY_MAX
}NINKEY;

//D�p�b�h�{�^���̎��
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

//�p�b�h�X�e�B�b�N�̎��
typedef enum
{
	STICK_LEFT = 0,
	STICK_RIGHT,
	STICK_MAX
}STICK;

//�X�e�B�b�N�����̎��
typedef enum
{
	DIRESTICK_UP = 0,
	DIRESTICK_DOWN,
	DIRESTICK_LEFT,
	DIRESTICK_RIGHT,
	DIRESTICK_MAX
}DIRESTICK;

//Xinput
HRESULT InitJoypad(void);//����������
void UninitJoypad(void);//�I������
void UpdateJoypad(void);//�X�V����
bool GetJoykeyPress(JOYKEY key, CONTROLLER Controller);//�L�[��������
bool GetJoykeyTrigger(JOYKEY key, CONTROLLER Controller);//�L�[���͏���
bool GetJoykeyRelease(JOYKEY key, CONTROLLER Controller);//�L�[��������
bool GetJoykeyRepeat(JOYKEY key, CONTROLLER Controller);//�L�[����������
float *GetJoyStick(STICK Stick, CONTROLLER Controller);//�X�e�B�b�N����
bool JoyStickPress(DIRESTICK DireStick ,STICK Stick, CONTROLLER Controller);//�X�e�B�b�N��������
bool JoyStickTrigger(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller);//�X�e�B�b�N���͏���
bool JoyStickRelease(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller);//�X�e�B�b�N��������
bool JoyStickRepeat(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller);//�X�e�B�b�N����������
void VibrateController(WORD wLeftMotorSpeed, WORD wRightMotorSpeed, CONTROLLER Controller);//�o�C�u���[�V��������

//dinput�p�b�h
HRESULT InitdJoypad(HINSTANCE hInstanse, HWND hWnd);//����������
void UninitdJoypad(void);//�I������
void UpdatedJoypad(void);//�X�V����
bool GetdJoykeyPress(int nkey, CONTROLLER Controller);//�L�[��������
bool GetdJoykeyTrigger(int nkey, CONTROLLER Controller);//�L�[���͏���
bool GetdJoykeyRelease(int nkey, CONTROLLER Controller);//�L�[��������
bool GetdJoykeyRepeat(int nkey, CONTROLLER Controller);//�L�[����������
float* GetdJoyStick(STICK Stick, CONTROLLER Controller);//�X�e�B�b�N����
bool dJoyStickPress(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller);//�X�e�B�b�N����
bool dJoyStickTrigger(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller);//�X�e�B�b�N���͏���
bool dJoyStickRelease(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller);//�X�e�B�b�N��������
bool dJoyStickRepeat(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller);//�X�e�B�b�N����������
float GetdJoySlider(int nSlider, CONTROLLER Controller);//�X���C�_�[������
bool GetdJoyPov(float fData,int nPov, CONTROLLER Controller);//POV��������
bool GetdJoyPovTrigger(float fData, int nPov, CONTROLLER Controller);//POV���͏���
bool GetdJoyPovRelease(float fData, int nPov, CONTROLLER Controller);//POV��������
bool GetdJoyPovRepeat(float fData, int nPov, CONTROLLER Controller);//POV����������
void VibratedController(LONG lMotorPower, CONTROLLER Controller);//�o�C�u���[�V��������
void VibratedControllerStop(CONTROLLER Controller);//�o�C�u���[�V������~����

void SetVibrate(float fPower, CONTROLLER Controller);//�o�C�u���[�V�����ݒ�

//�ڑ��m�F
bool IsXInputControllerConnected(CONTROLLER Controller);//Xinput�ڑ��m�F
bool IsDirectInputControllerConnected(CONTROLLER Controller);//dinpit�ڑ��m�F
char *ControllerName(CONTROLLER Controller);//�R���g���[���[�C���X�^���X�l�[���擾
int ControllerNum(CONTYPE Contype);//�R���g���[���[���擾
BOOL CALLBACK EnumDevicesCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext);//�R���g���[���[�񋓃R�[���o�b�N�֐�
int XNum(void);//Xinput�ڑ����擾

LPDIRECTINPUT8 GetInput(void);//���̓|�C���^�擾
void SetUnCon(bool bUnCon);

void InputReSet(void);

const DIDEVICEINSTANCE* GetDiInstance(CONTROLLER Controller);
XINPUT_STATE GetXstate(CONTROLLER Controller);
DIJOYSTATE Getdstate(CONTROLLER Controller);

#endif _INPUT_H_

#endif