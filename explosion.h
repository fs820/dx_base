//------------------------------------------
//
// Bulletの処理の定義・宣言[explosion.h]
// Author: fuma sato
//
//------------------------------------------
#pragma once
#include "main.h"
#include "object2D.h"

//--------------------------
// バレット (末端)
//--------------------------
class CExplosion final : public CObject2D
{
	// 公開
public:
	using TYPE = enum
	{
		DEFAULT = 0, // デフォルト
		EXTRA,       // 追加
		TYPE_MAX     // 最大
	};

	CExplosion() : m_nAnimationCount{}, m_aniLastTime{}, m_type{} {}
	CExplosion(int priority) : CObject2D(priority), m_nAnimationCount{}, m_aniLastTime{}, m_type{} {}
	~CExplosion() = default;

	static HRESULT Load(const string sTexturePass[TYPE_MAX]);
	static void Unload(void);
	static CExplosion* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, TYPE type, int priority = 3);

	HRESULT Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, TYPE type);
	void Uninit(void) override;
	void Update(void) override;
	void Draw(void) override;

// 非公開
private:
	void OnCollision(CObject* pOther) override {};

	static const D3DXVECTOR2 m_imageBlock[TYPE_MAX]; // テクスチャブロック数
	static const float m_AnimationTime;	             // アニメーションタイム

	static LPDIRECT3DTEXTURE9 m_apTexture[TYPE_MAX]; // 共有テクスチャのポインタ
	static D3DXVECTOR2 m_aImageSize[TYPE_MAX];       // テクスチャサイズ

	int m_nAnimationCount;              // アニメーションカウント
	float m_aniLastTime;                // アニメーション更新タイム

	TYPE m_type;                       // タイプ
};