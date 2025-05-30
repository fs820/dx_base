//------------------------------------------
//
// 2D�`��p�̏���[object2D.cpp]
// Author: fuma sato
//
//------------------------------------------
#include "object2D.h"
#include "renderer.h"
#include "manager.h"

//---------------------------------------
//
// �I�u�W�F�N�g2D�N���X
//
//---------------------------------------

//------------------------------
// ����
//------------------------------
CObject2D* CObject2D::Create(LPDIRECT3DTEXTURE9 pTexture, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, TYPE type, int priority)
{
	CObject2D* pObject2D = new CObject2D(priority); // �C���X�^���X����

	if (pObject2D == NULL)
	{// �������s
		return NULL;
	}

	// ������
	if (FAILED(pObject2D->Init(pos, rot, scale, type)))
	{
		delete pObject2D;
		pObject2D = NULL;
		return NULL;
	}

	// �e�N�X�`���̓o�^
	pObject2D->BindTexture(pTexture);

	return pObject2D;
}

//------------------------------
// ����
//------------------------------
CObject2D* CObject2D::Create(LPDIRECT3DTEXTURE9 pTexture, Transform transform, TYPE type, int priority)
{
	CObject2D* pObject2D = new CObject2D(priority); // �C���X�^���X����

	if (pObject2D == NULL)
	{// �������s
		return NULL;
	}

	// ������
	if (FAILED(pObject2D->Init(transform, type)))
	{
		delete pObject2D;
		pObject2D = NULL;
		return NULL;
	}

	// �e�N�X�`���̓o�^
	pObject2D->BindTexture(pTexture);

	return pObject2D;
}

//------------------------------
//����������
//------------------------------
HRESULT CObject2D::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, TYPE type)
{
	CObject::Init(type); // �e�N���X�̏�����

	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice(); // �����_���[����f�o�C�X���擾

	if (pDevice == NULL)
	{
		return E_POINTER;
	}

	// �X�N���[���T�C�Y�̎擾
	D3DXVECTOR2 screenSize = {};
	if (FAILED(pRenderer->GetDxScreenSize(&screenSize)))
	{
		return E_FAIL;
	}

	SetTransform(Transform(pos, rot, scale));

	//�o�b�t�@�[�̐ݒ�
	if (FAILED(pDevice->CreateVertexBuffer
	(
		sizeof(Vertex) * VT_DEF,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL
	)))
	{
		return E_FAIL;
	}

	return S_OK;
}

//------------------------------
//����������
//------------------------------
HRESULT CObject2D::Init(Transform transform, TYPE type)
{
	CObject::Init(type); // �e�N���X�̏�����

	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice(); // �����_���[����f�o�C�X���擾

	if (pDevice == NULL)
	{
		return E_POINTER;
	}

	// �X�N���[���T�C�Y�̎擾
	D3DXVECTOR2 screenSize = {};
	if (FAILED(pRenderer->GetDxScreenSize(&screenSize)))
	{
		return E_FAIL;
	}

	SetTransform(transform);

	//�o�b�t�@�[�̐ݒ�
	if (FAILED(pDevice->CreateVertexBuffer
	(
		sizeof(Vertex) * VT_DEF,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL
	)))
	{
		return E_FAIL;
	}

	return S_OK;
}

//------------------------------
//�I������
//------------------------------
void CObject2D::Uninit(void)
{
	// ���_�o�b�t�@�̉��
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}
}

//------------------------------
//�X�V����
//------------------------------
void CObject2D::Update(void)
{

}

//------------------------------
//�`�揈��
//------------------------------
void CObject2D::Draw(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();               // �����_���[�̎擾
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice(); // �����_���[����f�o�C�X���擾

	if (pDevice == NULL || m_pVtxBuff == NULL)
	{
		return;
	}

	// ���_�o�b�t�@
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(Vertex));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, m_pTexture);

	// �|���S���̕`��
	pDevice->DrawPrimitive
	(
		D3DPT_TRIANGLESTRIP,//�^�C�v
		0,                  // �C���f�b�N�X
		2                   //�|���S����
	);
}