//------------------------------------------
//
// �I�u�W�F�N�g2D�̏����̒�`�E�錾[object2D.h]
// Author: fuma sato
//
//------------------------------------------
#pragma once
#include "main.h"
#include "object.h"

using namespace DirectX2D; // DirectX2D���O��Ԃ̎g�p

//-----------------------------------------
// �I�u�W�F�N�g2D�N���X (�I�u�W�F�N�g�h��)
//-----------------------------------------
class CObject2D : public CObject
{
// ���J
public:
	CObject2D() : m_pVtxBuff{}, m_pTexture{} {}
	CObject2D(int priority) : CObject(priority), m_pVtxBuff{}, m_pTexture{} {}
	~CObject2D() = default;

	static CObject2D* Create(LPDIRECT3DTEXTURE9 pTexture, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, TYPE type, int priority = 3);
	static CObject2D* Create(LPDIRECT3DTEXTURE9 pTexture, Transform transform, TYPE type, int priority = 3);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, TYPE type);
	HRESULT Init(Transform transform, TYPE type);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	void GetVtxBuff(LPDIRECT3DVERTEXBUFFER9* ppVtxBuff) const { *ppVtxBuff = m_pVtxBuff; }
	LPDIRECT3DVERTEXBUFFER9 GetVtxBuff(void) const { return m_pVtxBuff; }

	void BindTexture(const LPDIRECT3DTEXTURE9 pTexture) { m_pTexture = pTexture; }
	void GetTexture(LPDIRECT3DTEXTURE9* ppTexture) const { *ppTexture = m_pTexture; }
	LPDIRECT3DTEXTURE9 GetTexture(void) const { return m_pTexture; }

// ����J
private:
	void OnCollision(CObject* pOther) override {};

	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff; // ���_
	LPDIRECT3DTEXTURE9 m_pTexture;      // �e�N�X�`��

};