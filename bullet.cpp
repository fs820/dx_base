//------------------------------------------
//
// Bullet���� [bullet.cpp]
// Author: fuma sato
//
//------------------------------------------
#include "bullet.h"
#include "renderer.h"
#include "manager.h"
#include "math.h"
#include "explosion.h"
#include "effect.h"
#include "particle.h"
#include "debug.h"

//---------------------------------------
//
// Bullet
//
//---------------------------------------

// �ÓI�����o�ϐ�
LPDIRECT3DTEXTURE9 CBullet::m_apTexture[TYPE_MAX] = { NULL }; // ���L�e�N�X�`���̃|�C���^
D3DXVECTOR2 CBullet::m_aImageSize[TYPE_MAX] = {};             // �e�N�X�`���T�C�Y

//------------------------------
// �\�[�X�ǂݍ���
//------------------------------
HRESULT CBullet::Load(const string sTexturePass[TYPE_MAX])
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

	m_aImageSize[1] *= 0.2f; // ������
	return S_OK;
}

//------------------------------
// �\�[�X�j��
//------------------------------
void CBullet::Unload(void)
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
CBullet* CBullet::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fSpeed, TYPE type, int priority)
{
	CBullet* pBullet = new CBullet(priority); // �C���X�^���X����

	if (pBullet == NULL)
	{// �������s
		return NULL;
	}

	D3DXVECTOR3 imageSize = D3DXVECTOR3(m_aImageSize[type].x, m_aImageSize[type].y, 0.0f); // �e�N�X�`���T�C�Y�̎擾
	pBullet->SetSize(imageSize); // �e�N�X�`���T�C�Y�̐ݒ�

	// ������
	if (FAILED(pBullet->Init(pos, rot, scale, fSpeed, type)))
	{
		delete pBullet;
		pBullet = NULL;
		return NULL;
	}

	// �e�N�X�`���o�^
	pBullet->BindTexture(m_apTexture[type]);

	return pBullet;
}

//------------------------------
//����������
//------------------------------
HRESULT CBullet::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fSpeed, TYPE type)
{
	CObject2D::Init(pos, rot, scale, BULLET); // �e�̏�����

	IsCollision(true); // �����蔻�������

	// �X�N���[���T�C�Y�̎擾
	D3DXVECTOR2 screenSize = {};
	if (FAILED(CManager::GetRenderer()->GetDxScreenSize(&screenSize)))
	{
		return E_FAIL;
	}

	m_fSpeed = fSpeed;             // �ړ����x
	m_type = type;                 // ���
	m_fLife = LIFE;                // ���C�t
	m_EffectTIme = 0.0f;           // �G�t�F�N�g����

	// ���_���̐ݒ�
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();
	Vertex* pVtx;
	if (FAILED(pVtxBuff->Lock(0, 0, (void**)&pVtx, 0))) { return E_FAIL; }

	Transform transform = GetTransform();
	D3DXVECTOR3 screenPos = D3DXVECTOR3(transform.pos.x * screenSize.x, transform.pos.y * screenSize.y, 0.0f);

	D3DXVECTOR3 resultPos[VT_DEF] = {};
	D3DXVECTOR3 size = GetSize(); // �T�C�Y�̎擾
	size.x *= transform.scale.x;
	size.y *= transform.scale.y;

	resultPos[0] = D3DXVECTOR3(-size.x * 0.5f, -size.y * 0.5f, 0.0f);
	resultPos[1] = D3DXVECTOR3(size.x * 0.5f, -size.y * 0.5f, 0.0f);
	resultPos[2] = D3DXVECTOR3(-size.x * 0.5f, size.y * 0.5f, 0.0f);
	resultPos[3] = D3DXVECTOR3(size.x * 0.5f, size.y * 0.5f, 0.0f);

	for (size_t cntVtx = 0; cntVtx < VT_DEF; cntVtx++)
	{
		resultPos[cntVtx] += screenPos;

		pVtx[cntVtx].pos = resultPos[cntVtx];
		pVtx[cntVtx].rhw = 1.0f;
		pVtx[cntVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[cntVtx].tex = D3DXVECTOR2((float)(cntVtx % 2) * 1.0f, (float)(cntVtx / 2) * 1.0f);
	}

	if (FAILED(pVtxBuff->Unlock())) { return E_FAIL; }

	return S_OK;
}

//------------------------------
//�I������
//------------------------------
void CBullet::Uninit(void)
{
	CObject2D::Uninit(); // �e�̏I��
}

//------------------------------
//�X�V����
//------------------------------
void CBullet::Update(void)
{
	if (m_fLife <= 0.0f)
	{// ���C�t��0�ȉ��Ȃ�Δ������č폜
		Transform transform = GetTransform();
		CParticle::Create(transform.pos, transform.rot, transform.scale * 10.0f);
		Release();
		return;
	}

	CObject2D::Update(); // �e�̍X�V

	float deltaTime = CMain::GetDeltaTimeGameSpeed();

	m_fLife -= deltaTime; // ���C�t�����炷

	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice(); // �����_���[����f�o�C�X���擾

	Transform transform = GetTransform(); // �ό`���̎擾
	D3DXVECTOR3 move = { 0.0f,0.0f,0.0f };
	move.x += m_fSpeed * sinf(transform.rot.z) * deltaTime;
	move.y += m_fSpeed * cosf(transform.rot.z) * deltaTime;
	AddPosition(move);

	transform = GetTransform(); // �ό`���̎擾
	if (transform.pos.x < 0.0f || transform.pos.x>1.0f || transform.pos.y < 0.0f || transform.pos.y>1.0f)
	{
		CParticle::Create(transform.pos, transform.rot, transform.scale * 10.0f);
		Release();
		return;
	}

	if ((m_EffectTIme += CMain::GetDeltaTimeGameSpeed()) >= EFFECT_TIME)
	{// �G�t�F�N�g���Ԃ���������
		// �G�t�F�N�g����
		D3DXVECTOR3 EffectSize{};
		EffectSize.x = transform.scale.x * GetSize().x;
		EffectSize.y = transform.scale.y * GetSize().y;
		CEffect::Create(transform.pos, transform.rot, EffectSize * 0.05f, 1);

		m_EffectTIme = 0.0f; // �߂�
	}

	// �X�N���[���T�C�Y�̎擾
	D3DXVECTOR2 screenSize = {};
	if (FAILED(pRenderer->GetDxScreenSize(&screenSize)))
	{
		return;
	}

	// ���_���̐ݒ�
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();
	Vertex* pVtx;
	if (FAILED(pVtxBuff->Lock(0, 0, (void**)&pVtx, 0))) { return; }

	// ��]�֌W����y
	D3DXVECTOR3 screenPos = D3DXVECTOR3(transform.pos.x * screenSize.x, transform.pos.y * screenSize.y, 0.0f);

	D3DXVECTOR3 resultPos[VT_DEF] = {};
	D3DXVECTOR3 size = GetSize(); // �T�C�Y�̎擾
	size.x *= transform.scale.x;
	size.y *= transform.scale.y;

	resultPos[0] = D3DXVECTOR3(-size.x * 0.5f, -size.y * 0.5f, 0.0f);
	resultPos[1] = D3DXVECTOR3(size.x * 0.5f, -size.y * 0.5f, 0.0f);
	resultPos[2] = D3DXVECTOR3(-size.x * 0.5f, size.y * 0.5f, 0.0f);
	resultPos[3] = D3DXVECTOR3(size.x * 0.5f, size.y * 0.5f, 0.0f);

	// ���_���̐ݒ�
	for (size_t cntVtx = 0; cntVtx < VT_DEF; cntVtx++)
	{
		resultPos[cntVtx] += screenPos;

		pVtx[cntVtx].pos = resultPos[cntVtx];
		pVtx[cntVtx].rhw = 1.0f;
		pVtx[cntVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[cntVtx].tex = D3DXVECTOR2((float)(cntVtx % 2) * 1.0f, (float)(cntVtx / 2) * 1.0f);
	}

	if (FAILED(pVtxBuff->Unlock())) { return; }
}

//------------------------------
//�`�揈��
//------------------------------
void CBullet::Draw(void)
{
	CObject2D::Draw(); // �e�̕`��
}

//------------------------------
// �Փˏ���
//------------------------------
void CBullet::OnCollision(CObject* pOther)
{
	CObject::TYPE Type = pOther->GetType();
	if ((m_type == CBullet::PLAYER && Type == CObject::ENEMY) || (m_type == CBullet::ENEMY && Type == CObject::PLAYER))
	{// ���������ꍇ
		Hit(); // �G�ɓ�����������

		CDebugProc::Print("BulletHit �������l:%d", m_type);
		CDebugProc::Print("BulletHit ���������l:%d", Type);
	}
}