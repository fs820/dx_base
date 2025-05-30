//---------------------------------------
//
// Particle����[particle.cpp]
// Author: fuma sato
//
//---------------------------------------
#include "particle.h"
#include "effect.h"
#include "manager.h"
#include "math.h"

//---------------------------------------
//
// Particle
//
//---------------------------------------

//------------------------------
// �\�[�X�ǂݍ���
//------------------------------
HRESULT CParticle::Load(const string sTexturePass)
{
	return S_OK;
}

//------------------------------
// �\�[�X�j��
//------------------------------
void CParticle::Unload(void)
{

}

//------------------------------
// ����
//------------------------------
CParticle* CParticle::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, int priority)
{
	CParticle* pParticle = new CParticle(priority); // �C���X�^���X����

	if (pParticle == NULL)
	{// �������s
		return NULL;
	}

	// ������
	if (FAILED(pParticle->Init(pos, rot, scale)))
	{
		delete pParticle;
		pParticle = NULL;
		return NULL;
	}

	return pParticle;
}

//------------------------------
//����������
//------------------------------
HRESULT CParticle::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale)
{
	SetTransform(Transform(pos, rot, scale));
	m_nLife = LIFE;

	return S_OK;
}

//------------------------------
//�I������
//------------------------------
void CParticle::Uninit(void)
{
	
}

//------------------------------
//�X�V����
//------------------------------
void CParticle::Update(void)
{
	if (CMain::GetElapsedTimeGameSpeed())
	{
		Transform transform = GetTransform();
		transform.scale *= m_nLife / LIFE;

		for (int nCnt = 0; nCnt < NUMBER; nCnt++)
		{
			D3DXVECTOR3 scale{};
			float fscale = 0.0f;
			fscale = (float)(rand() % (int)(transform.scale.x + 1.0f) * 10.0f);
			scale.x = fscale;
			scale.y = fscale;
			scale.z = fscale;

			CEffect::Create(transform.pos, D3DXVECTOR3(0.0f, 0.0f, RandomAngle), scale, 1, RandomColorTranslucent, CMath::Random(1.0f, 10.0f));
		}
	}

	m_nLife -= CMain::GetDeltaTimeGameSpeed();

	if (m_nLife <= 0.0f)
	{
		Release();
	}
}

//------------------------------
//�`�揈��
//------------------------------
void CParticle::Draw(void)
{

}