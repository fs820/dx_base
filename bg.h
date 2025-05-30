//------------------------------------------
//
// Player�̏����̒�`�E�錾[player.h]
// Author: fuma sato
//
//------------------------------------------
#pragma once
#include "main.h"
#include "object.h"

class CObject2D; // �O���錾

using namespace DirectX2D; // DirectX2D���O��Ԃ̎g�p

//--------------------------
// �v���C���[�N���X
//--------------------------
class CBg final : public CObject
{
	// ���J
public:
	CBg() : m_TextureU{} {}
	CBg(int priority) :CObject(priority), m_TextureU{} {}
	~CBg() = default;

	static HRESULT Load(void);
	static void Unload(void);
	static CBg* Create(int priority = 3);

	HRESULT Init(void);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

	// ����J
private:
	void OnCollision(CObject* pOther) override {};

	static constexpr int MAX_TEXTURE = 3;            // �w�i�̍ő吔

	static const string m_sTexturePass[MAX_TEXTURE]; // �e�N�X�`���̃p�X
	static const float m_afScrollSpeed[MAX_TEXTURE]; // �X�N���[�����x

	static CObject2D* m_pBg[MAX_TEXTURE];  // �w�i�̃|�C���^�z��

	static LPDIRECT3DTEXTURE9 m_apTexture[MAX_TEXTURE]; // �e�N�X�`���̃|�C���^�z��
	static D3DXVECTOR2 m_aImageSize[MAX_TEXTURE];       // �e�N�X�`���̃T�C�Y

	float m_TextureU[MAX_TEXTURE]; // �e�N�X�`����U���W
};