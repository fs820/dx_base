//------------------------------------------
//
// 2D描画用の処理[object2D.cpp]
// Author: fuma sato
//
//------------------------------------------
#include "object2D.h"
#include "renderer.h"
#include "manager.h"

//---------------------------------------
//
// オブジェクト2Dクラス
//
//---------------------------------------

//------------------------------
// 生成
//------------------------------
CObject2D* CObject2D::Create(LPDIRECT3DTEXTURE9 pTexture, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, TYPE type, int priority)
{
	CObject2D* pObject2D = new CObject2D(priority); // インスタンス生成

	if (pObject2D == NULL)
	{// 生成失敗
		return NULL;
	}

	// 初期化
	if (FAILED(pObject2D->Init(pos, rot, scale, type)))
	{
		delete pObject2D;
		pObject2D = NULL;
		return NULL;
	}

	// テクスチャの登録
	pObject2D->BindTexture(pTexture);

	return pObject2D;
}

//------------------------------
// 生成
//------------------------------
CObject2D* CObject2D::Create(LPDIRECT3DTEXTURE9 pTexture, Transform transform, TYPE type, int priority)
{
	CObject2D* pObject2D = new CObject2D(priority); // インスタンス生成

	if (pObject2D == NULL)
	{// 生成失敗
		return NULL;
	}

	// 初期化
	if (FAILED(pObject2D->Init(transform, type)))
	{
		delete pObject2D;
		pObject2D = NULL;
		return NULL;
	}

	// テクスチャの登録
	pObject2D->BindTexture(pTexture);

	return pObject2D;
}

//------------------------------
//初期化処理
//------------------------------
HRESULT CObject2D::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, TYPE type)
{
	CObject::Init(type); // 親クラスの初期化

	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice(); // レンダラーからデバイスを取得

	if (pDevice == NULL)
	{
		return E_POINTER;
	}

	// スクリーンサイズの取得
	D3DXVECTOR2 screenSize = {};
	if (FAILED(pRenderer->GetDxScreenSize(&screenSize)))
	{
		return E_FAIL;
	}

	SetTransform(Transform(pos, rot, scale));

	//バッファーの設定
	if (FAILED(pDevice->CreateVertexBuffer
	(
		sizeof(Vertex) * VT_DEF,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL
	)))
	{
		return E_FAIL;
	}

	return S_OK;
}

//------------------------------
//初期化処理
//------------------------------
HRESULT CObject2D::Init(Transform transform, TYPE type)
{
	CObject::Init(type); // 親クラスの初期化

	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice(); // レンダラーからデバイスを取得

	if (pDevice == NULL)
	{
		return E_POINTER;
	}

	// スクリーンサイズの取得
	D3DXVECTOR2 screenSize = {};
	if (FAILED(pRenderer->GetDxScreenSize(&screenSize)))
	{
		return E_FAIL;
	}

	SetTransform(transform);

	//バッファーの設定
	if (FAILED(pDevice->CreateVertexBuffer
	(
		sizeof(Vertex) * VT_DEF,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL
	)))
	{
		return E_FAIL;
	}

	return S_OK;
}

//------------------------------
//終了処理
//------------------------------
void CObject2D::Uninit(void)
{
	// 頂点バッファの解放
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}
}

//------------------------------
//更新処理
//------------------------------
void CObject2D::Update(void)
{

}

//------------------------------
//描画処理
//------------------------------
void CObject2D::Draw(void)
{
	CRenderer* pRenderer = CManager::GetRenderer();               // レンダラーの取得
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice(); // レンダラーからデバイスを取得

	if (pDevice == NULL || m_pVtxBuff == NULL)
	{
		return;
	}

	// 頂点バッファ
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(Vertex));

	// 頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX);

	// テクスチャの設定
	pDevice->SetTexture(0, m_pTexture);

	// ポリゴンの描画
	pDevice->DrawPrimitive
	(
		D3DPT_TRIANGLESTRIP,//タイプ
		0,                  // インデックス
		2                   //ポリゴン数
	);
}