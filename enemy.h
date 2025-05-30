//------------------------------------------
//
// enemy�̏����̒�`�E�錾[enemy.h]
// Author: fuma sato
//
//------------------------------------------
#pragma once
#include "main.h"
#include "object2D.h"

//--------------------------
// �o���b�g (���[)
//--------------------------
class CEnemy final : public CObject2D
{
// ���J
public:
	using TYPE = enum
	{
		DEFAULT = 0, // �f�t�H���g
		EXTRA,       // �ǉ�
		TYPE_MAX     // �ő�
	};

	// ��ԊǗ�
	using STATE = enum
	{
		NONE = 0, // ��
		NORMAL,   // �ʏ�
		DAMAGE,   // �_���[�W
		DEATH,    // ��
		STATE_MAX // ��Ԑ�
	};

	CEnemy() :m_state{}, m_StateTime{}, m_AttackTime{}, m_fSpeed{}, m_type{}, m_nLife{} { m_nNumAll++; }
	CEnemy(int priority) : CObject2D(priority), m_state{}, m_StateTime{}, m_AttackTime{}, m_fSpeed{}, m_type{}, m_nLife{} { m_nNumAll++; }
	~CEnemy() { m_nNumAll--; };

	static HRESULT Load(const string sTexturePass[TYPE_MAX]);
	static void Unload(void);
	static CEnemy* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fSpeed, TYPE type, int priority = 3);

	static int GetNumAll(void) { return m_nNumAll; };

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fSpeed, TYPE type);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	TYPE GetType(void) const { return m_type; };
	int GetLife(void) const { return m_nLife; }
	STATE GetState(void) const { return m_state; }

// ����J
private:
	void OnCollision(CObject* pOther) override;
	void Hit(const int damage);

	static constexpr int LIFE = 120; // ���C�t
	static constexpr float DAMAGE_TIME = 0.2f; // �_���[�W����
	static constexpr float DEATH_TIME = 0.1f;  // ������
	static constexpr float ATTACK_TIME = 2.0f; // �U���Ԋu

	static LPDIRECT3DTEXTURE9 m_apTexture[TYPE_MAX]; // ���L�e�N�X�`���̃|�C���^
	static D3DXVECTOR2 m_aImageSize[TYPE_MAX];       // �e�N�X�`���T�C�Y

	static int m_nNumAll;                   // �o����

	STATE m_state;                          // ���
	float m_StateTime;                      // ��ԃ^�C�}�[
	float m_AttackTime;                     // �U���^�C�}�[

	float m_fSpeed;                       // �ړ����x
	TYPE m_type;                          // �^�C�v
	int m_nLife;                          // �̗�
};