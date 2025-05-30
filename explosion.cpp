//------------------------------------------
//
// Bullet処理 [bullet.cpp]
// Author: fuma sato
//
//------------------------------------------
#include "explosion.h"
#include "renderer.h"
#include "manager.h"
#include "math.h"

//---------------------------------------
//
// Bullet
//
//---------------------------------------

// 静的メンバ変数
const D3DXVECTOR2 CExplosion::m_imageBlock[TYPE_MAX] = { D3DXVECTOR2(8.0f, 1.0f),D3DXVECTOR2(8.0f, 8.0f) };	 // テクスチャブロック数
const float CExplosion::m_AnimationTime = 0.5f;	                                                             // アニメーションタイム

LPDIRECT3DTEXTURE9 CExplosion::m_apTexture[TYPE_MAX] = { NULL }; // 共有テクスチャのポインタ
D3DXVECTOR2 CExplosion::m_aImageSize[TYPE_MAX] = {};             // テクスチャサイズ

//------------------------------
// ソース読み込み
//------------------------------
HRESULT CExplosion::Load(const string sTexturePass[TYPE_MAX])
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャ
	for (size_t cntTex = 0; cntTex < TYPE_MAX; cntTex++)
	{
		D3DXIMAGE_INFO imageInfo = {};
		if (FAILED(D3DXGetImageInfoFromFile
		(
			sTexturePass[cntTex].c_str(),
			&imageInfo
		)))
		{
			return E_FAIL;
		}
		m_aImageSize[cntTex] = D3DXVECTOR2((float)imageInfo.Width, (float)imageInfo.Height);

		if (FAILED(D3DXCreateTextureFromFile
		(
			pDevice,
			sTexturePass[cntTex].c_str(),
			&m_apTexture[cntTex]
		)))
		{
			return E_FAIL;
		}
	}

	return S_OK;
}

//------------------------------
// ソース破棄
//------------------------------
void CExplosion::Unload(void)
{
	// テクスチャ
	for (size_t cntTex = 0; cntTex < TYPE_MAX; cntTex++)
	{
		if (m_apTexture[cntTex] != NULL)
		{
			m_apTexture[cntTex]->Release();
			m_apTexture[cntTex] = NULL;
		}
	}
}

//------------------------------
// 生成
//------------------------------
CExplosion* CExplosion::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, TYPE type, int priority)
{
	CExplosion* pExplosion = new CExplosion(priority); // インスタンス生成

	if (pExplosion == NULL)
	{// 生成失敗
		return NULL;
	}

	pExplosion->SetSize(D3DXVECTOR3(m_aImageSize[type].x, m_aImageSize[type].y, 0.0f)); // サイズの設定

	// 初期化
	if (FAILED(pExplosion->Init(pos, rot, scale, type)))
	{
		delete pExplosion;
		pExplosion = NULL;
		return NULL;
	}

	// テクスチャ登録
	pExplosion->BindTexture(m_apTexture[type]);

	return pExplosion;
}

//------------------------------
//初期化処理
//------------------------------
HRESULT CExplosion::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, TYPE type)
{
	CObject2D::Init(pos, rot, scale, EXPLOSION); // 親の初期化

	// スクリーンサイズの取得
	D3DXVECTOR2 screenSize = {};
	if (FAILED(CManager::GetRenderer()->GetDxScreenSize(&screenSize)))
	{
		return E_FAIL;
	}

	Transform transform = GetTransform(); // 変換情報の取得

	m_type = type; // タイプの設定

	D3DXVECTOR3 size = GetSize();     // サイズの取得
	size.x /= m_imageBlock[m_type].x; // 画像のブロックを考慮
	size.y /= m_imageBlock[m_type].y; // 画像のブロックを考慮
	SetSize(size);                    // テクスチャサイズの設定

	m_nAnimationCount = 0;                   // アニメーションカウント
	m_aniLastTime = CMain::GetElapsedTime(); // アニメーション更新時間

	// 頂点情報の設定
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff(); // 頂点バッファの取得
	Vertex* pVtx;
	if (FAILED(pVtxBuff->Lock(0, 0, (void**)&pVtx, 0))) { return E_FAIL; }

	D3DXVECTOR3 screenPos = D3DXVECTOR3(transform.pos.x * screenSize.x, transform.pos.y * screenSize.y, 0.0f);

	size = GetSize(); // テクスチャサイズの取得
	size.x *= transform.scale.x;
	size.y *= transform.scale.y;

	D3DXVECTOR3 resultPos[VT_DEF] = {};
	CMath::Rotation(resultPos, D3DXVECTOR2(size.x, size.y), transform.rot.z);

	for (size_t cntVtx = 0; cntVtx < VT_DEF; cntVtx++)
	{
		resultPos[cntVtx] += screenPos;

		pVtx[cntVtx].pos = resultPos[cntVtx];
		pVtx[cntVtx].rhw = 1.0f;
		pVtx[cntVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[cntVtx].tex = D3DXVECTOR2((float)(cntVtx % 2) * (1.0f / m_imageBlock[m_type].x) + (float)(m_nAnimationCount % (int)(m_imageBlock[m_type].x)) * (1.0f / m_imageBlock[m_type].x), (float)(cntVtx / 2) * (1.0f / m_imageBlock[m_type].y) + (float)(m_nAnimationCount / (int)(m_imageBlock[m_type].x)) * (1.0f / m_imageBlock[m_type].y));
	}

	if (FAILED(pVtxBuff->Unlock())) { return E_FAIL; }

	return S_OK;
}

//------------------------------
//終了処理
//------------------------------
void CExplosion::Uninit(void)
{
	CObject2D::Uninit(); // 親の終了
}

//------------------------------
//更新処理
//------------------------------
void CExplosion::Update(void)
{
	CObject2D::Update(); // 親の更新

	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice(); // レンダラーからデバイスを取得

	float deltaTime = CMain::GetDeltaTimeGameSpeed();

	if (m_nAnimationCount >= (int)(m_imageBlock[m_type].x * m_imageBlock[m_type].y))
	{
		Release();
		return;
	}

	// スクリーンサイズの取得
	D3DXVECTOR2 screenSize = {};
	if (FAILED(pRenderer->GetDxScreenSize(&screenSize)))
	{
		return;
	}

	Transform transform = GetTransform(); // 変換情報の取得

	// 頂点情報の設定
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff(); // 頂点バッファの取得
	Vertex* pVtx;
	if (FAILED(pVtxBuff->Lock(0, 0, (void**)&pVtx, 0))) { return; }

	// 回転関係処理y
	D3DXVECTOR3 screenPos = D3DXVECTOR3(transform.pos.x * screenSize.x, transform.pos.y * screenSize.y, 0.0f);
	D3DXVECTOR3 resultPos[VT_DEF] = {};

	D3DXVECTOR3 size = GetSize(); // テクスチャサイズの取得
	size.x *= transform.scale.x;
	size.y *= transform.scale.y;
	CMath::Rotation(resultPos, D3DXVECTOR2(size.x, size.y), transform.rot.z);

	if ((CMain::GetElapsedTime() - m_aniLastTime) >= m_AnimationTime / (m_imageBlock[m_type].x * m_imageBlock[m_type].y))
	{// アニメーションの更新
		m_nAnimationCount++;

		m_aniLastTime = CMain::GetElapsedTime();
	}

	// 頂点情報の設定
	for (size_t cntVtx = 0; cntVtx < VT_DEF; cntVtx++)
	{
		resultPos[cntVtx] += screenPos;

		pVtx[cntVtx].pos = resultPos[cntVtx];
		pVtx[cntVtx].rhw = 1.0f;
		pVtx[cntVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[cntVtx].tex = D3DXVECTOR2((float)(cntVtx % 2) * (1.0f / m_imageBlock[m_type].x) + (float)(m_nAnimationCount % (int)(m_imageBlock[m_type].x)) * (1.0f / m_imageBlock[m_type].x), (float)(cntVtx / 2) * (1.0f / m_imageBlock[m_type].y) + (float)(m_nAnimationCount / (int)(m_imageBlock[m_type].x)) * (1.0f / m_imageBlock[m_type].y));
	}

	if (FAILED(pVtxBuff->Unlock())) { return; }
}

//------------------------------
//描画処理
//------------------------------
void CExplosion::Draw(void)
{
	CObject2D::Draw(); // 親の描画
}