//---------------------------------------
//
//パーティクル表示処理の定義・宣言[particle.h]
//Author fuma sato
//
//---------------------------------------
#pragma once

#include "main.h"
#include "object.h"

using namespace DirectX2D; // DirectX2D名前空間の使用

//--------------------------
// Particle (末端)
//--------------------------
class CParticle final : public CObject
{
// 公開
public:
	CParticle() : m_nLife{} {};
	CParticle(int priority) : CObject(priority), m_nLife{} {}
	~CParticle() = default;

	static HRESULT Load(const string sTexturePass);
	static void Unload(void);
	static CParticle* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, int priority = 3);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

// 非公開
private:
	void OnCollision(CObject* pOther) override {};

	static constexpr float LIFE = 0.5f;      // 持続時間
	static constexpr float INTERVAL = 0.01f; // 発生間隔
	static constexpr int NUMBER = 1;         // 一度に出す数

	float m_nLife;
};