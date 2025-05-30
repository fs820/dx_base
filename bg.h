//------------------------------------------
//
// Playerの処理の定義・宣言[player.h]
// Author: fuma sato
//
//------------------------------------------
#pragma once
#include "main.h"
#include "object.h"

class CObject2D; // 前方宣言

using namespace DirectX2D; // DirectX2D名前空間の使用

//--------------------------
// プレイヤークラス
//--------------------------
class CBg final : public CObject
{
	// 公開
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

	// 非公開
private:
	void OnCollision(CObject* pOther) override {};

	static constexpr int MAX_TEXTURE = 3;            // 背景の最大数

	static const string m_sTexturePass[MAX_TEXTURE]; // テクスチャのパス
	static const float m_afScrollSpeed[MAX_TEXTURE]; // スクロール速度

	static CObject2D* m_pBg[MAX_TEXTURE];  // 背景のポインタ配列

	static LPDIRECT3DTEXTURE9 m_apTexture[MAX_TEXTURE]; // テクスチャのポインタ配列
	static D3DXVECTOR2 m_aImageSize[MAX_TEXTURE];       // テクスチャのサイズ

	float m_TextureU[MAX_TEXTURE]; // テクスチャのU座標
};