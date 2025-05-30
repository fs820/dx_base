//---------------------------------------
//
//���͏���[input.cpp]
//Author fuma sato
//
//---------------------------------------

#include"input.h"

#if 0
DIJOYSTATE g_djoykeyState[CONTROLLER_MAX] = { NULL };//d�p�b�h���
LPDIRECTINPUTEFFECT g_djoyEffect[CONTROLLER_MAX] = { NULL };
char g_ConName[CONTROLLER_MAX][64] = { NULL };
int g_XcontrollerNum = 0;
int g_DcontrollerNum = 0;
bool g_bUnCon = false;
bool g_bX = false;
const DIDEVICEINSTANCE* g_pDiInstance[CONTROLLER_MAX];
#endif

//-------------------------------
//
// �C���v�b�g��{�N���X
//
//-------------------------------

// �ÓI�����o�ϐ��̒�`
LPDIRECTINPUT8 CInput::m_pInput = NULL; // DirectInput�I�u�W�F�N�g

//-----------------
// ������
//-----------------
HRESULT CInput::Init(HINSTANCE hInstanse, HWND hWnd)
{
	//�C���v�b�g�I�u�W�F�N�g�̍쐬
	if (m_pInput == NULL)
	{
		if (FAILED(DirectInput8Create
		(
			hInstanse,
			DIRECTINPUT_VERSION, IID_IDirectInput8,
			(void**)&m_pInput,
			NULL
		)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//-----------------
// �I��
//-----------------
void CInput::Uninit(void)
{
	//�C���v�b�g�̔j��
	if (m_pInput != NULL)
	{
		m_pInput->Release();
		m_pInput = NULL;
	}
}

//--------------------------
//
//�L�[�{�[�h
//
//--------------------------

//--------------------
//����������
//--------------------
HRESULT CInputKeyboard::Init(HINSTANCE hInstanse, HWND hWnd)
{
	//�C���v�b�g�I�u�W�F�N�g�̍쐬
	CInput::Init(hInstanse, hWnd);

	//�C���v�b�g�f�o�C�X�̍쐬
	if (FAILED(m_pInput->CreateDevice(GUID_SysKeyboard, &m_pDevice, NULL)))
	{
		return E_FAIL;
	}

	//�t�H�[�}�b�g�쐬
	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIKeyboard)))
	{
		return E_FAIL;
	}

	// �o�b�t�@�����O��L���ɂ���
	if (FAILED(m_pDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	//�A�N�Z�X�����̎擾
	if (FAILED(m_pDevice->Acquire()))
	{
		return E_FAIL;
	}

	return S_OK;
}

//----------------------
//�I������
//----------------------
void CInputKeyboard::Uninit(void)
{
	// �f�o�C�X�̔j��
	if (m_pDevice != NULL)
	{
		m_pDevice->Unacquire();
		m_pDevice->Release();
		m_pDevice = NULL;
	}

	// �C���v�b�g�̔j��
	CInput::Uninit();
}

//-------------------------
//�X�V����
//-------------------------
void CInputKeyboard::Update(void)
{
	for (size_t nCntkey = 0; nCntkey < MAX_KEY; nCntkey++)
	{
		m_KeyStateOld[nCntkey] = m_KeyState[nCntkey]; //�O��̃L�[����ۑ�
	}

	BYTE KeyState[MAX_KEY];//�ꎞ���͊i�[
	if (SUCCEEDED(m_pDevice->GetDeviceState(sizeof(KeyState), KeyState)))
	{
		for (size_t nCntkey = 0; nCntkey < MAX_KEY; nCntkey++)
		{
			m_KeyState[nCntkey] = KeyState[nCntkey]; //�ꎞ�I�ȏ��𐳋K���ɓn��

			bool now = (m_KeyState[nCntkey] & 0x80) != 0;    // �������Ă��邩�ǂ���
			bool old = (m_KeyStateOld[nCntkey] & 0x80) != 0; // �O�񉟂��Ă������ǂ���
			
			m_bKeyPress[nCntkey] = now;           // ��
			m_bKeyTrigger[nCntkey] = now && !old; // ����
			m_bKeyRelease[nCntkey] = (!now && old && m_KeyPressTime[nCntkey] >= RELEASE_TIME) ? true : false;

			if (m_bKeyTrigger[nCntkey] || m_bKeyRelease[nCntkey])
			{
				m_KeyPressTime[nCntkey] = 0;
			}
			else if (m_bKeyPress[nCntkey])
			{
				m_KeyPressTime[nCntkey]++;
			}

			m_bKeyRepeat[nCntkey] = (now && m_KeyPressTime[nCntkey] >= REPEAT_TIME) ? true : false;
		}
	}
	else
	{
		m_pDevice->Acquire();//�A�N�Z�X�����̎擾
	}
}

//------------------------------
//�L�[��������
//------------------------------
bool CInputKeyboard::GetPress(size_t key)
{
	return m_bKeyPress[key];
}

//------------------------------
//�L�[���͎��̏���
//------------------------------
bool CInputKeyboard::GetTrigger(size_t key)
{
	return m_bKeyTrigger[key];
}

//------------------------------
//�L�[�b�������ۂ̏���
//------------------------------
bool CInputKeyboard::GetRelease(size_t key)
{
	return m_bKeyRelease[key];
}

//------------------------------
//�L�[����������
//------------------------------
bool CInputKeyboard::GetRepeat(size_t key)
{
	return m_bKeyRepeat[key];
}

//--------------------------
//
//�}�E�X
//
//--------------------------

//--------------------
//����������
//--------------------
HRESULT CInputMouse::Init(HINSTANCE hInstanse, HWND hWnd)
{
	// �C���v�b�g�I�u�W�F�N�g�̐���
	CInput::Init(hInstanse, hWnd);

	//�C���v�b�g�f�o�C�X�̍쐬
	if (FAILED(m_pInput->CreateDevice(GUID_SysMouse, &m_pDevice, NULL)))
	{
		return E_FAIL;
	}

	//�t�H�[�}�b�g�쐬
	if (FAILED(m_pDevice->SetDataFormat(&c_dfDIMouse2)))
	{
		return E_FAIL;
	}

	// �o�b�t�@�����O��L���ɂ���
	if (FAILED(m_pDevice->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_NONEXCLUSIVE))))
	{
		return E_FAIL;
	}

	m_pDevice->Acquire();//�A�N�Z�X�����̎擾

	return S_OK;
}

//----------------------
//�I������
//----------------------
void CInputMouse::Uninit(void)
{
	// �f�o�C�X�̔j��
	if (m_pDevice != NULL)
	{
		m_pDevice->Unacquire();
		m_pDevice->Release();
		m_pDevice = NULL;
	}

	// �C���v�b�g�̔j��
	CInput::Uninit();
}

//-------------------------
//�X�V����
//-------------------------
void CInputMouse::Update(void)
{
	m_MouseStateOld = m_MouseState; //�O��̃}�E�X����ۑ�

	DIMOUSESTATE2 MouseState;//�}�E�X���//�ꎞ���͊i�[
	if (SUCCEEDED(m_pDevice->GetDeviceState(sizeof(DIMOUSESTATE2), &MouseState)))
	{
		m_MouseState = MouseState;//�ꎞ�I�ȏ��𐳋K���ɓn��

		for (size_t nCntButton = 0; nCntButton < BUTTON_MAX; nCntButton++)
		{
			bool now = (m_MouseState.rgbButtons[nCntButton] & 0x80) != 0;    // �������Ă��邩�ǂ���
			bool old = (m_MouseStateOld.rgbButtons[nCntButton] & 0x80) != 0; // �O�񉟂��Ă������ǂ���

			m_bPress[nCntButton] = now;           // ��
			m_bTrigger[nCntButton] = now && !old; // ����
			m_bRelease[nCntButton] = (!now && old && m_PressTime[nCntButton] >= RELEASE_TIME) ? true : false;

			if (m_bTrigger[nCntButton] || m_bRelease[nCntButton])
			{
				m_PressTime[nCntButton] = 0;
			}
			else if (m_bPress[nCntButton])
			{
				m_PressTime[nCntButton]++;
			}

			m_bRepeat[nCntButton] = (now && m_PressTime[nCntButton] >= REPEAT_TIME) ? true : false;

			m_Move[0] = (float)m_MouseState.lX;
			m_Move[1] = (float)m_MouseState.lY;
			m_Move[0] /= MOUSE_INA;
			m_Move[1] /= MOUSE_INA;

			m_WheelMove = (float)m_MouseState.lZ;
			m_WheelMove /= MOUSE_WHEEL_INA;
		}
	}
	else
	{
		m_pDevice->Acquire();//�A�N�Z�X�����̎擾
	}
}

//------------------------------
//�L�[��������
//------------------------------
bool CInputMouse::GetPress(BUTTON button)
{
	return m_bPress[button];
}

//------------------------------
//�L�[���͎��̏���
//------------------------------
bool CInputMouse::GetTrigger(BUTTON button)
{
	return m_bTrigger[button];
}

//------------------------------
//�L�[�b�������ۂ̏���
//------------------------------
bool CInputMouse::GetRelease(BUTTON button)
{
	return m_bRelease[button];
}

//------------------------------
//�L�[����������
//------------------------------
bool CInputMouse::GetRepeat(BUTTON button)
{
	return m_bRepeat[button];
}

//--------------
//�}�E�X�ړ�
//--------------
float* CInputMouse::GetMove(void)
{
	return m_Move;
}

//----------------
//�L�[��������
//----------------
float CInputMouse::GetWheel(void)
{
	return m_WheelMove;
}

#if 0
//--------------------------
//
//�p�b�h
//
//--------------------------

//--------------------------------
//����������
//--------------------------------
HRESULT InitJoypad(void)
{
	memset(&g_joykeyState,0,sizeof(XINPUT_STATE)* CONTROLLER_MAX);

	XInputEnable(true);

	return S_OK;
}

//------------------------
//�I������
//------------------------
void UninitJoypad(void)
{
	XInputEnable(false);
}

//------------------------
//�X�V����
//------------------------
void UpdateJoypad(void)
{
	XINPUT_STATE joykeyState[CONTROLLER_MAX];
	int ControllerNum = 0;

	for (int i = 0; i < CONTROLLER_MAX; i++)
	{
		if (XInputGetState(i, &joykeyState[i]) == ERROR_SUCCESS)
		{
			g_joykeyState[i] = joykeyState[i];
		}
		else
		{
			ControllerNum++;
		}
	}

	g_XcontrollerNum = CONTROLLER_MAX - ControllerNum;
}

//--------------------------
//�L�[��������
//--------------------------
bool GetJoykeyPress(JOYKEY key, CONTROLLER Controller)
{
	BYTE Trigger = 0;

	if (Controller==CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			if (key == JOYKEY_LT || key == JOYKEY_RT)
			{
				if (key == JOYKEY_LT)
				{
					Trigger = g_joykeyState[i].Gamepad.bLeftTrigger;
				}
				else
				{
					Trigger = g_joykeyState[i].Gamepad.bRightTrigger;
				}
			}
			
			if (g_joykeyState[i].Gamepad.wButtons & (0x01 << key) || Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD)
			{
				return true;
			}
		}

		return false;
	}

	if (key==JOYKEY_LT||key==JOYKEY_RT)
	{
		if (key == JOYKEY_LT)
		{
			Trigger = g_joykeyState[Controller].Gamepad.bLeftTrigger;
		}
		else
		{
			Trigger = g_joykeyState[Controller].Gamepad.bRightTrigger;
		}
	}

	return (g_joykeyState[Controller].Gamepad.wButtons & (0x01 << key)|| Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD) ? true: false;
}

//--------------------------
//�L�[���͏���
//--------------------------
bool GetJoykeyTrigger(JOYKEY key, CONTROLLER Controller)
{
	static bool bOldJoy[CONTROLLER_MAX][JOYKEY_MAX] = { false };
	BYTE Trigger = 0;

	if (Controller == CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			if (key == JOYKEY_LT || key == JOYKEY_RT)
			{
				if (key == JOYKEY_LT)
				{
					Trigger = g_joykeyState[i].Gamepad.bLeftTrigger;
				}
				else
				{
					Trigger = g_joykeyState[i].Gamepad.bRightTrigger;
				}
			}

			if (bOldJoy[i][key] == false && (((g_joykeyState[i].Gamepad.wButtons & (0x01 << key)) ? true : false) == true || Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD))
			{
				bOldJoy[i][key] = true;
				return true;
			}
			else if (bOldJoy[i][key] == true && (((g_joykeyState[i].Gamepad.wButtons & (0x01 << key)) ? true : false) == false && Trigger <= XINPUT_GAMEPAD_TRIGGER_THRESHOLD))
			{
				bOldJoy[i][key] = false;
			}
		}

		return false;
	}

	if (key == JOYKEY_LT || key == JOYKEY_RT)
	{
		if (key == JOYKEY_LT)
		{
			Trigger = g_joykeyState[Controller].Gamepad.bLeftTrigger;
		}
		else
		{
			Trigger = g_joykeyState[Controller].Gamepad.bRightTrigger;
		}
	}

	if (bOldJoy[Controller][key] == false && (((g_joykeyState[Controller].Gamepad.wButtons & (0x01 << key)) ? true : false) == true|| Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD))
	{
		bOldJoy[Controller][key] = true;
		return true;
	}
	else if (bOldJoy[Controller][key] == true && (((g_joykeyState[Controller].Gamepad.wButtons & (0x01 << key)) ? true : false) == false&& Trigger <= XINPUT_GAMEPAD_TRIGGER_THRESHOLD))
	{
		bOldJoy[Controller][key] = false;
		return false;
	}
	else
	{
		return false;
	}
}


//----------------------------
//�L�[��������
//----------------------------
bool GetJoykeyRelease(JOYKEY key, CONTROLLER Controller)
{
	static bool bOldJoy[CONTROLLER_MAX][JOYKEY_MAX] = { false };
	static int KeyPutTime[CONTROLLER_MAX][JOYKEY_MAX] = { 0 };
	BYTE Trigger = 0;

	if (Controller == CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			if (key == JOYKEY_LT || key == JOYKEY_RT)
			{
				if (key == JOYKEY_LT)
				{
					Trigger = g_joykeyState[i].Gamepad.bLeftTrigger;
				}
				else
				{
					Trigger = g_joykeyState[i].Gamepad.bRightTrigger;
				}
			}

			if (bOldJoy[i][key] == true && (((g_joykeyState[i].Gamepad.wButtons & (0x01 << key)) ? true : false) == false && Trigger <= XINPUT_GAMEPAD_TRIGGER_THRESHOLD))
			{
				bOldJoy[i][key] = false;
				if (KeyPutTime[i][key] >= RELEASE_TIME)
				{
					KeyPutTime[i][key] = 0;
					return true;
				}
				else
				{
					KeyPutTime[i][key] = 0;
				}
			}
			else if (bOldJoy[i][key] == false && (((g_joykeyState[i].Gamepad.wButtons & (0x01 << key)) ? true : false) == true || Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD))
			{
				bOldJoy[i][key] = true;
			}
			else if (bOldJoy[i][key] == true && (((g_joykeyState[i].Gamepad.wButtons & (0x01 << key)) ? true : false) == true || Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD))
			{
				KeyPutTime[i][key]++;
			}
		}

		return false;
	}

	if (key == JOYKEY_LT || key == JOYKEY_RT)
	{
		if (key == JOYKEY_LT)
		{
			Trigger = g_joykeyState[Controller].Gamepad.bLeftTrigger;
		}
		else
		{
			Trigger = g_joykeyState[Controller].Gamepad.bRightTrigger;
		}
	}

	if (bOldJoy[Controller][key] == true && (((g_joykeyState[Controller].Gamepad.wButtons & (0x01 << key)) ? true : false) == false && Trigger <= XINPUT_GAMEPAD_TRIGGER_THRESHOLD))
	{
		bOldJoy[Controller][key] = false;
		if (KeyPutTime[Controller][key] >= RELEASE_TIME)
		{
			KeyPutTime[Controller][key] = 0;
			return true;
		}
		else
		{
			KeyPutTime[Controller][key] = 0;
			return false;
		}
	}
	else if (bOldJoy[Controller][key] == false && (((g_joykeyState[Controller].Gamepad.wButtons & (0x01 << key)) ? true : false) == true || Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD))
	{
		bOldJoy[Controller][key] = true;
		return false;
	}
	else if (bOldJoy[Controller][key] == true && (((g_joykeyState[Controller].Gamepad.wButtons & (0x01 << key)) ? true : false) == true || Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD))
	{
		KeyPutTime[Controller][key]++;
		return false;
	}
	else
	{
		return false;
	}
}

//----------------------------
//�L�[����������
//----------------------------
bool GetJoykeyRepeat(JOYKEY key, CONTROLLER Controller)
{
	static bool bOldJoy[CONTROLLER_MAX][JOYKEY_MAX] = { false };
	static int KeyPutTime[CONTROLLER_MAX][JOYKEY_MAX] = { 0 };
	BYTE Trigger = 0;

	if (Controller == CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			if (key == JOYKEY_LT || key == JOYKEY_RT)
			{
				if (key == JOYKEY_LT)
				{
					Trigger = g_joykeyState[i].Gamepad.bLeftTrigger;
				}
				else
				{
					Trigger = g_joykeyState[i].Gamepad.bRightTrigger;
				}
			}

			if (bOldJoy[i][key] == true && (((g_joykeyState[i].Gamepad.wButtons & (0x01 << key)) ? true : false) == true || Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD))
			{
				return true;
			}
			else if (bOldJoy[i][key] == false && (((g_joykeyState[i].Gamepad.wButtons & (0x01 << key)) ? true : false) == true || Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD))
			{
				KeyPutTime[i][key]++;

				if (KeyPutTime[i][key] >= REPEAT_TIME)
				{
					KeyPutTime[i][key] = 0;
					bOldJoy[i][key] = true;
				}
			}
			else
			{
				bOldJoy[i][key] = false;
				KeyPutTime[i][key] = 0;
			}
		}

		return false;
	}

	if (key == JOYKEY_LT || key == JOYKEY_RT)
	{
		if (key == JOYKEY_LT)
		{
			Trigger = g_joykeyState[Controller].Gamepad.bLeftTrigger;
		}
		else
		{
			Trigger = g_joykeyState[Controller].Gamepad.bRightTrigger;
		}
	}

	if (bOldJoy[Controller][key] == true && (((g_joykeyState[Controller].Gamepad.wButtons & (0x01 << key)) ? true : false) == true || Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD))
	{
		return true;
	}
	else if (bOldJoy[Controller][key] == false && (((g_joykeyState[Controller].Gamepad.wButtons & (0x01 << key)) ? true : false) == true || Trigger > XINPUT_GAMEPAD_TRIGGER_THRESHOLD))
	{
		KeyPutTime[Controller][key]++;

		if (KeyPutTime[Controller][key] >= REPEAT_TIME)
		{
			KeyPutTime[Controller][key] = 0;
			bOldJoy[Controller][key] = true;
		}
		return false;
	}
	else
	{
		bOldJoy[Controller][key] = false;
		KeyPutTime[Controller][key] = 0;
		return false;
	}
}

//-----------------
//�X�e�B�b�N����
//-----------------
float *GetJoyStick(STICK Stick, CONTROLLER Controller)
{
	static float fStick[2] = { 0.0f };

	switch (Stick)
	{
	case STICK_LEFT:
		fStick[0] = (float)g_joykeyState[Controller].Gamepad.sThumbLX;
		fStick[1] = (float)g_joykeyState[Controller].Gamepad.sThumbLY;
		break;
	case STICK_RIGHT:
		fStick[0] = (float)g_joykeyState[Controller].Gamepad.sThumbRX;
		fStick[1] = (float)g_joykeyState[Controller].Gamepad.sThumbRY;
		break;
	}

	fStick[0] /= STICK_NUM;
	fStick[1] /= STICK_NUM;

	return &fStick[0];
}

//-----------------------------
//
//-----------------------------
bool JoyStickPress(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller)
{
	float fStick = 0.0f;

	if (Controller==CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			switch (Stick)
			{
			case STICK_LEFT:
				switch (DireStick)
				{
				case DIRESTICK_UP:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbLY;
					break;
				case DIRESTICK_DOWN:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbLY;
					break;
				case DIRESTICK_LEFT:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbLX;
					break;
				case DIRESTICK_RIGHT:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbLX;
					break;
				}
				break;
			case STICK_RIGHT:
				switch (DireStick)
				{
				case DIRESTICK_UP:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbRY;
					break;
				case DIRESTICK_DOWN:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbRY;
					break;
				case DIRESTICK_LEFT:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbRX;
					break;
				case DIRESTICK_RIGHT:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbRX;
					break;
				}
				break;
			}

			fStick /= STICK_NUM;

			switch (DireStick)
			{
			case DIRESTICK_UP:
				if (fStick > STICK_DED)
				{
					return true;
				}
				break;
			case DIRESTICK_DOWN:
				if (fStick < -STICK_DED)
				{
					return true;
				}
				break;
			case DIRESTICK_LEFT:
				if (fStick < -STICK_DED)
				{
					return true;
				}
				break;
			case DIRESTICK_RIGHT:
				if (fStick > STICK_DED)
				{
					return true;
				}
				break;
			}
		}
		return false;
	}

	switch (Stick)
	{
	case STICK_LEFT:
		switch (DireStick)
		{
		case DIRESTICK_UP:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbLY;
			break;
		case DIRESTICK_DOWN:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbLY;
			break;
		case DIRESTICK_LEFT:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbLX;
			break;
		case DIRESTICK_RIGHT:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbLX;
			break;
		}
		break;
	case STICK_RIGHT:
		switch (DireStick)
		{
		case DIRESTICK_UP:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbRY;
			break;
		case DIRESTICK_DOWN:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbRY;
			break;
		case DIRESTICK_LEFT:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbRX;
			break;
		case DIRESTICK_RIGHT:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbRX;
			break;
		}
		break;
	}

	fStick /= STICK_NUM;

	switch (DireStick)
	{
	case DIRESTICK_UP:
		return fStick > STICK_DED;
		break;
	case DIRESTICK_DOWN:
		return fStick < -STICK_DED;
		break;
	case DIRESTICK_LEFT:
		return fStick < -STICK_DED;
		break;
	case DIRESTICK_RIGHT:
		return fStick > STICK_DED;
		break;
	}

	return false;
}

//-----------------------------
//
//-----------------------------
bool JoyStickTrigger(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller)
{
	float fStick = 0.0f;
	static bool bOldStick[CONTROLLER_MAX][STICK_MAX][DIRESTICK_MAX] = {false};

	if (Controller == CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			switch (Stick)
			{
			case STICK_LEFT:
				switch (DireStick)
				{
				case DIRESTICK_UP:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbLY;
					break;
				case DIRESTICK_DOWN:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbLY;
					break;
				case DIRESTICK_LEFT:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbLX;
					break;
				case DIRESTICK_RIGHT:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbLX;
					break;
				}
				break;
			case STICK_RIGHT:
				switch (DireStick)
				{
				case DIRESTICK_UP:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbRY;
					break;
				case DIRESTICK_DOWN:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbRY;
					break;
				case DIRESTICK_LEFT:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbRX;
					break;
				case DIRESTICK_RIGHT:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbRX;
					break;
				}
				break;
			}

			fStick /= STICK_NUM;

			switch (DireStick)
			{
			case DIRESTICK_UP:
				if (bOldStick[i][Stick][DireStick] == false && fStick > STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = true;
					return true;
				}
				else if (bOldStick[i][Stick][DireStick] == true && fStick <= STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = false;
				}
				break;
			case DIRESTICK_DOWN:
				if (bOldStick[i][Stick][DireStick] == false && fStick < -STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = true;
					return true;
				}
				else if (bOldStick[i][Stick][DireStick] == true && fStick >= -STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = false;
				}
					break;
			case DIRESTICK_LEFT:
				if (bOldStick[i][Stick][DireStick] == false && fStick < -STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = true;
					return true;
				}
				else if (bOldStick[i][Stick][DireStick] == true && fStick >= -STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = false;
				}
				break;
			case DIRESTICK_RIGHT:
				if (bOldStick[i][Stick][DireStick] == false && fStick > STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = true;
					return true;
				}
				else if (bOldStick[i][Stick][DireStick] == true && fStick <= STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = false;
				}
				break;
			}
		}
		return false;
	}


	switch (Stick)
	{
	case STICK_LEFT:
		switch (DireStick)
		{
		case DIRESTICK_UP:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbLY;
			break;
		case DIRESTICK_DOWN:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbLY;
			break;
		case DIRESTICK_LEFT:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbLX;
			break;
		case DIRESTICK_RIGHT:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbLX;
			break;
		}
		break;
	case STICK_RIGHT:
		switch (DireStick)
		{
		case DIRESTICK_UP:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbRY;
			break;
		case DIRESTICK_DOWN:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbRY;
			break;
		case DIRESTICK_LEFT:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbRX;
			break;
		case DIRESTICK_RIGHT:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbRX;
			break;
		}
		break;
	}

	fStick /= STICK_NUM;

	switch (DireStick)
	{
	case DIRESTICK_UP:
		if (bOldStick[Controller][Stick][DireStick] == false && fStick > STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = true;
			return true;
		}
		else if (bOldStick[Controller][Stick][DireStick] == true && fStick <= STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = false;
			return false;
		}
		else
		{
			return false;
		}
		break;
	case DIRESTICK_DOWN:
		if (bOldStick[Controller][Stick][DireStick] == false && fStick < -STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = true;
			return true;
		}
		else if (bOldStick[Controller][Stick][DireStick] == true && fStick >= -STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = false;
			return false;
		}
		else
		{
			return false;
		}
		break;
	case DIRESTICK_LEFT:
		if (bOldStick[Controller][Stick][DireStick] == false && fStick < -STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = true;
			return true;
		}
		else if (bOldStick[Controller][Stick][DireStick] == true && fStick >= -STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = false;
			return false;
		}
		else
		{
			return false;
		}
		break;
	case DIRESTICK_RIGHT:
		if (bOldStick[Controller][Stick][DireStick] == false && fStick > STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = true;
			return true;
		}
		else if (bOldStick[Controller][Stick][DireStick] == true && fStick <= STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = false;
			return false;
		}
		else
		{
			return false;
		}
		break;
	}

	return false;
}

//-----------------------------
//
//-----------------------------
bool JoyStickRelease(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller)
{
	float fStick = 0.0f;
	static bool bOldStick[CONTROLLER_MAX][STICK_MAX][DIRESTICK_MAX] = { false };
	static int nStickTime[CONTROLLER_MAX][STICK_MAX][DIRESTICK_MAX] = { 0 };

	if (Controller == CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			switch (Stick)
			{
			case STICK_LEFT:
				switch (DireStick)
				{
				case DIRESTICK_UP:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbLY;
					break;
				case DIRESTICK_DOWN:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbLY;
					break;
				case DIRESTICK_LEFT:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbLX;
					break;
				case DIRESTICK_RIGHT:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbLX;
					break;
				}
				break;
			case STICK_RIGHT:
				switch (DireStick)
				{
				case DIRESTICK_UP:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbRY;
					break;
				case DIRESTICK_DOWN:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbRY;
					break;
				case DIRESTICK_LEFT:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbRX;
					break;
				case DIRESTICK_RIGHT:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbRX;
					break;
				}
				break;
			}

			fStick /= STICK_NUM;

			switch (DireStick)
			{
			case DIRESTICK_UP:
				if (bOldStick[i][Stick][DireStick] == true && fStick <= STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = false;
					if (nStickTime[i][Stick][DireStick] >= RELEASE_TIME)
					{
						nStickTime[i][Stick][DireStick] = 0;
						return true;
					}
					else
					{
						nStickTime[i][Stick][DireStick] = 0;
					}
				}
				else if (bOldStick[i][Stick][DireStick] == false && fStick > STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = true;
				}
				else if (bOldStick[i][Stick][DireStick] == true && fStick > STICK_DED)
				{
					nStickTime[i][Stick][DireStick]++;
				}
				break;
			case DIRESTICK_DOWN:
				if (bOldStick[i][Stick][DireStick] == true && fStick >= -STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = false;
					if (nStickTime[i][Stick][DireStick] >= RELEASE_TIME)
					{
						nStickTime[i][Stick][DireStick] = 0;
						return true;
					}
					else
					{
						nStickTime[i][Stick][DireStick] = 0;
					}
				}
				else if (bOldStick[i][Stick][DireStick] == false && fStick < -STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = true;
				}
				else if (bOldStick[i][Stick][DireStick] == true && fStick < -STICK_DED)
				{
					nStickTime[i][Stick][DireStick]++;
				}
				break;
			case DIRESTICK_LEFT:
				if (bOldStick[i][Stick][DireStick] == true && fStick >= -STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = false;
					if (nStickTime[i][Stick][DireStick] >= RELEASE_TIME)
					{
						nStickTime[i][Stick][DireStick] = 0;
						return true;
					}
					else
					{
						nStickTime[i][Stick][DireStick] = 0;
					}
				}
				else if (bOldStick[i][Stick][DireStick] == false && fStick < -STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = true;
				}
				else if (bOldStick[i][Stick][DireStick] == true && fStick < -STICK_DED)
				{
					nStickTime[i][Stick][DireStick]++;
				}
				break;
			case DIRESTICK_RIGHT:
				if (bOldStick[i][Stick][DireStick] == true && fStick <= STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = false;
					if (nStickTime[i][Stick][DireStick] >= RELEASE_TIME)
					{
						nStickTime[i][Stick][DireStick] = 0;
						return true;
					}
					else
					{
						nStickTime[i][Stick][DireStick] = 0;
					}
				}
				else if (bOldStick[i][Stick][DireStick] == false && fStick > STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = true;
				}
				else if (bOldStick[i][Stick][DireStick] == true && fStick > STICK_DED)
				{
					nStickTime[i][Stick][DireStick]++;
				}
				break;
			}
		}
		return false;
	}


	switch (Stick)
	{
	case STICK_LEFT:
		switch (DireStick)
		{
		case DIRESTICK_UP:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbLY;
			break;
		case DIRESTICK_DOWN:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbLY;
			break;
		case DIRESTICK_LEFT:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbLX;
			break;
		case DIRESTICK_RIGHT:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbLX;
			break;
		}
		break;
	case STICK_RIGHT:
		switch (DireStick)
		{
		case DIRESTICK_UP:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbRY;
			break;
		case DIRESTICK_DOWN:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbRY;
			break;
		case DIRESTICK_LEFT:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbRX;
			break;
		case DIRESTICK_RIGHT:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbRX;
			break;
		}
		break;
	}

	fStick /= STICK_NUM;

	switch (DireStick)
	{
	case DIRESTICK_UP:
		if (bOldStick[Controller][Stick][DireStick] == true && fStick <= STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = false;
			if (nStickTime[Controller][Stick][DireStick] >= RELEASE_TIME)
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				return true;
			}
			else
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				return false;
			}
		}
		else if (bOldStick[Controller][Stick][DireStick] == false && fStick > STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = true;
			return false;
		}
		else if (bOldStick[Controller][Stick][DireStick] == true && fStick > STICK_DED)
		{
			nStickTime[Controller][Stick][DireStick]++;
			return false;
		}
		else
		{
			return false;
		}
		break;
	case DIRESTICK_DOWN:
		if (bOldStick[Controller][Stick][DireStick] == true && fStick >= -STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = false;
			if (nStickTime[Controller][Stick][DireStick] >= RELEASE_TIME)
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				return true;
			}
			else
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				return false;
			}
		}
		else if (bOldStick[Controller][Stick][DireStick] == false && fStick < -STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = true;
			return false;
		}
		else if (bOldStick[Controller][Stick][DireStick] == true && fStick < -STICK_DED)
		{
			nStickTime[Controller][Stick][DireStick]++;
			return false;
		}
		else
		{
			return false;
		}
		break;
	case DIRESTICK_LEFT:
		if (bOldStick[Controller][Stick][DireStick] == true && fStick >= -STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = false;
			if (nStickTime[Controller][Stick][DireStick] >= RELEASE_TIME)
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				return true;
			}
			else
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				return false;
			}
		}
		else if (bOldStick[Controller][Stick][DireStick] == false && fStick < -STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = true;
			return false;
		}
		else if (bOldStick[Controller][Stick][DireStick] == true && fStick < -STICK_DED)
		{
			nStickTime[Controller][Stick][DireStick]++;
			return false;
		}
		else
		{
			return false;
		}
		break;
	case DIRESTICK_RIGHT:
		if (bOldStick[Controller][Stick][DireStick] == true && fStick <= STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = false;
			if (nStickTime[Controller][Stick][DireStick] >= RELEASE_TIME)
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				return true;
			}
			else
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				return false;
			}
		}
		else if (bOldStick[Controller][Stick][DireStick] == false && fStick > STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = true;
			return false;
		}
		else if (bOldStick[Controller][Stick][DireStick] == true && fStick > STICK_DED)
		{
			nStickTime[Controller][Stick][DireStick]++;
			return false;
		}
		else
		{
			return false;
		}
		break;
	}

	return false;
}

//-----------------------------
//
//-----------------------------
bool JoyStickRepeat(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller)
{
	float fStick = 0.0f;
	static bool bOldStick[CONTROLLER_MAX][STICK_MAX][DIRESTICK_MAX] = { false };
	static int nStickTime[CONTROLLER_MAX][STICK_MAX][DIRESTICK_MAX] = { 0 };

	if (Controller == CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			switch (Stick)
			{
			case STICK_LEFT:
				switch (DireStick)
				{
				case DIRESTICK_UP:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbLY;
					break;
				case DIRESTICK_DOWN:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbLY;
					break;
				case DIRESTICK_LEFT:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbLX;
					break;
				case DIRESTICK_RIGHT:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbLX;
					break;
				}
				break;
			case STICK_RIGHT:
				switch (DireStick)
				{
				case DIRESTICK_UP:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbRY;
					break;
				case DIRESTICK_DOWN:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbRY;
					break;
				case DIRESTICK_LEFT:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbRX;
					break;
				case DIRESTICK_RIGHT:
					fStick = (float)g_joykeyState[i].Gamepad.sThumbRX;
					break;
				}
				break;
			}

			fStick /= STICK_NUM;

			switch (DireStick)
			{
			case DIRESTICK_UP:
				if (bOldStick[i][Stick][DireStick] == true && fStick > STICK_DED)
				{
					return true;
				}
				else if (bOldStick[i][Stick][DireStick] == false && fStick > STICK_DED)
				{
					nStickTime[i][Stick][DireStick]++;

					if (nStickTime[i][Stick][DireStick] >= REPEAT_TIME)
					{
						nStickTime[i][Stick][DireStick] = 0;
						bOldStick[i][Stick][DireStick] = true;
					}
				}
				else
				{
					bOldStick[i][Stick][DireStick] = false;
					nStickTime[i][Stick][DireStick] = 0;
				}
				break;
			case DIRESTICK_DOWN:
				if (bOldStick[i][Stick][DireStick] == true && fStick < -STICK_DED)
				{
					return true;
				}
				else if (bOldStick[i][Stick][DireStick] == false && fStick < -STICK_DED)
				{
					nStickTime[i][Stick][DireStick]++;

					if (nStickTime[i][Stick][DireStick] >= REPEAT_TIME)
					{
						nStickTime[i][Stick][DireStick] = 0;
						bOldStick[i][Stick][DireStick] = true;
					}
				}
				else
				{
					bOldStick[i][Stick][DireStick] = false;
					nStickTime[i][Stick][DireStick] = 0;
				}
				break;
			case DIRESTICK_LEFT:
				if (bOldStick[i][Stick][DireStick] == true && fStick < -STICK_DED)
				{
					return true;
				}
				else if (bOldStick[i][Stick][DireStick] == false && fStick < -STICK_DED)
				{
					nStickTime[i][Stick][DireStick]++;

					if (nStickTime[i][Stick][DireStick] >= REPEAT_TIME)
					{
						nStickTime[i][Stick][DireStick] = 0;
						bOldStick[i][Stick][DireStick] = true;
					}
				}
				else
				{
					bOldStick[i][Stick][DireStick] = false;
					nStickTime[i][Stick][DireStick] = 0;
				}
				break;
			case DIRESTICK_RIGHT:
				if (bOldStick[i][Stick][DireStick] == true && fStick > STICK_DED)
				{
					return true;
				}
				else if (bOldStick[i][Stick][DireStick] == false && fStick > STICK_DED)
				{
					nStickTime[i][Stick][DireStick]++;

					if (nStickTime[i][Stick][DireStick] >= REPEAT_TIME)
					{
						nStickTime[i][Stick][DireStick] = 0;
						bOldStick[i][Stick][DireStick] = true;
					}
				}
				else
				{
					bOldStick[i][Stick][DireStick] = false;
					nStickTime[i][Stick][DireStick] = 0;
				}
				break;
			}
		}
		return false;
	}

	switch (Stick)
	{
	case STICK_LEFT:
		switch (DireStick)
		{
		case DIRESTICK_UP:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbLY;
			break;
		case DIRESTICK_DOWN:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbLY;
			break;
		case DIRESTICK_LEFT:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbLX;
			break;
		case DIRESTICK_RIGHT:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbLX;
			break;
		}
		break;
	case STICK_RIGHT:
		switch (DireStick)
		{
		case DIRESTICK_UP:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbRY;
			break;
		case DIRESTICK_DOWN:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbRY;
			break;
		case DIRESTICK_LEFT:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbRX;
			break;
		case DIRESTICK_RIGHT:
			fStick = (float)g_joykeyState[Controller].Gamepad.sThumbRX;
			break;
		}
		break;
	}

	fStick /= STICK_NUM;

	switch (DireStick)
	{
	case DIRESTICK_UP:
		if (bOldStick[Controller][Stick][DireStick] == true && fStick > STICK_DED)
		{
			return true;
		}
		else if (bOldStick[Controller][Stick][DireStick] == false && fStick > STICK_DED)
		{
			nStickTime[Controller][Stick][DireStick]++;

			if (nStickTime[Controller][Stick][DireStick] >= REPEAT_TIME)
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				bOldStick[Controller][Stick][DireStick] = true;
			}
			return false;
		}
		else
		{
			bOldStick[Controller][Stick][DireStick] = false;
			nStickTime[Controller][Stick][DireStick] = 0;
			return false;
		}
		break;
	case DIRESTICK_DOWN:
		if (bOldStick[Controller][Stick][DireStick] == true && fStick < -STICK_DED)
		{
			return true;
		}
		else if (bOldStick[Controller][Stick][DireStick] == false && fStick < -STICK_DED)
		{
			nStickTime[Controller][Stick][DireStick]++;

			if (nStickTime[Controller][Stick][DireStick] >= REPEAT_TIME)
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				bOldStick[Controller][Stick][DireStick] = true;
			}
			return false;
		}
		else
		{
			bOldStick[Controller][Stick][DireStick] = false;
			nStickTime[Controller][Stick][DireStick] = 0;
			return false;
		}
		break;
	case DIRESTICK_LEFT:
		if (bOldStick[Controller][Stick][DireStick] == true && fStick < -STICK_DED)
		{
			return true;
		}
		else if (bOldStick[Controller][Stick][DireStick] == false && fStick < -STICK_DED)
		{
			nStickTime[Controller][Stick][DireStick]++;

			if (nStickTime[Controller][Stick][DireStick] >= REPEAT_TIME)
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				bOldStick[Controller][Stick][DireStick] = true;
			}
			return false;
		}
		else
		{
			bOldStick[Controller][Stick][DireStick] = false;
			nStickTime[Controller][Stick][DireStick] = 0;
			return false;
		}
		break;
	case DIRESTICK_RIGHT:
		if (bOldStick[Controller][Stick][DireStick] == true && fStick > STICK_DED)
		{
			return true;
		}
		else if (bOldStick[Controller][Stick][DireStick] == false && fStick > STICK_DED)
		{
			nStickTime[Controller][Stick][DireStick]++;

			if (nStickTime[Controller][Stick][DireStick] >= REPEAT_TIME)
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				bOldStick[Controller][Stick][DireStick] = true;
			}
			return false;
		}
		else
		{
			bOldStick[Controller][Stick][DireStick] = false;
			nStickTime[Controller][Stick][DireStick] = 0;
			return false;
		}
		break;
	}

	return false;
}

//----------------
//�U��
//----------------
void VibrateController(WORD wLeftMotorSpeed, WORD wRightMotorSpeed, CONTROLLER Controller)
{
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));

	vibration.wLeftMotorSpeed = wLeftMotorSpeed;  // ���̐U�����[�^�[�̑��x (0-65535)
	vibration.wRightMotorSpeed = wRightMotorSpeed; // �E�̐U�����[�^�[�̑��x (0-65535)

	XInputSetState((DWORD)Controller, &vibration);
}

//-----------------------------
//
//dinput�p�b�h
//
//-----------------------------

//--------------------
//����������
//--------------------
HRESULT InitdJoypad(HINSTANCE hInstanse, HWND hWnd)
{
	//�C���v�b�g�I�u�W�F�N�g�̍쐬
	DirectInput8Create
	(
		hInstanse,
		DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&g_pinput,
		NULL
	);

	// DirectInput�f�o�C�X���
	g_pinput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumDevicesCallback, NULL, DIEDFL_ATTACHEDONLY);

	return S_OK;
}

//----------------------
//�I������
//----------------------
void UninitdJoypad(void)
{
	//�G�t�F�N�g�f�o�C�X�̔j��
	VibratedControllerStop(CONTROLLER_MAX);
	for (int i = 0; i < CONTROLLER_MAX; i++)
	{
		//�L�[�{�[�h�f�o�C�X�̔j��
		if (g_DevdJoypad[i] != NULL)
		{
			g_DevdJoypad[i]->Unacquire();
			g_DevdJoypad[i]->Release();
			g_DevdJoypad[i] = NULL;
		}
	}
	//�C���v�b�g�̔j��
	if (g_pinput != NULL)
	{
		g_pinput->Release();
		g_pinput = NULL;
	}
}

//-------------------------
//�X�V����
//-------------------------
void UpdatedJoypad(void)
{
	DIJOYSTATE  djoykeyState[CONTROLLER_MAX];//�ꎞ���͊i�[
	int ControllerNum = 0;

	for (int i = 0; i < CONTROLLER_MAX; i++)
	{
		if (g_DevdJoypad[i] != NULL)
		{
			if (SUCCEEDED(g_DevdJoypad[i]->GetDeviceState(sizeof(DIJOYSTATE), &djoykeyState[i])))
			{
				g_djoykeyState[i] = djoykeyState[i];//�ꎞ�I�ȏ��𐳋K���ɓn��
			}
			else
			{
				ControllerNum++;
			}
		}
		else
		{
			ControllerNum++;
		}
	}

	if (g_DcontrollerNum!=(CONTROLLER_MAX-ControllerNum))
	{
		SetUnCon(true);
		InputReSet();
		// DirectInput�f�o�C�X���
		g_pinput->EnumDevices(DI8DEVCLASS_GAMECTRL, EnumDevicesCallback, NULL, DIEDFL_ATTACHEDONLY);
	}
}

//------------------------------
//�L�[��������
//------------------------------
bool GetdJoykeyPress(int key, CONTROLLER Controller)
{
	if (Controller == CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			if ((g_djoykeyState[i].rgbButtons[key] & 0x80) ? true : false)
			{
				return true;
			}
		}

		return false;
	}

	return (g_djoykeyState[Controller].rgbButtons[key] & 0x80) ? true : false;
}

//------------------------------
//�L�[���͎��̏���
//------------------------------
bool GetdJoykeyTrigger(int key, CONTROLLER Controller)
{
	static bool bOldkey[CONTROLLER_MAX][NUM_DIJOYKEY_MAX] = { false };

	if (Controller == CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			if (bOldkey[i][key] == false && ((g_djoykeyState[i].rgbButtons[key] & 0x80) ? true : false) == true)
			{
				bOldkey[i][key] = true;
				return true;
			}
			else if (bOldkey[i][key] == true && ((g_djoykeyState[i].rgbButtons[key] & 0x80) ? true : false) == false)
			{
				bOldkey[i][key] = false;
			}
		}

		return false;
	}

	if (bOldkey[Controller][key] == false && ((g_djoykeyState[Controller].rgbButtons[key] & 0x80) ? true : false) == true)
	{
		bOldkey[Controller][key] = true;
		return true;
	}
	else if (bOldkey[Controller][key] == true && ((g_djoykeyState[Controller].rgbButtons[key] & 0x80) ? true : false) == false)
	{
		bOldkey[Controller][key] = false;
		return false;
	}
	else
	{
		return false;
	}
}

//------------------------------
//�L�[�b�������ۂ̏���
//------------------------------
bool GetdJoykeyRelease(int key, CONTROLLER Controller)
{
	static bool bOldkey[CONTROLLER_MAX][NUM_DIJOYKEY_MAX] = { false };
	static int KeyPutTime[CONTROLLER_MAX][NUM_DIJOYKEY_MAX] = { 0 };

	if (Controller == CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			if (bOldkey[i][key] == true && ((g_djoykeyState[i].rgbButtons[key] & 0x80) ? true : false) == false)
			{
				bOldkey[i][key] = false;
				if (KeyPutTime[i][key] >= RELEASE_TIME)
				{
					KeyPutTime[i][key] = 0;
					return true;
				}
				else
				{
					KeyPutTime[i][key] = 0;
				}
			}
			else if (bOldkey[i][key] == false && ((g_djoykeyState[i].rgbButtons[key] & 0x80) ? true : false) == true)
			{
				bOldkey[i][key] = true;
			}
			else if (bOldkey[i][key] == true && ((g_djoykeyState[i].rgbButtons[key] & 0x80) ? true : false) == true)
			{
				KeyPutTime[i][key]++;
			}
		}

		return false;
	}

	if (bOldkey[Controller][key] == true && ((g_djoykeyState[Controller].rgbButtons[key] & 0x80) ? true : false) == false)
	{
		bOldkey[Controller][key] = false;
		if (KeyPutTime[Controller][key] >= RELEASE_TIME)
		{
			KeyPutTime[Controller][key] = 0;
			return true;
		}
		else
		{
			KeyPutTime[Controller][key] = 0;
			return false;
		}
	}
	else if (bOldkey[Controller][key] == false && ((g_djoykeyState[Controller].rgbButtons[key] & 0x80) ? true : false) == true)
	{
		bOldkey[Controller][key] = true;
		return false;
	}
	else if (bOldkey[Controller][key] == true && ((g_djoykeyState[Controller].rgbButtons[key] & 0x80) ? true : false) == true)
	{
		KeyPutTime[Controller][key]++;
		return false;
	}
	else
	{
		return false;
	}
}

//------------------------------
//�L�[����������
//------------------------------
bool GetdJoykeyRepeat(int key, CONTROLLER Controller)
{
	static bool bOldkey[CONTROLLER_MAX][NUM_DIJOYKEY_MAX] = { false };
	static int KeyPutTime[CONTROLLER_MAX][NUM_DIJOYKEY_MAX] = { 0 };

	if (Controller == CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			if (bOldkey[i][key] == true && ((g_djoykeyState[i].rgbButtons[key] & 0x80) ? true : false) == true)
			{
				return true;
			}
			else if (bOldkey[i][key] == false && ((g_djoykeyState[i].rgbButtons[key] & 0x80) ? true : false) == true)
			{
				KeyPutTime[i][key]++;

				if (KeyPutTime[i][key] >= REPEAT_TIME)
				{
					KeyPutTime[i][key] = 0;
					bOldkey[i][key] = true;
				}
			}
			else
			{
				bOldkey[i][key] = false;
				KeyPutTime[i][key] = 0;
			}
		}

		return false;
	}

	if (bOldkey[Controller][key] == true && ((g_djoykeyState[Controller].rgbButtons[key] & 0x80) ? true : false) == true)
	{
		return true;
	}
	else if (bOldkey[Controller][key] == false && ((g_djoykeyState[Controller].rgbButtons[key] & 0x80) ? true : false) == true)
	{
		KeyPutTime[Controller][key]++;

		if (KeyPutTime[Controller][key] >= REPEAT_TIME)
		{
			KeyPutTime[Controller][key] = 0;
			bOldkey[Controller][key] = true;
		}
		return false;
	}
	else
	{
		bOldkey[Controller][key] = false;
		KeyPutTime[Controller][key] = 0;
		return false;
	}
}

//-----------------
//�X�e�B�b�N����
//-----------------
float* GetdJoyStick(STICK Stick, CONTROLLER Controller)
{
	static float fStick[2] = { 0.0f };

	switch (Stick)
	{
	case STICK_LEFT:
		fStick[0] = (float)g_djoykeyState[Controller].lX;
		fStick[1] = (float)g_djoykeyState[Controller].lY;
		break;
	case STICK_RIGHT:
		fStick[0] = (float)g_djoykeyState[Controller].lZ;
		fStick[1] = (float)g_djoykeyState[Controller].lRz;
		break;
	}

	fStick[0] /= STICK_NUM;
	fStick[1] /= STICK_NUM;

	fStick[0] += DSTICK_NUM;
	fStick[1] += DSTICK_NUM;

	return &fStick[0];
}

//
//
//
bool dJoyStickPress(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller)
{
	float fStick = 0.0f;

	if (Controller == CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			switch (Stick)
			{
			case STICK_LEFT:
				switch (DireStick)
				{
				case DIRESTICK_UP:
					fStick = (float)g_djoykeyState[i].lY;
					break;
				case DIRESTICK_DOWN:
					fStick = (float)g_djoykeyState[i].lY;
					break;
				case DIRESTICK_LEFT:
					fStick = (float)g_djoykeyState[i].lX;
					break;
				case DIRESTICK_RIGHT:
					fStick = (float)g_djoykeyState[i].lX;
					break;
				}
				break;
			case STICK_RIGHT:
				switch (DireStick)
				{
				case DIRESTICK_UP:
					fStick = (float)g_djoykeyState[i].lRz;
					break;
				case DIRESTICK_DOWN:
					fStick = (float)g_djoykeyState[i].lRz;
					break;
				case DIRESTICK_LEFT:
					fStick = (float)g_djoykeyState[i].lZ;
					break;
				case DIRESTICK_RIGHT:
					fStick = (float)g_djoykeyState[i].lZ;
					break;
				}
				break;
			}

			fStick /= STICK_NUM;
			fStick += DSTICK_NUM;

			switch (DireStick)
			{
			case DIRESTICK_UP:
				if (fStick < -STICK_DED)
				{
					return true;
				}
				break;
			case DIRESTICK_DOWN:
				if (fStick > STICK_DED)
				{
					return true;
				}
				break;
			case DIRESTICK_LEFT:
				if (fStick < -STICK_DED)
				{
					return true;
				}
				break;
			case DIRESTICK_RIGHT:
				if (fStick > STICK_DED)
				{
					return true;
				}
				break;
			}
		}
		return false;
	}
	switch (Stick)
	{
	case STICK_LEFT:
		switch (DireStick)
		{
		case DIRESTICK_UP:
			fStick = (float)g_djoykeyState[Controller].lY;
			break;
		case DIRESTICK_DOWN:
			fStick = (float)g_djoykeyState[Controller].lY;
			break;
		case DIRESTICK_LEFT:
			fStick = (float)g_djoykeyState[Controller].lX;
			break;
		case DIRESTICK_RIGHT:
			fStick = (float)g_djoykeyState[Controller].lX;
			break;
		}
		break;
	case STICK_RIGHT:
		switch (DireStick)
		{
		case DIRESTICK_UP:
			fStick = (float)g_djoykeyState[Controller].lRz;
			break;
		case DIRESTICK_DOWN:
			fStick = (float)g_djoykeyState[Controller].lRz;
			break;
		case DIRESTICK_LEFT:
			fStick = (float)g_djoykeyState[Controller].lZ;
			break;
		case DIRESTICK_RIGHT:
			fStick = (float)g_djoykeyState[Controller].lZ;
			break;
		}
		break;
	}

	fStick /= STICK_NUM;
	fStick += DSTICK_NUM;

	switch (DireStick)
	{
	case DIRESTICK_UP:
		return fStick > STICK_DED;
		break;
	case DIRESTICK_DOWN:
		return fStick < -STICK_DED;
		break;
	case DIRESTICK_LEFT:
		return fStick < -STICK_DED;
		break;
	case DIRESTICK_RIGHT:
		return fStick > STICK_DED;
		break;
	}

	return false;
}

//
//
//
bool dJoyStickTrigger(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller)
{
	float fStick = 0.0f;
	static bool bOldStick[CONTROLLER_MAX][STICK_MAX][DIRESTICK_MAX] = { false };

	if (Controller == CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			switch (Stick)
			{
			case STICK_LEFT:
				switch (DireStick)
				{
				case DIRESTICK_UP:
					fStick = (float)g_djoykeyState[i].lY;
					break;
				case DIRESTICK_DOWN:
					fStick = (float)g_djoykeyState[i].lY;
					break;
				case DIRESTICK_LEFT:
					fStick = (float)g_djoykeyState[i].lX;
					break;
				case DIRESTICK_RIGHT:
					fStick = (float)g_djoykeyState[i].lX;
					break;
				}
				break;
			case STICK_RIGHT:
				switch (DireStick)
				{
				case DIRESTICK_UP:
					fStick = (float)g_djoykeyState[i].lRz;
					break;
				case DIRESTICK_DOWN:
					fStick = (float)g_djoykeyState[i].lRz;
					break;
				case DIRESTICK_LEFT:
					fStick = (float)g_djoykeyState[i].lZ;
					break;
				case DIRESTICK_RIGHT:
					fStick = (float)g_djoykeyState[i].lZ;
					break;
				}
				break;
			}

			fStick /= STICK_NUM;
			fStick += DSTICK_NUM;

			switch (DireStick)
			{
			case DIRESTICK_UP:
				if (bOldStick[i][Stick][DireStick] == false && fStick < -STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = true;
					return true;
				}
				else if (bOldStick[i][Stick][DireStick] == true && fStick >= -STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = false;
				}
				break;
			case DIRESTICK_DOWN:
				if (bOldStick[i][Stick][DireStick] == false && fStick > STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = true;
					return true;
				}
				else if (bOldStick[i][Stick][DireStick] == true && fStick <= STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = false;
				}
				break;
			case DIRESTICK_LEFT:
				if (bOldStick[i][Stick][DireStick] == false && fStick < -STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = true;
					return true;
				}
				else if (bOldStick[i][Stick][DireStick] == true && fStick >= -STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = false;
				}
				break;
			case DIRESTICK_RIGHT:
				if (bOldStick[i][Stick][DireStick] == false && fStick > STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = true;
					return true;
				}
				else if (bOldStick[i][Stick][DireStick] == true && fStick <= STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = false;
				}
				break;
			}
		}
		return false;
	}


	switch (Stick)
	{
	case STICK_LEFT:
		switch (DireStick)
		{
		case DIRESTICK_UP:
			fStick = (float)g_djoykeyState[Controller].lY;
			break;
		case DIRESTICK_DOWN:
			fStick = (float)g_djoykeyState[Controller].lY;
			break;
		case DIRESTICK_LEFT:
			fStick = (float)g_djoykeyState[Controller].lX;
			break;
		case DIRESTICK_RIGHT:
			fStick = (float)g_djoykeyState[Controller].lX;
			break;
		}
		break;
	case STICK_RIGHT:
		switch (DireStick)
		{
		case DIRESTICK_UP:
			fStick = (float)g_djoykeyState[Controller].lRz;
			break;
		case DIRESTICK_DOWN:
			fStick = (float)g_djoykeyState[Controller].lRz;
			break;
		case DIRESTICK_LEFT:
			fStick = (float)g_djoykeyState[Controller].lZ;
			break;
		case DIRESTICK_RIGHT:
			fStick = (float)g_djoykeyState[Controller].lZ;
			break;
		}
		break;
	}

	fStick /= STICK_NUM;
	fStick += DSTICK_NUM;

	switch (DireStick)
	{
	case DIRESTICK_UP:
		if (bOldStick[Controller][Stick][DireStick] == false && fStick < -STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = true;
			return true;
		}
		else if (bOldStick[Controller][Stick][DireStick] == true && fStick >= -STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = false;
			return false;
		}
		else
		{
			return false;
		}
		break;
	case DIRESTICK_DOWN:
		if (bOldStick[Controller][Stick][DireStick] == false && fStick > STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = true;
			return true;
		}
		else if (bOldStick[Controller][Stick][DireStick] == true && fStick <= STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = false;
			return false;
		}
		else
		{
			return false;
		}
		break;
	case DIRESTICK_LEFT:
		if (bOldStick[Controller][Stick][DireStick] == false && fStick < -STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = true;
			return true;
		}
		else if (bOldStick[Controller][Stick][DireStick] == true && fStick >= -STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = false;
			return false;
		}
		else
		{
			return false;
		}
		break;
	case DIRESTICK_RIGHT:
		if (bOldStick[Controller][Stick][DireStick] == false && fStick > STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = true;
			return true;
		}
		else if (bOldStick[Controller][Stick][DireStick] == true && fStick <= STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = false;
			return false;
		}
		else
		{
			return false;
		}
		break;
	}

	return false;
}

//
//
//
bool dJoyStickRelease(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller)
{
	float fStick = 0.0f;
	static bool bOldStick[CONTROLLER_MAX][STICK_MAX][DIRESTICK_MAX] = { false };
	static int nStickTime[CONTROLLER_MAX][STICK_MAX][DIRESTICK_MAX] = { 0 };

	if (Controller == CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			switch (Stick)
			{
			case STICK_LEFT:
				switch (DireStick)
				{
				case DIRESTICK_UP:
					fStick = (float)g_djoykeyState[i].lY;
					break;
				case DIRESTICK_DOWN:
					fStick = (float)g_djoykeyState[i].lY;
					break;
				case DIRESTICK_LEFT:
					fStick = (float)g_djoykeyState[i].lX;
					break;
				case DIRESTICK_RIGHT:
					fStick = (float)g_djoykeyState[i].lX;
					break;
				}
				break;
			case STICK_RIGHT:
				switch (DireStick)
				{
				case DIRESTICK_UP:
					fStick = (float)g_djoykeyState[i].lRz;
					break;
				case DIRESTICK_DOWN:
					fStick = (float)g_djoykeyState[i].lRz;
					break;
				case DIRESTICK_LEFT:
					fStick = (float)g_djoykeyState[i].lZ;
					break;
				case DIRESTICK_RIGHT:
					fStick = (float)g_djoykeyState[i].lZ;
					break;
				}
				break;
			}

			fStick /= STICK_NUM;
			fStick += DSTICK_NUM;

			switch (DireStick)
			{
			case DIRESTICK_UP:
				if (bOldStick[i][Stick][DireStick] == true && fStick >= -STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = false;
					if (nStickTime[i][Stick][DireStick] >= RELEASE_TIME)
					{
						nStickTime[i][Stick][DireStick] = 0;
						return true;
					}
					else
					{
						nStickTime[i][Stick][DireStick] = 0;
					}
				}
				else if (bOldStick[i][Stick][DireStick] == false && fStick < -STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = true;
				}
				else if (bOldStick[i][Stick][DireStick] == true && fStick < -STICK_DED)
				{
					nStickTime[i][Stick][DireStick]++;
				}
				break;
			case DIRESTICK_DOWN:
				if (bOldStick[i][Stick][DireStick] == true && fStick <= STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = false;
					if (nStickTime[i][Stick][DireStick] >= RELEASE_TIME)
					{
						nStickTime[i][Stick][DireStick] = 0;
						return true;
					}
					else
					{
						nStickTime[i][Stick][DireStick] = 0;
					}
				}
				else if (bOldStick[i][Stick][DireStick] == false && fStick > STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = true;
				}
				else if (bOldStick[i][Stick][DireStick] == true && fStick > STICK_DED)
				{
					nStickTime[i][Stick][DireStick]++;
				}
			case DIRESTICK_LEFT:
				if (bOldStick[i][Stick][DireStick] == true && fStick >= -STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = false;
					if (nStickTime[i][Stick][DireStick] >= RELEASE_TIME)
					{
						nStickTime[i][Stick][DireStick] = 0;
						return true;
					}
					else
					{
						nStickTime[i][Stick][DireStick] = 0;
					}
				}
				else if (bOldStick[i][Stick][DireStick] == false && fStick < -STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = true;
				}
				else if (bOldStick[i][Stick][DireStick] == true && fStick < -STICK_DED)
				{
					nStickTime[i][Stick][DireStick]++;
				}
				break;
			case DIRESTICK_RIGHT:
				if (bOldStick[i][Stick][DireStick] == true && fStick <= STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = false;
					if (nStickTime[i][Stick][DireStick] >= RELEASE_TIME)
					{
						nStickTime[i][Stick][DireStick] = 0;
						return true;
					}
					else
					{
						nStickTime[i][Stick][DireStick] = 0;
					}
				}
				else if (bOldStick[i][Stick][DireStick] == false && fStick > STICK_DED)
				{
					bOldStick[i][Stick][DireStick] = true;
				}
				else if (bOldStick[i][Stick][DireStick] == true && fStick > STICK_DED)
				{
					nStickTime[i][Stick][DireStick]++;
				}
				break;
			}
		}
		return false;
	}


	switch (Stick)
	{
	case STICK_LEFT:
		switch (DireStick)
		{
		case DIRESTICK_UP:
			fStick = (float)g_djoykeyState[Controller].lY;
			break;
		case DIRESTICK_DOWN:
			fStick = (float)g_djoykeyState[Controller].lY;
			break;
		case DIRESTICK_LEFT:
			fStick = (float)g_djoykeyState[Controller].lX;
			break;
		case DIRESTICK_RIGHT:
			fStick = (float)g_djoykeyState[Controller].lX;
			break;
		}
		break;
	case STICK_RIGHT:
		switch (DireStick)
		{
		case DIRESTICK_UP:
			fStick = (float)g_djoykeyState[Controller].lRz;
			break;
		case DIRESTICK_DOWN:
			fStick = (float)g_djoykeyState[Controller].lRz;
			break;
		case DIRESTICK_LEFT:
			fStick = (float)g_djoykeyState[Controller].lZ;
			break;
		case DIRESTICK_RIGHT:
			fStick = (float)g_djoykeyState[Controller].lZ;
			break;
		}
		break;
	}

	fStick /= STICK_NUM;
	fStick += DSTICK_NUM;

	switch (DireStick)
	{
	case DIRESTICK_UP:
		if (bOldStick[Controller][Stick][DireStick] == true && fStick >= -STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = false;
			if (nStickTime[Controller][Stick][DireStick] >= RELEASE_TIME)
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				return true;
			}
			else
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				return false;
			}
		}
		else if (bOldStick[Controller][Stick][DireStick] == false && fStick < -STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = true;
			return false;
		}
		else if (bOldStick[Controller][Stick][DireStick] == true && fStick < -STICK_DED)
		{
			nStickTime[Controller][Stick][DireStick]++;
			return false;
		}
		else
		{
			return false;
		}
		break;
	case DIRESTICK_DOWN:
		if (bOldStick[Controller][Stick][DireStick] == true && fStick <= STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = false;
			if (nStickTime[Controller][Stick][DireStick] >= RELEASE_TIME)
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				return true;
			}
			else
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				return false;
			}
		}
		else if (bOldStick[Controller][Stick][DireStick] == false && fStick > STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = true;
			return false;
		}
		else if (bOldStick[Controller][Stick][DireStick] == true && fStick > STICK_DED)
		{
			nStickTime[Controller][Stick][DireStick]++;
			return false;
		}
		else
		{
			return false;
		}
		break;
	case DIRESTICK_LEFT:
		if (bOldStick[Controller][Stick][DireStick] == true && fStick >= -STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = false;
			if (nStickTime[Controller][Stick][DireStick] >= RELEASE_TIME)
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				return true;
			}
			else
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				return false;
			}
		}
		else if (bOldStick[Controller][Stick][DireStick] == false && fStick < -STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = true;
			return false;
		}
		else if (bOldStick[Controller][Stick][DireStick] == true && fStick < -STICK_DED)
		{
			nStickTime[Controller][Stick][DireStick]++;
			return false;
		}
		else
		{
			return false;
		}
		break;
	case DIRESTICK_RIGHT:
		if (bOldStick[Controller][Stick][DireStick] == true && fStick <= STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = false;
			if (nStickTime[Controller][Stick][DireStick] >= RELEASE_TIME)
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				return true;
			}
			else
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				return false;
			}
		}
		else if (bOldStick[Controller][Stick][DireStick] == false && fStick > STICK_DED)
		{
			bOldStick[Controller][Stick][DireStick] = true;
			return false;
		}
		else if (bOldStick[Controller][Stick][DireStick] == true && fStick > STICK_DED)
		{
			nStickTime[Controller][Stick][DireStick]++;
			return false;
		}
		else
		{
			return false;
		}
		break;
	}

	return false;
}

//
//
//
bool dJoyStickRepeat(DIRESTICK DireStick, STICK Stick, CONTROLLER Controller)
{
	float fStick = 0.0f;
	static bool bOldStick[CONTROLLER_MAX][STICK_MAX][DIRESTICK_MAX] = { false };
	static int nStickTime[CONTROLLER_MAX][STICK_MAX][DIRESTICK_MAX] = { 0 };

	if (Controller == CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			switch (Stick)
			{
			case STICK_LEFT:
				switch (DireStick)
				{
				case DIRESTICK_UP:
					fStick = (float)g_djoykeyState[i].lY;
					break;
				case DIRESTICK_DOWN:
					fStick = (float)g_djoykeyState[i].lY;
					break;
				case DIRESTICK_LEFT:
					fStick = (float)g_djoykeyState[i].lX;
					break;
				case DIRESTICK_RIGHT:
					fStick = (float)g_djoykeyState[i].lX;
					break;
				}
				break;
			case STICK_RIGHT:
				switch (DireStick)
				{
				case DIRESTICK_UP:
					fStick = (float)g_djoykeyState[i].lRz;
					break;
				case DIRESTICK_DOWN:
					fStick = (float)g_djoykeyState[i].lRz;
					break;
				case DIRESTICK_LEFT:
					fStick = (float)g_djoykeyState[i].lZ;
					break;
				case DIRESTICK_RIGHT:
					fStick = (float)g_djoykeyState[i].lZ;
					break;
				}
				break;
			}

			fStick /= STICK_NUM;
			fStick += DSTICK_NUM;

			switch (DireStick)
			{
			case DIRESTICK_UP:
				if (bOldStick[i][Stick][DireStick] == true && fStick < -STICK_DED)
				{
					return true;
				}
				else if (bOldStick[i][Stick][DireStick] == false && fStick < -STICK_DED)
				{
					nStickTime[i][Stick][DireStick]++;

					if (nStickTime[i][Stick][DireStick] >= REPEAT_TIME)
					{
						nStickTime[i][Stick][DireStick] = 0;
						bOldStick[i][Stick][DireStick] = true;
					}
				}
				else
				{
					bOldStick[i][Stick][DireStick] = false;
					nStickTime[i][Stick][DireStick] = 0;
				}
				break;
			case DIRESTICK_DOWN:
				if (bOldStick[i][Stick][DireStick] == true && fStick > STICK_DED)
				{
					return true;
				}
				else if (bOldStick[i][Stick][DireStick] == false && fStick > STICK_DED)
				{
					nStickTime[i][Stick][DireStick]++;

					if (nStickTime[i][Stick][DireStick] >= REPEAT_TIME)
					{
						nStickTime[i][Stick][DireStick] = 0;
						bOldStick[i][Stick][DireStick] = true;
					}
				}
				else
				{
					bOldStick[i][Stick][DireStick] = false;
					nStickTime[i][Stick][DireStick] = 0;
				}
				break;
			case DIRESTICK_LEFT:
				if (bOldStick[i][Stick][DireStick] == true && fStick < -STICK_DED)
				{
					return true;
				}
				else if (bOldStick[i][Stick][DireStick] == false && fStick < -STICK_DED)
				{
					nStickTime[i][Stick][DireStick]++;

					if (nStickTime[i][Stick][DireStick] >= REPEAT_TIME)
					{
						nStickTime[i][Stick][DireStick] = 0;
						bOldStick[i][Stick][DireStick] = true;
					}
				}
				else
				{
					bOldStick[i][Stick][DireStick] = false;
					nStickTime[i][Stick][DireStick] = 0;
				}
				break;
			case DIRESTICK_RIGHT:
				if (bOldStick[i][Stick][DireStick] == true && fStick > STICK_DED)
				{
					return true;
				}
				else if (bOldStick[i][Stick][DireStick] == false && fStick > STICK_DED)
				{
					nStickTime[i][Stick][DireStick]++;

					if (nStickTime[i][Stick][DireStick] >= REPEAT_TIME)
					{
						nStickTime[i][Stick][DireStick] = 0;
						bOldStick[i][Stick][DireStick] = true;
					}
				}
				else
				{
					bOldStick[i][Stick][DireStick] = false;
					nStickTime[i][Stick][DireStick] = 0;
				}
				break;
			}
		}
		return false;
	}

	switch (Stick)
	{
	case STICK_LEFT:
		switch (DireStick)
		{
		case DIRESTICK_UP:
			fStick = (float)g_djoykeyState[Controller].lY;
			break;
		case DIRESTICK_DOWN:
			fStick = (float)g_djoykeyState[Controller].lY;
			break;
		case DIRESTICK_LEFT:
			fStick = (float)g_djoykeyState[Controller].lX;
			break;
		case DIRESTICK_RIGHT:
			fStick = (float)g_djoykeyState[Controller].lX;
			break;
		}
		break;
	case STICK_RIGHT:
		switch (DireStick)
		{
		case DIRESTICK_UP:
			fStick = (float)g_djoykeyState[Controller].lRz;
			break;
		case DIRESTICK_DOWN:
			fStick = (float)g_djoykeyState[Controller].lRz;
			break;
		case DIRESTICK_LEFT:
			fStick = (float)g_djoykeyState[Controller].lZ;
			break;
		case DIRESTICK_RIGHT:
			fStick = (float)g_djoykeyState[Controller].lZ;
			break;
		}
		break;
	}

	fStick /= STICK_NUM;
	fStick += DSTICK_NUM;

	switch (DireStick)
	{
	case DIRESTICK_UP:
		if (bOldStick[Controller][Stick][DireStick] == true && fStick < -STICK_DED)
		{
			return true;
		}
		else if (bOldStick[Controller][Stick][DireStick] == false && fStick < -STICK_DED)
		{
			nStickTime[Controller][Stick][DireStick]++;

			if (nStickTime[Controller][Stick][DireStick] >= REPEAT_TIME)
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				bOldStick[Controller][Stick][DireStick] = true;
			}
			return false;
		}
		else
		{
			bOldStick[Controller][Stick][DireStick] = false;
			nStickTime[Controller][Stick][DireStick] = 0;
			return false;
		}
		break;
	case DIRESTICK_DOWN:
		if (bOldStick[Controller][Stick][DireStick] == true && fStick > STICK_DED)
		{
			return true;
		}
		else if (bOldStick[Controller][Stick][DireStick] == false && fStick > STICK_DED)
		{
			nStickTime[Controller][Stick][DireStick]++;

			if (nStickTime[Controller][Stick][DireStick] >= REPEAT_TIME)
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				bOldStick[Controller][Stick][DireStick] = true;
			}
			return false;
		}
		else
		{
			bOldStick[Controller][Stick][DireStick] = false;
			nStickTime[Controller][Stick][DireStick] = 0;
			return false;
		}
		break;
	case DIRESTICK_LEFT:
		if (bOldStick[Controller][Stick][DireStick] == true && fStick < -STICK_DED)
		{
			return true;
		}
		else if (bOldStick[Controller][Stick][DireStick] == false && fStick < -STICK_DED)
		{
			nStickTime[Controller][Stick][DireStick]++;

			if (nStickTime[Controller][Stick][DireStick] >= REPEAT_TIME)
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				bOldStick[Controller][Stick][DireStick] = true;
			}
			return false;
		}
		else
		{
			bOldStick[Controller][Stick][DireStick] = false;
			nStickTime[Controller][Stick][DireStick] = 0;
			return false;
		}
		break;
	case DIRESTICK_RIGHT:
		if (bOldStick[Controller][Stick][DireStick] == true && fStick > STICK_DED)
		{
			return true;
		}
		else if (bOldStick[Controller][Stick][DireStick] == false && fStick > STICK_DED)
		{
			nStickTime[Controller][Stick][DireStick]++;

			if (nStickTime[Controller][Stick][DireStick] >= REPEAT_TIME)
			{
				nStickTime[Controller][Stick][DireStick] = 0;
				bOldStick[Controller][Stick][DireStick] = true;
			}
			return false;
		}
		else
		{
			bOldStick[Controller][Stick][DireStick] = false;
			nStickTime[Controller][Stick][DireStick] = 0;
			return false;
		}
		break;
	}
	return false;
}

//----------------------
//�X���C�_�[��
//----------------------
float GetdJoySlider(int nSlider, CONTROLLER Controller)
{
	float fSlider = 0.0f;

	fSlider = (float)g_djoykeyState[Controller].rglSlider[nSlider];

	fSlider /= STICK_NUM;

	fSlider += DSTICK_NUM;

	return fSlider;
}

//------------------------
//�n�b�g�X�C�b�`
//------------------------
bool GetdJoyPov(float fData, int nPov, CONTROLLER Controller)
{
	float fPov = 0.0f;

	if (Controller == CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			fPov = (float)g_djoykeyState[i].rgdwPOV[nPov];

			if (fPov==fData)
			{
				return true;
			}
		}

		return false;
	}

	fPov = (float)g_djoykeyState[Controller].rgdwPOV[nPov];

	return (fPov == fData) ? true : false;
}

//-------------------------
//
//-------------------------
bool GetdJoyPovTrigger(float fData, int nPov, CONTROLLER Controller)
{
	float fPov = 0.0f;
	static bool bOldPov[CONTROLLER_MAX][POV_MAX][POV_SYS_MAX] = { false };

	if (Controller == CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			fPov = (float)g_djoykeyState[i].rgdwPOV[nPov];

			if (bOldPov[i][nPov][(int)(fData / POV_NUM)] == false && ((fPov == fData) ? true : false) == true)
			{
				bOldPov[i][nPov][(int)(fData / POV_NUM)] = true;
				return true;
			}
			else if (bOldPov[i][nPov][(int)(fData / POV_NUM)] == true && ((fPov == fData) ? true : false) == false)
			{
				bOldPov[i][nPov][(int)(fData / POV_NUM)] = false;
				return false;
			}
		}

		return false;
	}

	fPov = (float)g_djoykeyState[Controller].rgdwPOV[nPov];

	if (bOldPov[Controller][nPov][(int)(fData / POV_NUM)] == false && ((fPov == fData) ? true : false) == true)
	{
		bOldPov[Controller][nPov][(int)(fData / POV_NUM)] = true;
		return true;
	}
	else if (bOldPov[Controller][nPov][(int)(fData / POV_NUM)] == true && ((fPov == fData) ? true : false) == false)
	{
		bOldPov[Controller][nPov][(int)(fData / POV_NUM)] = false;
		return false;
	}
	else
	{
		return false;
	}
}

//-----------------------
//
//-----------------------
bool GetdJoyPovRelease(float fData, int nPov, CONTROLLER Controller)
{
	float fPov = 0.0f;
	static bool bOldPov[CONTROLLER_MAX][POV_MAX][POV_SYS_MAX] = { false };
	static int PovPutTime[CONTROLLER_MAX][POV_MAX][POV_SYS_MAX] = { 0 };

	if (Controller == CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			fPov = (float)g_djoykeyState[i].rgdwPOV[nPov];

			if (bOldPov[i][nPov][(int)(fData / POV_NUM)] == true && ((fPov == fData) ? true : false) == false)
			{
				bOldPov[i][nPov][(int)(fData / POV_NUM)] = false;
				if (PovPutTime[i][nPov][(int)(fData / POV_NUM)] >= RELEASE_TIME)
				{
					PovPutTime[i][nPov][(int)(fData / POV_NUM)] = 0;
					return true;
				}
				else
				{
					PovPutTime[i][nPov][(int)(fData / POV_NUM)] = 0;
					return false;
				}
			}
			else if (bOldPov[i][nPov][(int)(fData / POV_NUM)] == false && ((fPov == fData) ? true : false) == true)
			{
				bOldPov[i][nPov][(int)(fData / POV_NUM)] = true;
				return false;
			}
			else if (bOldPov[i][nPov][(int)(fData / POV_NUM)] == true && ((fPov == fData) ? true : false) == true)
			{
				PovPutTime[i][nPov][(int)(fData / POV_NUM)]++;
				return false;
			}
		}

		return false;
	}

	fPov = (float)g_djoykeyState[Controller].rgdwPOV[nPov];

	if (bOldPov[Controller][nPov][(int)(fData / POV_NUM)] == true && ((fPov == fData) ? true : false) == false)
	{
		bOldPov[Controller][nPov][(int)(fData / POV_NUM)] = false;
		if (PovPutTime[Controller][nPov][(int)(fData / POV_NUM)] >= RELEASE_TIME)
		{
			PovPutTime[Controller][nPov][(int)(fData / POV_NUM)] = 0;
			return true;
		}
		else
		{
			PovPutTime[Controller][nPov][(int)(fData / POV_NUM)] = 0;
			return false;
		}
	}
	else if (bOldPov[Controller][nPov][(int)(fData / POV_NUM)] == false && ((fPov == fData) ? true : false) == true)
	{
		bOldPov[Controller][nPov][(int)(fData / POV_NUM)] = true;
		return false;
	}
	else if (bOldPov[Controller][nPov][(int)(fData / POV_NUM)] == true && ((fPov == fData) ? true : false) == true)
	{
		PovPutTime[Controller][nPov][(int)(fData / POV_NUM)]++;
		return false;
	}
	else
	{
		return false;
	}
}

//---------------------
//
//---------------------
bool GetdJoyPovRepeat(float fData, int nPov, CONTROLLER Controller)
{
	float fPov = 0.0f;
	static bool bOldPov[CONTROLLER_MAX][POV_MAX][POV_SYS_MAX] = { false };
	static int PovPutTime[CONTROLLER_MAX][POV_MAX][POV_SYS_MAX] = { 0 };

	if (Controller == CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			fPov = (float)g_djoykeyState[i].rgdwPOV[nPov];

			if (bOldPov[i][nPov][(int)(fData / POV_NUM)] == true && ((fPov == fData) ? true : false) == true)
			{
				return true;
			}
			else if (bOldPov[i][nPov][(int)(fData / POV_NUM)] == false && ((fPov == fData) ? true : false) == true)
			{
				PovPutTime[i][nPov][(int)(fData / POV_NUM)]++;

				if (PovPutTime[i][nPov][(int)(fData / POV_NUM)] >= REPEAT_TIME)
				{
					PovPutTime[i][nPov][(int)(fData / POV_NUM)] = 0;
					bOldPov[i][nPov][(int)(fData / POV_NUM)] = true;
				}
				return false;
			}
			else
			{
				bOldPov[i][nPov][(int)(fData / POV_NUM)] = false;
				PovPutTime[i][nPov][(int)(fData / POV_NUM)] = 0;
			}


		}

		return false;
	}

	fPov = (float)g_djoykeyState[Controller].rgdwPOV[nPov];

	if (bOldPov[Controller][nPov][(int)(fData / POV_NUM)] == true && ((fPov == fData) ? true : false) == true)
	{
		return true;
	}
	else if (bOldPov[Controller][nPov][(int)(fData / POV_NUM)] == false && ((fPov == fData) ? true : false) == true)
	{
		PovPutTime[Controller][nPov][(int)(fData / POV_NUM)]++;

		if (PovPutTime[Controller][nPov][(int)(fData / POV_NUM)] >= REPEAT_TIME)
		{
			PovPutTime[Controller][nPov][(int)(fData / POV_NUM)] = 0;
			bOldPov[Controller][nPov][(int)(fData / POV_NUM)] = true;
		}
		return false;
	}
	else
	{
		bOldPov[Controller][nPov][(int)(fData / POV_NUM)] = false;
		PovPutTime[Controller][nPov][(int)(fData / POV_NUM)] = 0;
		return false;
	}
}

//-------------
//�U��
//-------------
void VibratedController(LONG lMotorPower, CONTROLLER Controller)
{
	// �t�H�[�X�t�B�[�h�o�b�N�̃G�t�F�N�g���쐬����
	DIEFFECT effect;
	ZeroMemory(&effect, sizeof(DIEFFECT));
	effect.dwSize = sizeof(DIEFFECT);
	effect.dwFlags = DIEFF_CARTESIAN | DIEFF_OBJECTOFFSETS;
	effect.dwDuration = INFINITE;  // �����ɑ����U��
	effect.dwGain = DI_FFNOMINALMAX;
	effect.dwTriggerButton = DIEB_NOTRIGGER;

	// ���ƕ����̐ݒ�
	DWORD rgdwAxes[1] = { DIJOFS_X };  // X���̐U��
	LONG rglDirection[1] = { 0 };      // �����͐������i0�j
	effect.cAxes = 1;                  // 1��
	effect.rgdwAxes = rgdwAxes;        // �g�p���鎲
	effect.rglDirection = rglDirection;// �U���̕���

	DICONSTANTFORCE cf;
	cf.lMagnitude = lMotorPower;  // �U���̋��� (0 - DI_FFNOMINALMAX)

	effect.cbTypeSpecificParams = sizeof(DICONSTANTFORCE);
	effect.lpvTypeSpecificParams = &cf;

	if (Controller == CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			if (SUCCEEDED(g_DevdJoypad[i]->CreateEffect(GUID_ConstantForce, &effect, &g_djoyEffect[i], NULL)))
			{
				if (FAILED(g_djoyEffect[i]->Start(1, 0)))
				{
					g_djoyEffect[i]->Release();
				}
			}
		}
	}
	else if (SUCCEEDED(g_DevdJoypad[Controller]->CreateEffect(GUID_ConstantForce, &effect, &g_djoyEffect[Controller], NULL)))
	{
		if (FAILED(g_djoyEffect[Controller]->Start(1, 0)))
		{
			g_djoyEffect[Controller]->Release();
		}
	}
}

//---------
//�U��
//---------
void VibratedControllerStop(CONTROLLER Controller)
{
	if (Controller==CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			if (g_djoyEffect[i] != NULL)
			{
				g_djoyEffect[i]->Stop();
				g_djoyEffect[i]->Release();
				g_djoyEffect[i] = NULL;
			}
		}
	}
	else if (g_djoyEffect[Controller] != NULL)
	{
		g_djoyEffect[Controller]->Stop();
		g_djoyEffect[Controller]->Release();
		g_djoyEffect[Controller] = NULL;
	}
}


//------------------------
//
//�U��
//
//------------------------
void SetVibrate(float fPower, CONTROLLER Controller)
{
	static bool bVibate = false;
	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	if (g_DcontrollerNum>=(Controller+1)||Controller==CONTROLLER_MAX)
	{
		if (bVibate)
		{
			if (Controller == CONTROLLER_MAX)
			{
				for (int i = 0; i < g_DcontrollerNum; i++)
				{
					if (XInputGetState((CONTROLLER)i, &state) == ERROR_SUCCESS)
					{
						VibrateController((WORD)(VIBRATION_MAX * fPower), (WORD)(VIBRATION_MAX * fPower), (CONTROLLER)i);
					}
					else
					{
						VibratedControllerStop((CONTROLLER)i);
					}
				}
			}
			else
			{
				if (XInputGetState(Controller, &state) == ERROR_SUCCESS)
				{
					VibrateController((WORD)(VIBRATION_MAX * fPower), (WORD)(VIBRATION_MAX * fPower), Controller);
				}
				else
				{
					VibratedControllerStop(Controller);
				}
			}
			bVibate = false;
		}
		else
		{
			if (Controller == CONTROLLER_MAX)
			{
				for (int i = 0; i < g_DcontrollerNum; i++)
				{
					if (XInputGetState(i, &state) == ERROR_SUCCESS)
					{
						VibrateController((WORD)(VIBRATION_MAX * fPower), (WORD)(VIBRATION_MAX * fPower), (CONTROLLER)i);
					}
					else
					{
						VibratedController((LONG)(DI_FFNOMINALMAX * fPower), (CONTROLLER)i);
						bVibate = true;
					}
				}
			}

			if (XInputGetState(Controller, &state) == ERROR_SUCCESS)
			{
				VibrateController((WORD)(VIBRATION_MAX * fPower), (WORD)(VIBRATION_MAX * fPower), Controller);
			}
			else
			{
				VibratedController((LONG)(DI_FFNOMINALMAX * fPower), Controller);
				bVibate = true;
			}

		}
	}
}

//--------------------------------
//�ڑ��m�F
//--------------------------------

//-----------
//Xinput
//-----------
bool IsXInputControllerConnected(CONTROLLER Controller)
{
	XINPUT_STATE joykeyState[CONTROLLER_MAX];

	if (Controller==CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			if (XInputGetState(i, &joykeyState[i]) == ERROR_SUCCESS)
			{
				return true;
			}
		}

		return false;
	}

	return XInputGetState(Controller, &joykeyState[Controller]) == ERROR_SUCCESS;
}

//------------
//dinput
//------------
bool IsDirectInputControllerConnected(CONTROLLER Controller)
{
	if (Controller == CONTROLLER_MAX)
	{
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			if (g_DevdJoypad[i] != NULL)
			{
				return true;
			}
		}

		return false;
	}

	return g_DevdJoypad[Controller] != NULL;
}

//------------------
//�R���g���[���[��
//------------------
char *ControllerName(CONTROLLER Controller)
{
	return &g_ConName[Controller][0];
}

//------------------
//�R���g���[���[��
//------------------
int ControllerNum(CONTYPE Contype)
{
	if (Contype==CONTYPE_X)
	{
		return g_XcontrollerNum;
	}
	else
	{
		return g_DcontrollerNum;
	}
}

//---------------------------
//�f�o�C�X���擾
//---------------------------
BOOL CALLBACK EnumDevicesCallback(const DIDEVICEINSTANCE* pdidInstance, VOID* pContext)
{
	static int nControllerNum = 0;
	static int XcontrollerNum = 0;

	if (g_bUnCon)
	{
		nControllerNum = 0;
		XcontrollerNum = 0;
		g_DcontrollerNum = 0;
		g_XcontrollerNum = 0;
		g_bX = false;
		g_bUnCon = false;
		//�G�t�F�N�g�f�o�C�X�̔j��
		VibratedControllerStop(CONTROLLER_MAX);
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			//�L�[�{�[�h�f�o�C�X�̔j��
			if (g_DevdJoypad[i] != NULL)
			{
				g_DevdJoypad[i]->Unacquire();
				g_DevdJoypad[i]->Release();
				g_DevdJoypad[i] = NULL;
			}
		}
		for (int i = 0; i < CONTROLLER_MAX; i++)
		{
			g_ConName[i][0] = '\0';
		}
	}

	if ((nControllerNum+XcontrollerNum)<=CONTROLLER_MAX)
	{
		XINPUT_STATE state;
		ZeroMemory(&state, sizeof(XINPUT_STATE));

		g_pDiInstance[nControllerNum] = pdidInstance;

		if (XInputGetState(XcontrollerNum, &state) == ERROR_SUCCESS)
		{
			if (SUCCEEDED(g_pinput->CreateDevice(pdidInstance->guidInstance, &g_DevdJoypad[XcontrollerNum], NULL)))
			{//�C���v�b�g�f�o�C�X�̍쐬
				HINSTANCE hInstanse;
				HWND hWnd;

				hInstanse = GethInstanse();
				hWnd = GethWnd();

				//�t�H�[�}�b�g�쐬
				g_DevdJoypad[nControllerNum]->SetDataFormat(&c_dfDIJoystick);

				g_DevdJoypad[nControllerNum]->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_EXCLUSIVE));

				g_DevdJoypad[nControllerNum]->Acquire();//�A�N�Z�X�����̎擾

				strcpy(&g_ConName[nControllerNum][0], pdidInstance->tszProductName);
				XcontrollerNum++;
			}
		}
		else if (SUCCEEDED(g_pinput->CreateDevice(pdidInstance->guidInstance, &g_DevdJoypad[nControllerNum + XNum()], NULL)))
		{//�C���v�b�g�f�o�C�X�̍쐬
			HINSTANCE hInstanse;
			HWND hWnd;

			hInstanse = GethInstanse();
			hWnd = GethWnd();

			//�t�H�[�}�b�g�쐬
			g_DevdJoypad[nControllerNum + XNum()]->SetDataFormat(&c_dfDIJoystick);

			g_DevdJoypad[nControllerNum + XNum()]->SetCooperativeLevel(hWnd, (DISCL_FOREGROUND | DISCL_EXCLUSIVE));

			g_DevdJoypad[nControllerNum + XNum()]->Acquire();//�A�N�Z�X�����̎擾

			strcpy(&g_ConName[nControllerNum + XNum()][0], pdidInstance->tszProductName);
			nControllerNum++;
		}
	}
	g_DcontrollerNum = nControllerNum+XcontrollerNum;

	// �񋓂𑱍s
	return DIENUM_CONTINUE;
}

//----------
//
//----------
int XNum(void)
{
	int XNum = 0;
	XINPUT_STATE state;
	ZeroMemory(&state, sizeof(XINPUT_STATE));
	for (int i = 0; i < CONTROLLER_MAX; i++)
	{
		if (XInputGetState(i, &state) == ERROR_SUCCESS)
		{
			XNum++;
		}
	}
	return XNum;
}

LPDIRECTINPUT8 GetInput(void)
{
	return g_pinput;
}

void SetUnCon(bool bUnCon)
{
	g_bUnCon = bUnCon;
}

//-----------------------
//�R���g���[���[�Ǘ�������
//-----------------------
void InputReSet(void)
{
	g_bUnCon = false;
	for (int i = 0; i < CONTROLLER_MAX; i++)
	{
		g_ConName[i][0] = '\0';
	}
	g_XcontrollerNum = 0;
	g_DcontrollerNum = 0;
    g_bX = false;
	//�G�t�F�N�g�f�o�C�X�̔j��
	VibratedControllerStop(CONTROLLER_MAX);
	for (int i = 0; i < CONTROLLER_MAX; i++)
	{
		//�L�[�{�[�h�f�o�C�X�̔j��
		if (g_DevdJoypad[i] != NULL)
		{
			g_DevdJoypad[i]->Unacquire();
			g_DevdJoypad[i]->Release();
			g_DevdJoypad[i] = NULL;
		}
	}
}

//------------------------------------------------
//�R���g���[���[���擾
//------------------------------------------------
const DIDEVICEINSTANCE* GetDiInstance(CONTROLLER Controller)
{
	return g_pDiInstance[Controller];
}

//----------------------------------------------
//Xinput�R���g���[���[���擾
//----------------------------------------------
XINPUT_STATE GetXstate(CONTROLLER Controller)
{
	return g_joykeyState[Controller];
}

//----------------------------------------------
//dinput�R���g���[���[���擾
//----------------------------------------------
DIJOYSTATE Getdstate(CONTROLLER Controller)
{
	return g_djoykeyState[Controller];
}

#endif