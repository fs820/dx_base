//------------------------------------------
//
// Bullet�̏����̒�`�E�錾[bullet.h]
// Author: fuma sato
//
//------------------------------------------
#pragma once
#include "main.h"
#include "object2D.h"

//--------------------------
// �o���b�g (���[)
//--------------------------
class CBullet final : public CObject2D
{
// ���J
public:
	using TYPE = enum
	{
		PLAYER = 0, // �v���C���[
		ENEMY,      // �G�l�~�[
		TYPE_MAX    // �ő�
	};

	CBullet() : m_fSpeed{}, m_type{}, m_fLife{}, m_EffectTIme{} {}
	CBullet(int priority) :CObject2D(priority), m_fSpeed {}, m_type{}, m_fLife{}, m_EffectTIme{} {}
	~CBullet() = default;

	static HRESULT Load(const string sTexturePass[TYPE_MAX]);
	static void Unload(void);
	static CBullet* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fSpeed, TYPE type, int priority = 3);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fSpeed, TYPE type);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	TYPE GetType(void) const { return m_type; };

// ����J
private:
	void OnCollision(CObject* pOther) override;
	void Hit(void) { m_fLife = 0; }

	static constexpr float LIFE = 20.0f;         // ���C�t
	static constexpr float EFFECT_TIME = 0.001f; // �G�t�F�N�g�Ԋu

	static LPDIRECT3DTEXTURE9 m_apTexture[TYPE_MAX]; // ���L�e�N�X�`���̃|�C���^
	static D3DXVECTOR2 m_aImageSize[TYPE_MAX];       // �e�N�X�`���T�C�Y

	float m_fSpeed;                       // �ړ����x
	TYPE m_type;                          // �o���b�g�̎��
	float m_fLife;                          // �o���b�g�̃��C�t
	float m_EffectTIme;                   // �G�t�F�N�g�^�C�}�[
};