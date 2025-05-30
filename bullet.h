//------------------------------------------
//
// Bulletの処理の定義・宣言[bullet.h]
// Author: fuma sato
//
//------------------------------------------
#pragma once
#include "main.h"
#include "object2D.h"

//--------------------------
// バレット (末端)
//--------------------------
class CBullet final : public CObject2D
{
// 公開
public:
	using TYPE = enum
	{
		PLAYER = 0, // プレイヤー
		ENEMY,      // エネミー
		TYPE_MAX    // 最大
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

// 非公開
private:
	void OnCollision(CObject* pOther) override;
	void Hit(void) { m_fLife = 0; }

	static constexpr float LIFE = 20.0f;         // ライフ
	static constexpr float EFFECT_TIME = 0.001f; // エフェクト間隔

	static LPDIRECT3DTEXTURE9 m_apTexture[TYPE_MAX]; // 共有テクスチャのポインタ
	static D3DXVECTOR2 m_aImageSize[TYPE_MAX];       // テクスチャサイズ

	float m_fSpeed;                       // 移動速度
	TYPE m_type;                          // バレットの種類
	float m_fLife;                          // バレットのライフ
	float m_EffectTIme;                   // エフェクトタイマー
};