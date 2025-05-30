//------------------------------------------
//
// Effectの処理の定義・宣言[effect.h]
// Author: fuma sato
//
//------------------------------------------
#pragma once
#include "main.h"
#include "object2D.h"

//--------------------------
// Effect (末端)
//--------------------------
class CEffect final : public CObject2D
{
// 公開
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

// 非公開
private:
	static constexpr float LIFE = 1.0f;   // ライフ

	static LPDIRECT3DTEXTURE9 m_apTexture; // 共有テクスチャのポインタ
	static D3DXVECTOR2 m_aImageSize;       // テクスチャサイズ
	D3DXVECTOR3 m_DefaultScale;              // 最初の大きさ
	D3DXCOLOR m_color;
	float m_fSpeed;                       // 移動速度
	float m_fLife;                          // バレットのライフ
};