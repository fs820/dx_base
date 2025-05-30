//------------------------------------------
//
// Player�̏����̒�`�E�錾[player.h]
// Author: fuma sato
//
//------------------------------------------
#pragma once
#include "main.h"
#include "object2D.h"

//--------------------------
// �v���C���[�N���X (���[)
//--------------------------
class CPlayer final : public CObject2D
{
// ���J
public:
	// ��ԊǗ�
	using STATE = enum
	{
		NONE = 0, // ��
		APPEAR,   // �o��
		NORMAL,   // �ʏ�
		DAMAGE,   // �_���[�W
		DEATH,    // ��
		STATE_MAX // ��Ԑ�
	};

	CPlayer() : m_state{}, m_StateTime{}, m_nLife{}, m_fSpeed{}, m_fRotSpeed{}, m_nAnimationCount{}, m_aniLastTime{} {}
	CPlayer(int priority) :CObject2D(priority), m_state{}, m_StateTime{}, m_nLife{}, m_fSpeed{}, m_fRotSpeed{}, m_nAnimationCount{}, m_aniLastTime{} {}
	~CPlayer() = default;

	static HRESULT Load(const string sTexturePass);
	static void Unload(void);
	static CPlayer* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fSpeed, float fRotSpeed, int priority = 3);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fSpeed, float fRotSpeed);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	int GetLife(void) const { return m_nLife; }
	STATE GetState(void) const { return m_state; }

// ����J
private:
	void OnCollision(CObject* pOther) override;
	void Hit(int damage);

	static constexpr int LIFE = 200;           // ���C�t
	static constexpr float APPEAR_TIME = 2.0f; // �o������
	static constexpr float DAMAGE_TIME = 0.5f; // �_���[�W����
	static constexpr float DEATH_TIME = 2.0f;  // ������

	static const D3DXVECTOR2 m_imageBlock;	 // �e�N�X�`���u���b�N��
	static const float m_AnimationTime;	     // �A�j���[�V�����^�C��

	static LPDIRECT3DTEXTURE9 m_pTexture;    // ���L�e�N�X�`���̃|�C���^
	static D3DXVECTOR2 m_imageSize;          // �e�N�X�`���T�C�Y

	STATE m_state;                          // ���
	float m_StateTime;                      // ��ԃ^�C�}�[
	int m_nLife;                            // ���C�t

	float m_fSpeed;                         // �ړ����x
	float m_fRotSpeed;                      // ��]���x

	int m_nAnimationCount;              // �A�j���[�V�����J�E���g
	float m_aniLastTime;                // �A�j���[�V�����X�V�^�C��
};