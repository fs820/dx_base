//------------------------------------------
//
// Bullet�̏����̒�`�E�錾[explosion.h]
// Author: fuma sato
//
//------------------------------------------
#pragma once
#include "main.h"
#include "object2D.h"

//--------------------------
// �o���b�g (���[)
//--------------------------
class CExplosion final : public CObject2D
{
	// ���J
public:
	using TYPE = enum
	{
		DEFAULT = 0, // �f�t�H���g
		EXTRA,       // �ǉ�
		TYPE_MAX     // �ő�
	};

	CExplosion() : m_nAnimationCount{}, m_aniLastTime{}, m_type{} {}
	CExplosion(int priority) : CObject2D(priority), m_nAnimationCount{}, m_aniLastTime{}, m_type{} {}
	~CExplosion() = default;

	static HRESULT Load(const string sTexturePass[TYPE_MAX]);
	static void Unload(void);
	static CExplosion* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, TYPE type, int priority = 3);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, TYPE type);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

// ����J
private:
	void OnCollision(CObject* pOther) override {};

	static const D3DXVECTOR2 m_imageBlock[TYPE_MAX]; // �e�N�X�`���u���b�N��
	static const float m_AnimationTime;	             // �A�j���[�V�����^�C��

	static LPDIRECT3DTEXTURE9 m_apTexture[TYPE_MAX]; // ���L�e�N�X�`���̃|�C���^
	static D3DXVECTOR2 m_aImageSize[TYPE_MAX];       // �e�N�X�`���T�C�Y

	int m_nAnimationCount;              // �A�j���[�V�����J�E���g
	float m_aniLastTime;                // �A�j���[�V�����X�V�^�C��

	TYPE m_type;                       // �^�C�v
};