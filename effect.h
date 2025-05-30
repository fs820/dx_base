//------------------------------------------
//
// Effect�̏����̒�`�E�錾[effect.h]
// Author: fuma sato
//
//------------------------------------------
#pragma once
#include "main.h"
#include "object2D.h"

//--------------------------
// Effect (���[)
//--------------------------
class CEffect final : public CObject2D
{
// ���J
public:
	CEffect() :m_color{}, m_fSpeed{}, m_fLife{} {}
	CEffect(int priority) : CObject2D(priority), m_fSpeed{}, m_fLife{}, m_DefaultScale{} {}
	~CEffect() = default;

	static HRESULT Load(const string sTexturePass);
	static void Unload(void);
	static CEffect* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, int priority = 3, D3DXCOLOR color = D3DXCOLOR(0.1f, 0.5f, 0.4f, 0.5f), float fSpeed = 0.0f);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, D3DXCOLOR color, float fSpeed);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

// ����J
private:
	static constexpr float LIFE = 1.0f;   // ���C�t

	static LPDIRECT3DTEXTURE9 m_apTexture; // ���L�e�N�X�`���̃|�C���^
	static D3DXVECTOR2 m_aImageSize;       // �e�N�X�`���T�C�Y
	D3DXVECTOR3 m_DefaultScale;              // �ŏ��̑傫��
	D3DXCOLOR m_color;
	float m_fSpeed;                       // �ړ����x
	float m_fLife;                          // �o���b�g�̃��C�t
};