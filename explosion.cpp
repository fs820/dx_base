//------------------------------------------
//
// Bullet���� [bullet.cpp]
// Author: fuma sato
//
//------------------------------------------
#include "explosion.h"
#include "renderer.h"
#include "manager.h"
#include "math.h"

//---------------------------------------
//
// Bullet
//
//---------------------------------------

// �ÓI�����o�ϐ�
const D3DXVECTOR2 CExplosion::m_imageBlock[TYPE_MAX] = { D3DXVECTOR2(8.0f, 1.0f),D3DXVECTOR2(8.0f, 8.0f) };	 // �e�N�X�`���u���b�N��
const float CExplosion::m_AnimationTime = 0.5f;	                                                             // �A�j���[�V�����^�C��

LPDIRECT3DTEXTURE9 CExplosion::m_apTexture[TYPE_MAX] = { NULL }; // ���L�e�N�X�`���̃|�C���^
D3DXVECTOR2 CExplosion::m_aImageSize[TYPE_MAX] = {};             // �e�N�X�`���T�C�Y

//------------------------------
// �\�[�X�ǂݍ���
//------------------------------
HRESULT CExplosion::Load(const string sTexturePass[TYPE_MAX])
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �e�N�X�`��
	for (size_t cntTex = 0; cntTex < TYPE_MAX; cntTex++)
	{
		D3DXIMAGE_INFO imageInfo = {};
		if (FAILED(D3DXGetImageInfoFromFile
		(
			sTexturePass[cntTex].c_str(),
			&imageInfo
		)))
		{
			return E_FAIL;
		}
		m_aImageSize[cntTex] = D3DXVECTOR2((float)imageInfo.Width, (float)imageInfo.Height);

		if (FAILED(D3DXCreateTextureFromFile
		(
			pDevice,
			sTexturePass[cntTex].c_str(),
			&m_apTexture[cntTex]
		)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//------------------------------
// �\�[�X�j��
//------------------------------
void CExplosion::Unload(void)
{
	// �e�N�X�`��
	for (size_t cntTex = 0; cntTex < TYPE_MAX; cntTex++)
	{
		if (m_apTexture[cntTex] != NULL)
		{
			m_apTexture[cntTex]->Release();
			m_apTexture[cntTex] = NULL;
		}
	}
}

//------------------------------
// ����
//------------------------------
CExplosion* CExplosion::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, TYPE type, int priority)
{
	CExplosion* pExplosion = new CExplosion(priority); // �C���X�^���X����

	if (pExplosion == NULL)
	{// �������s
		return NULL;
	}

	pExplosion->SetSize(D3DXVECTOR3(m_aImageSize[type].x, m_aImageSize[type].y, 0.0f)); // �T�C�Y�̐ݒ�

	// ������
	if (FAILED(pExplosion->Init(pos, rot, scale, type)))
	{
		delete pExplosion;
		pExplosion = NULL;
		return NULL;
	}

	// �e�N�X�`���o�^
	pExplosion->BindTexture(m_apTexture[type]);

	return pExplosion;
}

//------------------------------
//����������
//------------------------------
HRESULT CExplosion::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, TYPE type)
{
	CObject2D::Init(pos, rot, scale, EXPLOSION); // �e�̏�����

	// �X�N���[���T�C�Y�̎擾
	D3DXVECTOR2 screenSize = {};
	if (FAILED(CManager::GetRenderer()->GetDxScreenSize(&screenSize)))
	{
		return E_FAIL;
	}

	Transform transform = GetTransform(); // �ϊ����̎擾

	m_type = type; // �^�C�v�̐ݒ�

	D3DXVECTOR3 size = GetSize();     // �T�C�Y�̎擾
	size.x /= m_imageBlock[m_type].x; // �摜�̃u���b�N���l��
	size.y /= m_imageBlock[m_type].y; // �摜�̃u���b�N���l��
	SetSize(size);                    // �e�N�X�`���T�C�Y�̐ݒ�

	m_nAnimationCount = 0;                   // �A�j���[�V�����J�E���g
	m_aniLastTime = CMain::GetElapsedTime(); // �A�j���[�V�����X�V����

	// ���_���̐ݒ�
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff(); // ���_�o�b�t�@�̎擾
	Vertex* pVtx;
	if (FAILED(pVtxBuff->Lock(0, 0, (void**)&pVtx, 0))) { return E_FAIL; }

	D3DXVECTOR3 screenPos = D3DXVECTOR3(transform.pos.x * screenSize.x, transform.pos.y * screenSize.y, 0.0f);

	size = GetSize(); // �e�N�X�`���T�C�Y�̎擾
	size.x *= transform.scale.x;
	size.y *= transform.scale.y;

	D3DXVECTOR3 resultPos[VT_DEF] = {};
	CMath::Rotation(resultPos, D3DXVECTOR2(size.x, size.y), transform.rot.z);

	for (size_t cntVtx = 0; cntVtx < VT_DEF; cntVtx++)
	{
		resultPos[cntVtx] += screenPos;

		pVtx[cntVtx].pos = resultPos[cntVtx];
		pVtx[cntVtx].rhw = 1.0f;
		pVtx[cntVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[cntVtx].tex = D3DXVECTOR2((float)(cntVtx % 2) * (1.0f / m_imageBlock[m_type].x) + (float)(m_nAnimationCount % (int)(m_imageBlock[m_type].x)) * (1.0f / m_imageBlock[m_type].x), (float)(cntVtx / 2) * (1.0f / m_imageBlock[m_type].y) + (float)(m_nAnimationCount / (int)(m_imageBlock[m_type].x)) * (1.0f / m_imageBlock[m_type].y));
	}

	if (FAILED(pVtxBuff->Unlock())) { return E_FAIL; }

	return S_OK;
}

//------------------------------
//�I������
//------------------------------
void CExplosion::Uninit(void)
{
	CObject2D::Uninit(); // �e�̏I��
}

//------------------------------
//�X�V����
//------------------------------
void CExplosion::Update(void)
{
	CObject2D::Update(); // �e�̍X�V

	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice(); // �����_���[����f�o�C�X���擾

	float deltaTime = CMain::GetDeltaTimeGameSpeed();

	if (m_nAnimationCount >= (int)(m_imageBlock[m_type].x * m_imageBlock[m_type].y))
	{
		Release();
		return;
	}

	// �X�N���[���T�C�Y�̎擾
	D3DXVECTOR2 screenSize = {};
	if (FAILED(pRenderer->GetDxScreenSize(&screenSize)))
	{
		return;
	}

	Transform transform = GetTransform(); // �ϊ����̎擾

	// ���_���̐ݒ�
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff(); // ���_�o�b�t�@�̎擾
	Vertex* pVtx;
	if (FAILED(pVtxBuff->Lock(0, 0, (void**)&pVtx, 0))) { return; }

	// ��]�֌W����y
	D3DXVECTOR3 screenPos = D3DXVECTOR3(transform.pos.x * screenSize.x, transform.pos.y * screenSize.y, 0.0f);
	D3DXVECTOR3 resultPos[VT_DEF] = {};

	D3DXVECTOR3 size = GetSize(); // �e�N�X�`���T�C�Y�̎擾
	size.x *= transform.scale.x;
	size.y *= transform.scale.y;
	CMath::Rotation(resultPos, D3DXVECTOR2(size.x, size.y), transform.rot.z);

	if ((CMain::GetElapsedTime() - m_aniLastTime) >= m_AnimationTime / (m_imageBlock[m_type].x * m_imageBlock[m_type].y))
	{// �A�j���[�V�����̍X�V
		m_nAnimationCount++;

		m_aniLastTime = CMain::GetElapsedTime();
	}

	// ���_���̐ݒ�
	for (size_t cntVtx = 0; cntVtx < VT_DEF; cntVtx++)
	{
		resultPos[cntVtx] += screenPos;

		pVtx[cntVtx].pos = resultPos[cntVtx];
		pVtx[cntVtx].rhw = 1.0f;
		pVtx[cntVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[cntVtx].tex = D3DXVECTOR2((float)(cntVtx % 2) * (1.0f / m_imageBlock[m_type].x) + (float)(m_nAnimationCount % (int)(m_imageBlock[m_type].x)) * (1.0f / m_imageBlock[m_type].x), (float)(cntVtx / 2) * (1.0f / m_imageBlock[m_type].y) + (float)(m_nAnimationCount / (int)(m_imageBlock[m_type].x)) * (1.0f / m_imageBlock[m_type].y));
	}

	if (FAILED(pVtxBuff->Unlock())) { return; }
}

//------------------------------
//�`�揈��
//------------------------------
void CExplosion::Draw(void)
{
	CObject2D::Draw(); // �e�̕`��
}