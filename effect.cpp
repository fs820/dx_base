//------------------------------------------
//
// Effect処理 [effect.cpp]
// Author: fuma sato
//
//------------------------------------------
#include "effect.h"
#include "renderer.h"
#include "manager.h"

//---------------------------------------
//
// Effect
//
//---------------------------------------

// 静的メンバ変数
LPDIRECT3DTEXTURE9 CEffect::m_apTexture = nullptr; // 共有テクスチャのポインタ
D3DXVECTOR2 CEffect::m_aImageSize = {};            // テクスチャサイズ

//------------------------------
// ソース読み込み
//------------------------------
HRESULT CEffect::Load(const string sTexturePass)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// テクスチャ
	D3DXIMAGE_INFO imageInfo = {};
	if (FAILED(D3DXGetImageInfoFromFile
	(
		sTexturePass.c_str(),
		&imageInfo
	)))
	{
		return E_FAIL;
	}
	m_aImageSize = D3DXVECTOR2((float)imageInfo.Width, (float)imageInfo.Height);

	if (FAILED(D3DXCreateTextureFromFile
	(
		pDevice,
		sTexturePass.c_str(),
		&m_apTexture
	)))
	{
		return E_FAIL;
	}

	return S_OK;
}

//------------------------------
// ソース破棄
//------------------------------
void CEffect::Unload(void)
{
	// テクスチャ
	if (m_apTexture != NULL)
	{
		m_apTexture->Release();
		m_apTexture = NULL;
	}
}

//------------------------------
// 生成
//------------------------------
CEffect* CEffect::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, int priority, D3DXCOLOR color, float fSpeed)
{
	CEffect* pEffect = new CEffect(priority); // インスタンス生成

	if (pEffect == NULL)
	{// 生成失敗
		return NULL;
	}

	D3DXVECTOR3 imageSize = D3DXVECTOR3(m_aImageSize.x, m_aImageSize.y, 0.0f); // テクスチャサイズの取得
	pEffect->SetSize(imageSize); // テクスチャサイズの設定

	// 初期化
	if (FAILED(pEffect->Init(pos, rot, scale, color, fSpeed)))
	{
		delete pEffect;
		pEffect = NULL;
		return NULL;
	}

	// テクスチャ登録
	pEffect->BindTexture(m_apTexture);

	return pEffect;
}

//------------------------------
//初期化処理
//------------------------------
HRESULT CEffect::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, D3DXCOLOR color, float fSpeed)
{
	CObject2D::Init(pos, rot, scale, EFFECT); // 親の初期化

	// スクリーンサイズの取得
	D3DXVECTOR2 screenSize = {};
	if (FAILED(CManager::GetRenderer()->GetDxScreenSize(&screenSize)))
	{
		return E_FAIL;
	}

	m_color = color;
	m_fSpeed = fSpeed;             // 移動速度
	m_fLife = LIFE;                 // ライフ

	// 頂点情報の設定
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();
	Vertex* pVtx;
	if (FAILED(pVtxBuff->Lock(0, 0, (void**)&pVtx, 0))) { return E_FAIL; }

	Transform transform = GetTransform();
	m_DefaultScale = transform.scale;

	D3DXVECTOR3 screenPos = D3DXVECTOR3(transform.pos.x * screenSize.x, transform.pos.y * screenSize.y, 0.0f);

	D3DXVECTOR3 resultPos[VT_DEF] = {};
	D3DXVECTOR3 size = GetSize(); // サイズの取得
	size.x *= transform.scale.x;
	size.y *= transform.scale.y;

	resultPos[0] = D3DXVECTOR3(-size.x * 0.5f, -size.y * 0.5f, 0.0f);
	resultPos[1] = D3DXVECTOR3(size.x * 0.5f, -size.y * 0.5f, 0.0f);
	resultPos[2] = D3DXVECTOR3(-size.x * 0.5f, size.y * 0.5f, 0.0f);
	resultPos[3] = D3DXVECTOR3(size.x * 0.5f, size.y * 0.5f, 0.0f);

	for (size_t cntVtx = 0; cntVtx < VT_DEF; cntVtx++)
	{
		resultPos[cntVtx] += screenPos;

		pVtx[cntVtx].pos = resultPos[cntVtx];
		pVtx[cntVtx].rhw = 1.0f;
		pVtx[cntVtx].col = m_color;
		pVtx[cntVtx].tex = D3DXVECTOR2((float)(cntVtx % 2) * 1.0f, (float)(cntVtx / 2) * 1.0f);
	}

	if (FAILED(pVtxBuff->Unlock())) { return E_FAIL; }

	return S_OK;
}

//------------------------------
//終了処理
//------------------------------
void CEffect::Uninit(void)
{
	CObject2D::Uninit(); // 親の終了
}

//------------------------------
//更新処理
//------------------------------
void CEffect::Update(void)
{
	if (m_fLife <= 0.0f)
	{// ライフが0以下ならば削除
		Release();
		return;
	}

	CObject2D::Update(); // 親の更新

	float deltaTime = CMain::GetDeltaTimeGameSpeed();

	m_fLife -= deltaTime; // ライフを減らす

	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice(); // レンダラーからデバイスを取得

	Transform transform = GetTransform(); // 変形情報の取得
	transform.pos.x += m_fSpeed * sinf(transform.rot.z) * deltaTime;
	transform.pos.y += m_fSpeed * cosf(transform.rot.z) * deltaTime;
	transform.pos.z += 0.0f;

	transform.scale.x = m_DefaultScale.x * (m_fLife / LIFE);
	transform.scale.y = m_DefaultScale.y * (m_fLife / LIFE);
	transform.scale.z += 1.0f;

	SetTransform(transform);

	// スクリーンサイズの取得
	D3DXVECTOR2 screenSize = {};
	if (FAILED(pRenderer->GetDxScreenSize(&screenSize)))
	{
		return;
	}

	// 頂点情報の設定
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();
	Vertex* pVtx;
	if (FAILED(pVtxBuff->Lock(0, 0, (void**)&pVtx, 0))) { return; }

	// 回転関係処理y
	D3DXVECTOR3 screenPos = D3DXVECTOR3(transform.pos.x * screenSize.x, transform.pos.y * screenSize.y, 0.0f);

	D3DXVECTOR3 resultPos[VT_DEF] = {};
	D3DXVECTOR3 size = GetSize(); // サイズの取得
	size.x *= transform.scale.x;
	size.y *= transform.scale.y;

	resultPos[0] = D3DXVECTOR3(-size.x * 0.5f, -size.y * 0.5f, 0.0f);
	resultPos[1] = D3DXVECTOR3(size.x * 0.5f, -size.y * 0.5f, 0.0f);
	resultPos[2] = D3DXVECTOR3(-size.x * 0.5f, size.y * 0.5f, 0.0f);
	resultPos[3] = D3DXVECTOR3(size.x * 0.5f, size.y * 0.5f, 0.0f);

	// 頂点情報の設定
	for (size_t cntVtx = 0; cntVtx < VT_DEF; cntVtx++)
	{
		resultPos[cntVtx] += screenPos;

		pVtx[cntVtx].pos = resultPos[cntVtx];
		pVtx[cntVtx].rhw = 1.0f;
		pVtx[cntVtx].col = m_color;
		pVtx[cntVtx].tex = D3DXVECTOR2((float)(cntVtx % 2) * 1.0f, (float)(cntVtx / 2) * 1.0f);
	}

	if (FAILED(pVtxBuff->Unlock())) { return; }
}

//------------------------------
//描画処理
//------------------------------
void CEffect::Draw(void)
{
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	//αブレンディングを加算合成に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	CObject2D::Draw(); // 親の描画

	//αブレンディングを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}