//------------------------------------------
//
// Bullet処理 [bullet.cpp]
// Author: fuma sato
//
//------------------------------------------
#include "bullet.h"
#include "renderer.h"
#include "manager.h"
#include "math.h"
#include "explosion.h"
#include "effect.h"
#include "particle.h"
#include "debug.h"

//---------------------------------------
//
// Bullet
//
//---------------------------------------

// 静的メンバ変数
LPDIRECT3DTEXTURE9 CBullet::m_apTexture[TYPE_MAX] = { NULL }; // 共有テクスチャのポインタ
D3DXVECTOR2 CBullet::m_aImageSize[TYPE_MAX] = {};             // テクスチャサイズ

//------------------------------
// ソース読み込み
//------------------------------
HRESULT CBullet::Load(const string sTexturePass[TYPE_MAX])
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

	m_aImageSize[1] *= 0.2f; // 小さく
	return S_OK;
}

//------------------------------
// ソース破棄
//------------------------------
void CBullet::Unload(void)
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
CBullet* CBullet::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fSpeed, TYPE type, int priority)
{
	CBullet* pBullet = new CBullet(priority); // インスタンス生成

	if (pBullet == NULL)
	{// 生成失敗
		return NULL;
	}

	D3DXVECTOR3 imageSize = D3DXVECTOR3(m_aImageSize[type].x, m_aImageSize[type].y, 0.0f); // テクスチャサイズの取得
	pBullet->SetSize(imageSize); // テクスチャサイズの設定

	// 初期化
	if (FAILED(pBullet->Init(pos, rot, scale, fSpeed, type)))
	{
		delete pBullet;
		pBullet = NULL;
		return NULL;
	}

	// テクスチャ登録
	pBullet->BindTexture(m_apTexture[type]);

	return pBullet;
}

//------------------------------
//初期化処理
//------------------------------
HRESULT CBullet::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fSpeed, TYPE type)
{
	CObject2D::Init(pos, rot, scale, BULLET); // 親の初期化

	IsCollision(true); // 当たり判定をする

	// スクリーンサイズの取得
	D3DXVECTOR2 screenSize = {};
	if (FAILED(CManager::GetRenderer()->GetDxScreenSize(&screenSize)))
	{
		return E_FAIL;
	}

	m_fSpeed = fSpeed;             // 移動速度
	m_type = type;                 // 種類
	m_fLife = LIFE;                // ライフ
	m_EffectTIme = 0.0f;           // エフェクト時間

	// 頂点情報の設定
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();
	Vertex* pVtx;
	if (FAILED(pVtxBuff->Lock(0, 0, (void**)&pVtx, 0))) { return E_FAIL; }

	Transform transform = GetTransform();
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
		pVtx[cntVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[cntVtx].tex = D3DXVECTOR2((float)(cntVtx % 2) * 1.0f, (float)(cntVtx / 2) * 1.0f);
	}

	if (FAILED(pVtxBuff->Unlock())) { return E_FAIL; }

	return S_OK;
}

//------------------------------
//終了処理
//------------------------------
void CBullet::Uninit(void)
{
	CObject2D::Uninit(); // 親の終了
}

//------------------------------
//更新処理
//------------------------------
void CBullet::Update(void)
{
	if (m_fLife <= 0.0f)
	{// ライフが0以下ならば爆発して削除
		Transform transform = GetTransform();
		CParticle::Create(transform.pos, transform.rot, transform.scale * 10.0f);
		Release();
		return;
	}

	CObject2D::Update(); // 親の更新

	float deltaTime = CMain::GetDeltaTimeGameSpeed();

	m_fLife -= deltaTime; // ライフを減らす

	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice(); // レンダラーからデバイスを取得

	Transform transform = GetTransform(); // 変形情報の取得
	D3DXVECTOR3 move = { 0.0f,0.0f,0.0f };
	move.x += m_fSpeed * sinf(transform.rot.z) * deltaTime;
	move.y += m_fSpeed * cosf(transform.rot.z) * deltaTime;
	AddPosition(move);

	transform = GetTransform(); // 変形情報の取得
	if (transform.pos.x < 0.0f || transform.pos.x>1.0f || transform.pos.y < 0.0f || transform.pos.y>1.0f)
	{
		CParticle::Create(transform.pos, transform.rot, transform.scale * 10.0f);
		Release();
		return;
	}

	if ((m_EffectTIme += CMain::GetDeltaTimeGameSpeed()) >= EFFECT_TIME)
	{// エフェクト時間がたったら
		// エフェクト生成
		D3DXVECTOR3 EffectSize{};
		EffectSize.x = transform.scale.x * GetSize().x;
		EffectSize.y = transform.scale.y * GetSize().y;
		CEffect::Create(transform.pos, transform.rot, EffectSize * 0.05f, 1);

		m_EffectTIme = 0.0f; // 戻す
	}

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
		pVtx[cntVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[cntVtx].tex = D3DXVECTOR2((float)(cntVtx % 2) * 1.0f, (float)(cntVtx / 2) * 1.0f);
	}

	if (FAILED(pVtxBuff->Unlock())) { return; }
}

//------------------------------
//描画処理
//------------------------------
void CBullet::Draw(void)
{
	CObject2D::Draw(); // 親の描画
}

//------------------------------
// 衝突処理
//------------------------------
void CBullet::OnCollision(CObject* pOther)
{
	CObject::TYPE Type = pOther->GetType();
	if ((m_type == CBullet::PLAYER && Type == CObject::ENEMY) || (m_type == CBullet::ENEMY && Type == CObject::PLAYER))
	{// 当たった場合
		Hit(); // 敵に当たった処理

		CDebugProc::Print("BulletHit 撃った人:%d", m_type);
		CDebugProc::Print("BulletHit 当たった人:%d", Type);
	}
}