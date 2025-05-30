//------------------------------------------
//
// �v���C���[�̏��� [player.cpp]
// Author: fuma sato
//
//------------------------------------------
#include "bg.h"
#include "renderer.h"
#include "manager.h"
#include "object2D.h"

//---------------------------------------
//
// �v���C���[�N���X
//
//---------------------------------------

// �ÓI�����o�ϐ��̒�`

// �e�N�X�`���̃p�X
const string CBg::m_sTexturePass[CBg::MAX_TEXTURE] =
{
	"data\\TEXTURE\\bg100.png",
	"data\\TEXTURE\\bg101.png",
	"data\\TEXTURE\\bg102.png"
};
const float CBg::m_afScrollSpeed[CBg::MAX_TEXTURE] = { 0.01f, 0.05f, 0.1f }; // �X�N���[�����x

CObject2D* CBg::m_pBg[CBg::MAX_TEXTURE] = { nullptr };      // �w�i�̃|�C���^�z��
LPDIRECT3DTEXTURE9 CBg::m_apTexture[CBg::MAX_TEXTURE] = {}; // �e�N�X�`���̔z��
D3DXVECTOR2 CBg::m_aImageSize[CBg::MAX_TEXTURE] = {};       // �e�N�X�`���̃T�C�Y

//------------------------------
// �\�[�X�ǂݍ���
//------------------------------
HRESULT CBg::Load(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �e�N�X�`��
	for (size_t cntTex = 0; cntTex < MAX_TEXTURE; cntTex++)
	{
		if (m_sTexturePass[cntTex].empty())
		{// �e�N�X�`���p�X����Ȃ�I��
			break;
		}

		D3DXIMAGE_INFO imageInfo = {};
		if (FAILED(D3DXGetImageInfoFromFile
		(
			m_sTexturePass[cntTex].c_str(),
			&imageInfo
		)))
		{
			return E_FAIL;
		}
		m_aImageSize[cntTex] = D3DXVECTOR2((float)imageInfo.Width, (float)imageInfo.Height);

		if (FAILED(D3DXCreateTextureFromFile
		(
			pDevice,
			m_sTexturePass[cntTex].c_str(),
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
void CBg::Unload(void)
{
	// �e�N�X�`��
	for (size_t cntTex = 0; cntTex < MAX_TEXTURE; cntTex++)
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
CBg* CBg::Create(int priority)
{
	CBg* pBg = new CBg(priority);

	if (FAILED(pBg->Init()))
	{
		delete pBg;
		pBg = NULL;
		return NULL;
	}
	return pBg;
}

//------------------------------
//����������
//------------------------------
HRESULT CBg::Init(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice(); // �����_���[����f�o�C�X���擾

	CObject::Init(BACKGROUND);

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

	// �w�i�T�C�Y�̎擾
	for (size_t cntTex = 0; cntTex < MAX_TEXTURE; cntTex++)
	{
		if (m_apTexture[cntTex] == nullptr)
		{
			break;
		}

		m_pBg[cntTex] = CObject2D::Create(m_apTexture[cntTex], D3DXVECTOR3(screenSize.x * 0.5f, screenSize.y * 0.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(1.0f, 1.0f, 1.0f), BACKGROUND);

		if (m_aImageSize[cntTex].x == 0.0f || m_aImageSize[cntTex].y == 0.0f)
		{
			continue;
		}

		D3DXVECTOR2 screenSizeRatio = D3DXVECTOR2(screenSize.x / m_aImageSize[cntTex].x, screenSize.y / m_aImageSize[cntTex].y);
		if (screenSizeRatio.x > screenSizeRatio.y)
		{
			SetSize(D3DXVECTOR3(m_aImageSize[cntTex].x * screenSizeRatio.x, m_aImageSize[cntTex].y * screenSizeRatio.x, 0.0f)); // �T�C�Y��ݒ�
		}
		else
		{
			SetSize(D3DXVECTOR3(m_aImageSize[cntTex].x * screenSizeRatio.y, m_aImageSize[cntTex].y * screenSizeRatio.y, 0.0f)); // �T�C�Y��ݒ�
		}

		// ���_���̐ݒ�
		LPDIRECT3DVERTEXBUFFER9 pVtxBuff = m_pBg[cntTex]->GetVtxBuff();
		Vertex* pVtx;
		if (FAILED(pVtxBuff->Lock(0, 0, (void**)&pVtx, 0))) { return E_FAIL; }

		D3DXVECTOR3 size = GetSize(); // �T�C�Y���擾
		for (size_t cntVtx = 0; cntVtx < VT_DEF; cntVtx++)
		{
			pVtx[cntVtx].pos = D3DXVECTOR3(size.x * (float)(cntVtx % 2) + screenSize.x * 0.5f - size.x * 0.5f, size.y * (float)(cntVtx / 2) + screenSize.y * 0.5f - size.y * 0.5f, 0.0f);
			pVtx[cntVtx].rhw = 1.0f;
			pVtx[cntVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[cntVtx].tex = D3DXVECTOR2((float)(cntVtx % 2) + m_TextureU[cntTex], (float)(cntVtx / 2));
		}

		if (FAILED(pVtxBuff->Unlock())) { return E_FAIL; }
	}

	return S_OK;
}

//------------------------------
//�I������
//------------------------------
void CBg::Uninit(void)
{

}

//------------------------------
//�X�V����
//------------------------------
void CBg::Update(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice(); // �����_���[����f�o�C�X���擾

	// �X�N���[���T�C�Y�̎擾
	D3DXVECTOR2 screenSize = {};
	if (FAILED(pRenderer->GetDxScreenSize(&screenSize)))
	{
		return;
	}

	// �w�i�T�C�Y�̎擾
	for (size_t cntTex = 0; cntTex < MAX_TEXTURE; cntTex++)
	{
		if (m_pBg[cntTex] == NULL)
		{
			break;
		}

		if (m_aImageSize[cntTex].x == 0.0f || m_aImageSize[cntTex].y == 0.0f)
		{
			continue;
		}

		// �X�N���[������
		m_TextureU[cntTex] += m_afScrollSpeed[cntTex] * CMain::GetDeltaTimeGameSpeed();

		D3DXVECTOR2 screenSizeRatio = D3DXVECTOR2(screenSize.x / m_aImageSize[cntTex].x, screenSize.y / m_aImageSize[cntTex].y);
		if (screenSizeRatio.x > screenSizeRatio.y)
		{
			SetSize(D3DXVECTOR3(m_aImageSize[cntTex].x * screenSizeRatio.x, m_aImageSize[cntTex].y * screenSizeRatio.x, 0.0f)); // �T�C�Y��ݒ�
		}
		else
		{
			SetSize(D3DXVECTOR3(m_aImageSize[cntTex].x * screenSizeRatio.y, m_aImageSize[cntTex].y * screenSizeRatio.y, 0.0f)); // �T�C�Y��ݒ�
		}

		// ���_���̐ݒ�
		LPDIRECT3DVERTEXBUFFER9 pVtxBuff = m_pBg[cntTex]->GetVtxBuff();
		Vertex* pVtx;
		if (FAILED(pVtxBuff->Lock(0, 0, (void**)&pVtx, 0))) { return; }

		D3DXVECTOR3 size = GetSize(); // �T�C�Y���擾
		for (size_t cntVtx = 0; cntVtx < VT_DEF; cntVtx++)
		{
			pVtx[cntVtx].pos = D3DXVECTOR3(size.x * (float)(cntVtx % 2) + screenSize.x * 0.5f - size.x * 0.5f, size.y * (float)(cntVtx / 2) + screenSize.y * 0.5f - size.y * 0.5f, 0.0f);
			pVtx[cntVtx].rhw = 1.0f;
			pVtx[cntVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[cntVtx].tex = D3DXVECTOR2((float)(cntVtx % 2) + m_TextureU[cntTex], (float)(cntVtx / 2));
		}

		if (FAILED(pVtxBuff->Unlock())) { return; }
	}
}

//------------------------------
//�`�揈��
//------------------------------
void CBg::Draw(void)
{

}