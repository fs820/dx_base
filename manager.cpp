//------------------------------------------
//
// �}�l�[�W���[ [manager.cpp]
// Author: fuma sato
//
//------------------------------------------
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "object.h"
#include "object2D.h"
#include "player.h"
#include "bg.h"
#include "bullet.h"
#include "explosion.h"
#include "math.h"
#include "enemy.h"
#include "effect.h"
#include "debug.h"

//------------------------------
//
// �}�l�[�W���[�N���X
//
//------------------------------

// �ÓI�����o�ϐ��̒�`
CRenderer* CManager::m_pRenderer{};
CDebugProc* CManager::m_pDebugProc{};         // �f�o�b�N�\���̃|�C���^
CInputKeyboard* CManager::m_pInputKeyboard{};
CBg* CManager::m_pBg{};
CPlayer* CManager::m_pPlayer{};

// �|�[�Y��Ԃ��ǂ���
bool CManager::m_bPause{};

//---------------
// ������
//---------------
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// �`��N���X
	if (m_pRenderer == NULL)
	{
		m_pRenderer = new CRenderer;   // �`��N���X�̃C���X�^���X����
	}

	if (m_pRenderer != NULL)
	{
		if (FAILED(m_pRenderer->Init(hInstance, hWnd, bWindow)))//����������
		{
			return E_FAIL;
		}
	}

	// �f�o�b�N�\���N���X
	if (m_pDebugProc == NULL)
	{
		m_pDebugProc = new CDebugProc;   // �`��N���X�̃C���X�^���X����
	}

	if (m_pDebugProc != NULL)
	{
		if (FAILED(m_pDebugProc->Init()))//����������
		{
			return E_FAIL;
		}
	}

	if (m_pInputKeyboard == NULL)
	{
		m_pInputKeyboard = new CInputKeyboard;   // �L�[�{�[�h�̃C���X�^���X����
	}

	if (m_pInputKeyboard != NULL)
	{
		if (FAILED(m_pInputKeyboard->Init(hInstance, hWnd)))//����������
		{
			return E_FAIL;
		}
	}

	if (FAILED(GameLoad()))return E_FAIL;  // �Q�[���̃��[�h
	if (FAILED(GameStart()))return E_FAIL; // �Q�[���̃X�^�[�g

	return S_OK;
}

//---------------
// �I��
//---------------
void CManager::Uninit(void)
{
	GameEnd();    // �Q�[���̏I��
	GameUnload(); // �\�[�X�̔j��

	// �L�[�{�[�h�j��
	if (m_pInputKeyboard != NULL)
	{
		m_pInputKeyboard->Uninit(); //�I������
		delete m_pInputKeyboard;    // �C���X�^���X�j��
		m_pInputKeyboard = NULL;    // NULL
	}

	// �f�o�b�N�\���j��
	if (m_pDebugProc != NULL)
	{
		m_pDebugProc->Uninit(); //�I������
		delete m_pDebugProc;    // �C���X�^���X�j��
		m_pDebugProc = NULL;    // NULL
	}

	// �����_���[�j��
	if (m_pRenderer != NULL)
	{
		m_pRenderer->Uninit(); //�I������
		delete m_pRenderer;    // �C���X�^���X�j��
		m_pRenderer = NULL;    // NULL
	}
}

//---------------
// �X�V
//---------------
void CManager::Update(void)
{
	// �L�[�{�[�h�̍X�V
	if (m_pInputKeyboard != NULL)
	{
		m_pInputKeyboard->Update(); // �X�V����
	}

	// �����_���[�̍X�V
	if (m_pRenderer != NULL)
	{
		m_pRenderer->Update();
	}

	// �v���C���[���X�|�[��
	if (m_pPlayer->GetState() == CPlayer::DEATH)
	{// �v���C���[�����񂾂�
		m_pPlayer = nullptr; // ���̃v���C���[�̊Ǘ����I���

		// �V�����v���C���[�̐���
		float scale = CMath::Random(0.005f, 0.05f);
		m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.5f, 0.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, RandomAngle), D3DXVECTOR3(scale, scale, 1.0f), CMath::Random(0.5f, 1.0f), CMath::Random(-0.01f, 0.1f));
		if (m_pPlayer == NULL)
		{
			return;
		}
	}

	// �Q�[�����Z�b�g
	if (CEnemy::GetNumAll() <= 0)
	{// �G�l�~�[�����Ȃ��Ȃ�����
		GameEnd();   // �I��点��
		GameStart(); // ���߂ăX�^�[�g����
	}

	CDebugProc::Print("�|�[�Y:[P]\n");
	if (m_pInputKeyboard->GetTrigger(DIK_P))
	{// �|�[�Y�ؑ�
		m_bPause = !m_bPause;
	}

	CDebugProc::Print("�f�o�b�N�\���ؑ�:[F2]\n");
	if (m_pInputKeyboard->GetTrigger(DIK_F2))
	{// �f�o�b�N�\���ؑ�
		CDebugProc::ToggleDebugDraw();
	}
}

//---------------
// �`��
//---------------
void CManager::Draw(void)
{
	// �����_���[�̕`��
	if (m_pRenderer != NULL)
	{
		m_pRenderer->Draw();
	}
}

//---------------------------------
// �Q�[���̃��[�h����
//---------------------------------
HRESULT CManager::GameLoad(void)
{
	//--------------------
	// �\�[�X�ǂݍ���
    //--------------------
	const string sTextureList[] =
	{
		"data\\TEXTURE\\player.png",
		"data\\TEXTURE\\enemy000.png",
		"data\\TEXTURE\\enemy001.png",
		"data\\TEXTURE\\bullet000.png",
		"data\\TEXTURE\\C.png",
		"data\\TEXTURE\\explosion000.png",
		"data\\TEXTURE\\explosion001.png",
		"data\\TEXTURE\\shadow000.jpg"
	};
	if (FAILED(CBg::Load())) return E_FAIL;                        // �w�i
	if (FAILED(CPlayer::Load(sTextureList[0]))) return E_FAIL;     // �v���C���[
	if (FAILED(CEnemy::Load(&sTextureList[1]))) return E_FAIL;     // �G
	if (FAILED(CBullet::Load(&sTextureList[3]))) return E_FAIL;    // �o���b�g
	if (FAILED(CExplosion::Load(&sTextureList[5]))) return E_FAIL; // ����
	if (FAILED(CEffect::Load(sTextureList[7]))) return E_FAIL;     // Effect

	return S_OK;
}

//---------------------------------
// �Q�[���̃X�^�[�g����
//---------------------------------
HRESULT CManager::GameStart(void)
{
	//--------------------
	// �C���X�^���X����
	//--------------------

    // �w�i
	m_pBg = CBg::Create(0);
	if (m_pBg == NULL)
	{
		return E_POINTER;
	}

	// Enemy
	for (size_t cntEnemy = 0; cntEnemy < CMath::Random(size_t(2), size_t(5)); cntEnemy++)
	{
		float scale = CMath::Random(0.05f, 1.0f);
		CEnemy* pEnemy = CEnemy::Create(D3DXVECTOR3(CMath::Random(0.1f, 0.9f), CMath::Random(0.1f, 0.9f), 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(scale, scale, 1.0f), CMath::Random(0.1f, 0.2f), CMath::Random(CEnemy::DEFAULT, CEnemy::EXTRA));
		if (pEnemy == NULL)
		{
			return E_POINTER;
		}
	}

	// �v���C���[
	float scale = CMath::Random(0.005f, 0.05f);
	m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.5f, 0.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, RandomAngle), D3DXVECTOR3(scale, scale, 1.0f), CMath::Random(0.5f, 1.0f), CMath::Random(-0.01f, 0.1f));
	if (m_pPlayer == NULL)
	{
		return E_POINTER;
	}

	m_bPause = false; // �|�[�Y��Ԃ�������

	return S_OK;
}

//---------------------------------
// �Q�[���̃G���h����
//---------------------------------
void CManager::GameEnd(void)
{
	// �I�u�W�F�N�g�j��
	CObject::ReleaseAll();
}

//---------------------------------
// �Q�[���̃A�����[�h����
//---------------------------------
void CManager::GameUnload(void)
{
	CEffect::Unload();    // Effect
	CExplosion::Unload(); // ����
	CBullet::Unload();    // �o���b�g
	CEnemy::Unload();     // �G
	CPlayer::Unload();    // �v���C���[
	CBg::Unload();        // �w�i
}

//-------------------
// �����_���[���Z�b�g
//-------------------
HRESULT CManager::RenererReset(const BOOL bWindow)
{
	if (m_pRenderer != NULL)
	{
		HRESULT hr = m_pRenderer->ReSize(bWindow);// ���T�C�Y����

		return hr;
	}
	else
	{
		return E_POINTER;
	}
}