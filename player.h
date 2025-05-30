//------------------------------------------
//
// Playerの処理の定義・宣言[player.h]
// Author: fuma sato
//
//------------------------------------------
#pragma once
#include "main.h"
#include "object2D.h"

//--------------------------
// プレイヤークラス (末端)
//--------------------------
class CPlayer final : public CObject2D
{
// 公開
public:
	// 状態管理
	using STATE = enum
	{
		NONE = 0, // 無
		APPEAR,   // 出現
		NORMAL,   // 通常
		DAMAGE,   // ダメージ
		DEATH,    // 死
		STATE_MAX // 状態数
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

// 非公開
private:
	void OnCollision(CObject* pOther) override;
	void Hit(int damage);

	static constexpr int LIFE = 200;           // ライフ
	static constexpr float APPEAR_TIME = 2.0f; // 出現時間
	static constexpr float DAMAGE_TIME = 0.5f; // ダメージ時間
	static constexpr float DEATH_TIME = 2.0f;  // 死時間

	static const D3DXVECTOR2 m_imageBlock;	 // テクスチャブロック数
	static const float m_AnimationTime;	     // アニメーションタイム

	static LPDIRECT3DTEXTURE9 m_pTexture;    // 共有テクスチャのポインタ
	static D3DXVECTOR2 m_imageSize;          // テクスチャサイズ

	STATE m_state;                          // 状態
	float m_StateTime;                      // 状態タイマー
	int m_nLife;                            // ライフ

	float m_fSpeed;                         // 移動速度
	float m_fRotSpeed;                      // 回転速度

	int m_nAnimationCount;              // アニメーションカウント
	float m_aniLastTime;                // アニメーション更新タイム
};