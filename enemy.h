//------------------------------------------
//
// enemyの処理の定義・宣言[enemy.h]
// Author: fuma sato
//
//------------------------------------------
#pragma once
#include "main.h"
#include "object2D.h"

//--------------------------
// バレット (末端)
//--------------------------
class CEnemy final : public CObject2D
{
// 公開
public:
	using TYPE = enum
	{
		DEFAULT = 0, // デフォルト
		EXTRA,       // 追加
		TYPE_MAX     // 最大
	};

	// 状態管理
	using STATE = enum
	{
		NONE = 0, // 無
		NORMAL,   // 通常
		DAMAGE,   // ダメージ
		DEATH,    // 死
		STATE_MAX // 状態数
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

// 非公開
private:
	void OnCollision(CObject* pOther) override;
	void Hit(const int damage);

	static constexpr int LIFE = 120; // ライフ
	static constexpr float DAMAGE_TIME = 0.2f; // ダメージ時間
	static constexpr float DEATH_TIME = 0.1f;  // 死時間
	static constexpr float ATTACK_TIME = 2.0f; // 攻撃間隔

	static LPDIRECT3DTEXTURE9 m_apTexture[TYPE_MAX]; // 共有テクスチャのポインタ
	static D3DXVECTOR2 m_aImageSize[TYPE_MAX];       // テクスチャサイズ

	static int m_nNumAll;                   // 出現数

	STATE m_state;                          // 状態
	float m_StateTime;                      // 状態タイマー
	float m_AttackTime;                     // 攻撃タイマー

	float m_fSpeed;                       // 移動速度
	TYPE m_type;                          // タイプ
	int m_nLife;                          // 体力
};