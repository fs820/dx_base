//------------------------------------------
//
// �v���C���[�̏��� [player.cpp]
// Author: fuma sato
//
//------------------------------------------
#include "player.h"
#include "renderer.h"
#include "manager.h"
#include "math.h"
#include "bullet.h"
#include "explosion.h"
#include "input.h"
#include "particle.h"
#include "debug.h"

//---------------------------------------
//
// �v���C���[�N���X
//
//---------------------------------------

// �ÓI�����o�ϐ��̒�`
const D3DXVECTOR2 CPlayer::m_imageBlock = D3DXVECTOR2(1.0f, 1.0f);              // �摜�̃u���b�N
const float CPlayer::m_AnimationTime = 0.05f;                                   // �A�j���[�V�����^�C��
LPDIRECT3DTEXTURE9 CPlayer::m_pTexture = NULL;                                  // ���L�e�N�X�`���̃|�C���^
D3DXVECTOR2 CPlayer::m_imageSize = {};                                          // �e�N�X�`���T�C�Y

//-----------------------------
// �\�[�X�ǂݍ���
//-----------------------------
HRESULT CPlayer::Load(const string sTexturePass)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �e�N�X�`��
	D3DXIMAGE_INFO imageInfo = {};
	if (FAILED(D3DXGetImageInfoFromFile
	(
		sTexturePass.c_str(),
		&imageInfo
	)))
	{
		return E_FAIL;
	}
	m_imageSize = D3DXVECTOR2((float)imageInfo.Width, (float)imageInfo.Height);

	if (FAILED(D3DXCreateTextureFromFile
	(
		pDevice,
		sTexturePass.c_str(),
		&m_pTexture
	)))
	{
		return E_FAIL;
	}

	return S_OK;
}

//-----------------------------
// �\�[�X�j��
//-----------------------------
void CPlayer::Unload(void)
{
	// �e�N�X�`��
	if (m_pTexture != NULL)
	{
		m_pTexture->Release();
		m_pTexture = NULL;
	}
}

//------------------------------
// ����
//------------------------------
CPlayer* CPlayer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fSpeed, float fRotSpeed, int priority)
{
	CPlayer* pPlayer = new CPlayer(priority); // �C���X�^���X����

	if (pPlayer == NULL)
	{// �������s
		return NULL;
	}

	pPlayer->SetSize(D3DXVECTOR3(m_imageSize.x, m_imageSize.y, 0.0f)); // �T�C�Y�̐ݒ�

	// ������
	if (FAILED(pPlayer->Init(pos, rot, scale, fSpeed, fRotSpeed)))
	{
		delete pPlayer;
		pPlayer = NULL;
		return NULL;
	}

	// �e�N�X�`���̓o�^
	pPlayer->BindTexture(m_pTexture);

	return pPlayer;
}

//------------------------------
//����������
//------------------------------
HRESULT CPlayer::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fSpeed, float fRotSpeed)
{
	CObject2D::Init(pos, rot, scale, PLAYER); // �e�̏�����

	IsCollision(true); // �����蔻�������	

	// �X�N���[���T�C�Y�̎擾
	D3DXVECTOR2 screenSize = {};
	if (FAILED(CManager::GetRenderer()->GetDxScreenSize(&screenSize)))
	{
		return E_FAIL;
	}

	Transform transform = GetTransform();

	m_state = APPEAR;
	m_StateTime = CMain::GetElapsedTimeGameSpeed();

	m_nLife = LIFE; // �v���C���[�̃��C�t

	m_fSpeed = fSpeed;             // �|���S���̈ړ����x
	m_fRotSpeed = fRotSpeed;       // �|���S���̉�]���x

	m_nAnimationCount = 0;                   // �A�j���[�V�����J�E���g
	m_aniLastTime = CMain::GetElapsedTime(); // �A�j���[�V�����X�V����

	D3DXVECTOR3 size = GetSize(); // �T�C�Y�̎擾
	size.x /= m_imageBlock.x;     // �摜�̃u���b�N���l��
	size.y /= m_imageBlock.y;     // �摜�̃u���b�N���l��
	SetSize(size);                // �e�N�X�`���T�C�Y�̐ݒ�

	// ���_���̐ݒ�
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();
	Vertex* pVtx;
	if (FAILED(pVtxBuff->Lock(0, 0, (void**)&pVtx, 0))) { return E_FAIL; }

	D3DXVECTOR3 screenPos = D3DXVECTOR3(transform.pos.x * screenSize.x, transform.pos.y * screenSize.y, 0.0f);

	D3DXVECTOR3 resultPos[VT_DEF] = {};

	size = GetSize(); // �T�C�Y�̎擾
	size.x *= transform.scale.x;
	size.y *= transform.scale.y;
	CMath::Rotation(resultPos, D3DXVECTOR2(size.x, size.y), transform.rot.z);

	for (size_t cntVtx = 0; cntVtx < VT_DEF; cntVtx++)
	{
		resultPos[cntVtx] += screenPos;

		pVtx[cntVtx].pos = resultPos[cntVtx];
		pVtx[cntVtx].rhw = 1.0f;
		pVtx[cntVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[cntVtx].tex = D3DXVECTOR2((float)(cntVtx % 2) * (1.0f / m_imageBlock.x), (float)(cntVtx / 2) * (1.0f / m_imageBlock.y));
	}

	if (FAILED(pVtxBuff->Unlock())) { return E_FAIL; }

	return S_OK;
}

//------------------------------
//�I������
//------------------------------
void CPlayer::Uninit(void)
{
	CObject2D::Uninit(); // �e�̏I��
}

//------------------------------
//�X�V����
//------------------------------
void CPlayer::Update(void)
{
	CObject2D::Update(); // �e�̍X�V

	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice(); // �����_���[����f�o�C�X���擾

	// �X�N���[���T�C�Y�̎擾
	D3DXVECTOR2 screenSize = {};
	if (FAILED(pRenderer->GetDxScreenSize(&screenSize)))
	{
		return;
	}

	if (m_state != DEATH)
	{// ����łȂ����
		//�ړ��֌W
		CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();
		Transform transform = GetTransform();
		float deltaTime = CMain::GetDeltaTimeGameSpeed();

		D3DXVECTOR3 move = { 0.0f, 0.0f, 0.0f };
		float fAngle = GetRotation().z;

		if (pKeyboard->GetPress(DIK_A))//A�L�[
		{
			if (pKeyboard->GetPress(DIK_W))//W�L�[
			{//����
				move.x += sinf(D3DX_PI * -0.75f) * m_fSpeed * deltaTime;
				move.y += cosf(D3DX_PI * -0.75f) * m_fSpeed * deltaTime;
				fAngle = D3DX_PI * -0.25f;
			}
			else if (pKeyboard->GetPress(DIK_S))//S�L�[
			{//����
				move.x += sinf(D3DX_PI * -0.25f) * m_fSpeed * deltaTime;
				move.y += cosf(D3DX_PI * -0.25f) * m_fSpeed * deltaTime;
				fAngle = D3DX_PI * -0.75f;
			}
			else
			{//��
				move.x += sinf(D3DX_PI * -0.5f) * m_fSpeed * deltaTime;
				move.y += cosf(D3DX_PI * -0.5f) * m_fSpeed * deltaTime;
				fAngle = D3DX_PI * -0.5f;
			}
		}
		else if (pKeyboard->GetPress(DIK_D))//D�L�[
		{
			if (pKeyboard->GetPress(DIK_W) == true)//W�L�[
			{//�E��
				move.x += sinf(D3DX_PI * 0.75f) * m_fSpeed * deltaTime;
				move.y += cosf(D3DX_PI * 0.75f) * m_fSpeed * deltaTime;
				fAngle = D3DX_PI * 0.25f;
			}
			else if (pKeyboard->GetPress(DIK_S) == true)//S�L�[
			{//�E��
				move.x += sinf(D3DX_PI * 0.25f) * m_fSpeed * deltaTime;
				move.y += cosf(D3DX_PI * 0.25f) * m_fSpeed * deltaTime;
				fAngle = D3DX_PI * 0.75f;
			}
			else
			{//�E
				move.x += sinf(D3DX_PI * 0.5f) * m_fSpeed * deltaTime;
				move.y += cosf(D3DX_PI * 0.5f) * m_fSpeed * deltaTime;
				fAngle = D3DX_PI * 0.5f;
			}
		}
		else if (pKeyboard->GetPress(DIK_W) == true)//W�L�[
		{//��
			move.x += sinf(D3DX_PI) * m_fSpeed * deltaTime;
			move.y += cosf(D3DX_PI) * m_fSpeed * deltaTime;
			fAngle = 0.0f;
		}
		else if (pKeyboard->GetPress(DIK_S) == true)//S�L�[
		{//��
			move.x += sinf(0.0f) * m_fSpeed * deltaTime;
			move.y += cosf(0.0f) * m_fSpeed * deltaTime;
			fAngle = D3DX_PI;
		}

		AddPosition(move);
		SetRotation(D3DXVECTOR3(0.0f, 0.0f, fAngle));

		if (pKeyboard->GetTrigger(DIK_SPACE))
		{
			size_t randomNum = CMath::Random(4, 8);
			float angleSet = 2.0f * D3DX_PI / (float)randomNum; // �e�̊p�x�Ԋu
			for (size_t cntBullet = 0; cntBullet < randomNum; cntBullet++)
			{
				float angle = transform.rot.z - cntBullet * angleSet;
				CBullet::Create(transform.pos, D3DXVECTOR3(0.0f, 0.0f, atan2f(-cosf(angle), -sinf(angle))), transform.scale * 10.0f, CMath::Random(0.05f, 0.5f), CBullet::PLAYER, 2);
			}
		}
	}

	// ��ʃ��[�v
	D3DXVECTOR3 pos = GetPosition();
	pos.x = pos.x < 0.0f ? 1.0f : pos.x;
	pos.x = pos.x > 1.0f ? 0.0f : pos.x;
	pos.y = pos.y < 0.0f ? 1.0f : pos.y;
	pos.y = pos.y > 1.0f ? 0.0f : pos.y;
	SetPosition(pos);
	
	Transform transform = GetTransform();

	if ((CMain::GetElapsedTime() - m_aniLastTime) >= m_AnimationTime)
	{// �A�j���[�V�����̍X�V
		m_nAnimationCount++;
		m_nAnimationCount %= (int)(m_imageBlock.x * m_imageBlock.y);

		m_aniLastTime = CMain::GetElapsedTime();
	}

	// ���_���̐ݒ�
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();
	Vertex* pVtx;
	if (FAILED(pVtxBuff->Lock(0, 0, (void**)&pVtx, 0))) { return; }

	// ��]�֌W����
	D3DXVECTOR3 screenPos = D3DXVECTOR3(transform.pos.x * screenSize.x, transform.pos.y * screenSize.y, 0.0f);
	D3DXVECTOR3 resultPos[VT_DEF] = {};

	D3DXVECTOR3 size = GetSize(); // �T�C�Y�̎擾
	size.x *= transform.scale.x;
	size.y *= transform.scale.y;
	CMath::Rotation(resultPos, D3DXVECTOR2(size.x, size.y), transform.rot.z);

	// ���_���̐ݒ�
	for (size_t cntVtx = 0; cntVtx < VT_DEF; cntVtx++)
	{
		resultPos[cntVtx] += screenPos;

		pVtx[cntVtx].pos = resultPos[cntVtx];
		pVtx[cntVtx].rhw = 1.0f;
		pVtx[cntVtx].col = m_state == APPEAR ? TRANSLUCENT_WHITE : m_state == DAMAGE ? RED : WHITE; // �o���͔����� �_���[�W��Ԃ͐�
		pVtx[cntVtx].tex = D3DXVECTOR2((float)(cntVtx % 2) * (1.0f / m_imageBlock.x) + (float)(m_nAnimationCount % (int)(m_imageBlock.x)) * (1.0f / m_imageBlock.x), (float)(cntVtx / 2) * (1.0f / m_imageBlock.y) + (float)(m_nAnimationCount / (int)(m_imageBlock.x)) * (1.0f / m_imageBlock.y));
	}

	if (FAILED(pVtxBuff->Unlock())) { return; }

	// ��ԊǗ�
	switch (m_state)
	{
	case NONE:
		break;
	case APPEAR:
		if (CMain::GetElapsedTimeGameSpeed() - m_StateTime >= APPEAR_TIME)
		{// ��莞�Ԍo��
			m_state = NORMAL; // �ʏ���
			m_StateTime = CMain::GetElapsedTimeGameSpeed(); // ���ԋL�^
		}
		break;
	case NORMAL:
		break;
	case DAMAGE:
		if (CMain::GetElapsedTimeGameSpeed() - m_StateTime >= DAMAGE_TIME)
		{// ��莞�Ԍo��
			m_state = NORMAL; // �ʏ���
			m_StateTime = CMain::GetElapsedTimeGameSpeed(); // ���ԋL�^
		}
		break;
	case DEATH:
		if (CMain::GetElapsedTimeGameSpeed() - m_StateTime >= DEATH_TIME)
		{// ��莞�Ԍo��
			Release();
			return;
		}
		break;
	}

	CDebugProc::Print("Player%d:%d\n", GetID(), m_state);
	CDebugProc::Print("Player%d ��ԃJ�E���^�[:%f\n", GetID(), m_StateTime);
}

//------------------------------
//�`�揈��
//------------------------------
void CPlayer::Draw(void)
{
	CObject2D::Draw(); // �e�̕`��
}

//------------------------------
// �Փˏ���
//------------------------------
void CPlayer::OnCollision(CObject* pOther)
{
	if (m_state == NORMAL)
	{// ���G���ԂłȂ�
		if (pOther->GetType() == ENEMY)
		{// �G�ł����
			Hit(10); // �q�b�g����
		}

		if (pOther->GetType() == BULLET)
		{// �e�Ƃ̏Փ�
			if (dynamic_cast<CBullet*>(pOther)->GetType() == CBullet::ENEMY)
			{// �e�̃^�C�v���v���C���[
				Hit(10); // �q�b�g����
			}
		}
	}
}

//--------------------------
// �_���[�W����
//--------------------------
void CPlayer::Hit(int damage)
{
	m_nLife -= damage; // �_���[�W
	if (m_nLife <= 0)
	{// �̗͂��Ȃ��Ȃ���
		m_state = DEATH; // ��

		Transform transform = GetTransform();
		CExplosion::Create(transform.pos, transform.rot, transform.scale * 100.0f, CMath::Random(CExplosion::DEFAULT, CExplosion::EXTRA),4); // �����G�t�F�N�g�𐶐�
	}
	else
	{
		m_state = DAMAGE; // �_���[�W
	}
	m_StateTime = CMain::GetElapsedTimeGameSpeed(); // ����
}