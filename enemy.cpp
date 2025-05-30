//------------------------------------------
//
// enemy���� [enemy.cpp]
// Author: fuma sato
//
//------------------------------------------
#include "enemy.h"
#include "renderer.h"
#include "manager.h"
#include "math.h"
#include "explosion.h"
#include "particle.h"
#include "bullet.h"
#include "debug.h"

//---------------------------------------
//
// Enemy
//
//---------------------------------------

// �ÓI�����o�ϐ�
LPDIRECT3DTEXTURE9 CEnemy::m_apTexture[TYPE_MAX] = { NULL }; // ���L�e�N�X�`���̃|�C���^
D3DXVECTOR2 CEnemy::m_aImageSize[TYPE_MAX] = {};             // �e�N�X�`���T�C�Y
int CEnemy::m_nNumAll = 0;                                   // �o����

//------------------------------
// �\�[�X�ǂݍ���
//------------------------------
HRESULT CEnemy::Load(const string sTexturePass[TYPE_MAX])
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
void CEnemy::Unload(void)
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
CEnemy* CEnemy::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fSpeed, TYPE type, int priority)
{
	CEnemy* pBullet = new CEnemy(priority); // �C���X�^���X����

	if (pBullet == NULL)
	{// �������s
		return NULL;
	}

	pBullet->SetSize(D3DXVECTOR3(m_aImageSize[type].x, m_aImageSize[type].y, 0.0f)); // �T�C�Y�̐ݒ�

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
HRESULT CEnemy::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fSpeed, TYPE type)
{
	CObject2D::Init(pos, rot, scale, ENEMY); // �e�̏�����

	IsCollision(true); // �����蔻�������	

	// �X�N���[���T�C�Y�̎擾
	D3DXVECTOR2 screenSize = {};
	if (FAILED(CManager::GetRenderer()->GetDxScreenSize(&screenSize)))
	{
		return E_FAIL;
	}

	m_nLife = LIFE;                // ���C�t�̏�����
	m_state = NORMAL;              // ������
	m_StateTime = 0.0f;  // �o������
	m_AttackTime = 0.0f; // �U������

	m_fSpeed = fSpeed;             // �|���S���̈ړ����x
	m_type = type;                 // �G�̎��

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
	CMath::Rotation(resultPos, D3DXVECTOR2(size.x, size.y), GetRotation().z);

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
void CEnemy::Uninit(void)
{
	CObject2D::Uninit(); // �e�̏I��
}

//------------------------------
//�X�V����
//------------------------------
void CEnemy::Update(void)
{
	CObject2D::Update(); // �e�̍X�V

	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice(); // �����_���[����f�o�C�X���擾

	float deltaTime = CMain::GetDeltaTimeGameSpeed();

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

	Transform transform = GetTransform(); // �ό`���̎擾

	// ��]�֌W����
	D3DXVECTOR3 screenPos = D3DXVECTOR3(transform.pos.x * screenSize.x, transform.pos.y * screenSize.y, 0.0f);
	D3DXVECTOR3 resultPos[VT_DEF] = {};

	D3DXVECTOR3 size = GetSize(); // �e�N�X�`���T�C�Y�̎擾
	size.x *= transform.scale.x;
	size.y *= transform.scale.y;
	CMath::Rotation(resultPos, D3DXVECTOR2(size.x, size.y), transform.rot.z);

	// ���_���̐ݒ�
	for (size_t cntVtx = 0; cntVtx < VT_DEF; cntVtx++)
	{
		resultPos[cntVtx] += screenPos;

		pVtx[cntVtx].pos = resultPos[cntVtx];
		pVtx[cntVtx].rhw = 1.0f;
		pVtx[cntVtx].col = m_state == DAMAGE ? RED : WHITE; // �_���[�W��Ԃ͐�
		pVtx[cntVtx].tex = D3DXVECTOR2((float)(cntVtx % 2) * 1.0f, (float)(cntVtx / 2) * 1.0f);
	}

	if (FAILED(pVtxBuff->Unlock())) { return; }

	// ��ԊǗ�
	switch (m_state)
	{
	case NONE:
		break;
	case NORMAL:
		if ((m_AttackTime += CMain::GetDeltaTimeGameSpeed()) >= ATTACK_TIME)
		{// ��莞�Ԍo��
			size_t randomNum = CMath::Random(1, 2);
			float angleSet = 2.0f * D3DX_PI / (float)randomNum; // �e�̊p�x�Ԋu
			for (size_t cntBullet = 0; cntBullet < randomNum; cntBullet++)
			{
				float angle = transform.rot.z - cntBullet * angleSet;
				CBullet::Create(transform.pos, D3DXVECTOR3(0.0f, 0.0f, atan2f(-cosf(angle), -sinf(angle))), transform.scale * 2.0f, CMath::Random(0.05f, 0.5f), CBullet::ENEMY, 2);
			}
			m_AttackTime = 0.0f; // �߂�
		}
		break;
	case DAMAGE:
		if ((m_StateTime += CMain::GetDeltaTimeGameSpeed()) >= DAMAGE_TIME)
		{// ��莞�Ԍo��
			m_state = NORMAL; // �ʏ���
			m_StateTime = 0.0f; // �߂�
		}
		break;
	case DEATH:
		if ((m_StateTime += CMain::GetDeltaTimeGameSpeed()) >= DEATH_TIME)
		{// ��莞�Ԍo��
			Release();
			return;
		}
		break;
	}

	CDebugProc::Print("Enemy%d:%d\n", GetID(), m_state);
	CDebugProc::Print("Enemy%d ��ԃJ�E���^�[:%f\n", GetID(), m_StateTime);
	CDebugProc::Print("Enemy%d �U���J�E���^�[:%f\n", GetID(), m_AttackTime);
}

//------------------------------
//�`�揈��
//------------------------------
void CEnemy::Draw(void)
{
	CObject2D::Draw(); // �e�̕`��
}

//------------------------------
// �Փˏ���
//------------------------------
void CEnemy::OnCollision(CObject* pOther)
{
	if (m_state == NORMAL || m_state == DAMAGE)
	{// ���G���ԂłȂ�
		if (pOther->GetType() == BULLET)
		{// �e�Ƃ̏Փ�
			if (dynamic_cast<CBullet*>(pOther)->GetType() == CBullet::PLAYER)
			{// �e�̃^�C�v���v���C���[
				Hit(10); // �q�b�g����
			}
		}
	}
}

//--------------------------
// �_���[�W����
//--------------------------
void CEnemy::Hit(int damage)
{
	m_nLife -= damage; // �_���[�W
	if (m_nLife <= 0)
	{// �̗͂��Ȃ��Ȃ���
		m_state = DEATH; // ��

		Transform transform = GetTransform();
		CExplosion::Create(transform.pos, transform.rot, transform.scale * 100.0f, CMath::Random(CExplosion::DEFAULT, CExplosion::EXTRA), 4); // �����G�t�F�N�g�𐶐�
	}
	else
	{
		m_state = DAMAGE; // �_���[�W
	}
	m_StateTime = 0.0f; // �߂�
}