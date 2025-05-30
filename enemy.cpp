//------------------------------------------
//
// enemy処理 [enemy.cpp]
// Author: fuma sato
//
//------------------------------------------
#include "enemy.h"
#include "renderer.h"
#include "manager.h"
#include "math.h"
#include "explosion.h"
#include "particle.h"
#include "bullet.h"
#include "debug.h"

//---------------------------------------
//
// Enemy
//
//---------------------------------------

// 静的メンバ変数
LPDIRECT3DTEXTURE9 CEnemy::m_apTexture[TYPE_MAX] = { NULL }; // 共有テクスチャのポインタ
D3DXVECTOR2 CEnemy::m_aImageSize[TYPE_MAX] = {};             // テクスチャサイズ
int CEnemy::m_nNumAll = 0;                                   // 出現数

//------------------------------
// ソース読み込み
//------------------------------
HRESULT CEnemy::Load(const string sTexturePass[TYPE_MAX])
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
void CEnemy::Unload(void)
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
CEnemy* CEnemy::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fSpeed, TYPE type, int priority)
{
	CEnemy* pBullet = new CEnemy(priority); // インスタンス生成

	if (pBullet == NULL)
	{// 生成失敗
		return NULL;
	}

	pBullet->SetSize(D3DXVECTOR3(m_aImageSize[type].x, m_aImageSize[type].y, 0.0f)); // サイズの設定

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
HRESULT CEnemy::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fSpeed, TYPE type)
{
	CObject2D::Init(pos, rot, scale, ENEMY); // 親の初期化

	IsCollision(true); // 当たり判定をする	

	// スクリーンサイズの取得
	D3DXVECTOR2 screenSize = {};
	if (FAILED(CManager::GetRenderer()->GetDxScreenSize(&screenSize)))
	{
		return E_FAIL;
	}

	m_nLife = LIFE;                // ライフの初期化
	m_state = NORMAL;              // 初期化
	m_StateTime = 0.0f;  // 出現時間
	m_AttackTime = 0.0f; // 攻撃時間

	m_fSpeed = fSpeed;             // ポリゴンの移動速度
	m_type = type;                 // 敵の種類

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
	CMath::Rotation(resultPos, D3DXVECTOR2(size.x, size.y), GetRotation().z);

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
void CEnemy::Uninit(void)
{
	CObject2D::Uninit(); // 親の終了
}

//------------------------------
//更新処理
//------------------------------
void CEnemy::Update(void)
{
	CObject2D::Update(); // 親の更新

	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice(); // レンダラーからデバイスを取得

	float deltaTime = CMain::GetDeltaTimeGameSpeed();

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

	Transform transform = GetTransform(); // 変形情報の取得

	// 回転関係処理
	D3DXVECTOR3 screenPos = D3DXVECTOR3(transform.pos.x * screenSize.x, transform.pos.y * screenSize.y, 0.0f);
	D3DXVECTOR3 resultPos[VT_DEF] = {};

	D3DXVECTOR3 size = GetSize(); // テクスチャサイズの取得
	size.x *= transform.scale.x;
	size.y *= transform.scale.y;
	CMath::Rotation(resultPos, D3DXVECTOR2(size.x, size.y), transform.rot.z);

	// 頂点情報の設定
	for (size_t cntVtx = 0; cntVtx < VT_DEF; cntVtx++)
	{
		resultPos[cntVtx] += screenPos;

		pVtx[cntVtx].pos = resultPos[cntVtx];
		pVtx[cntVtx].rhw = 1.0f;
		pVtx[cntVtx].col = m_state == DAMAGE ? RED : WHITE; // ダメージ状態は赤
		pVtx[cntVtx].tex = D3DXVECTOR2((float)(cntVtx % 2) * 1.0f, (float)(cntVtx / 2) * 1.0f);
	}

	if (FAILED(pVtxBuff->Unlock())) { return; }

	// 状態管理
	switch (m_state)
	{
	case NONE:
		break;
	case NORMAL:
		if ((m_AttackTime += CMain::GetDeltaTimeGameSpeed()) >= ATTACK_TIME)
		{// 一定時間経過
			size_t randomNum = CMath::Random(1, 2);
			float angleSet = 2.0f * D3DX_PI / (float)randomNum; // 弾の角度間隔
			for (size_t cntBullet = 0; cntBullet < randomNum; cntBullet++)
			{
				float angle = transform.rot.z - cntBullet * angleSet;
				CBullet::Create(transform.pos, D3DXVECTOR3(0.0f, 0.0f, atan2f(-cosf(angle), -sinf(angle))), transform.scale * 2.0f, CMath::Random(0.05f, 0.5f), CBullet::ENEMY, 2);
			}
			m_AttackTime = 0.0f; // 戻す
		}
		break;
	case DAMAGE:
		if ((m_StateTime += CMain::GetDeltaTimeGameSpeed()) >= DAMAGE_TIME)
		{// 一定時間経過
			m_state = NORMAL; // 通常状態
			m_StateTime = 0.0f; // 戻す
		}
		break;
	case DEATH:
		if ((m_StateTime += CMain::GetDeltaTimeGameSpeed()) >= DEATH_TIME)
		{// 一定時間経過
			Release();
			return;
		}
		break;
	}

	CDebugProc::Print("Enemy%d:%d\n", GetID(), m_state);
	CDebugProc::Print("Enemy%d 状態カウンター:%f\n", GetID(), m_StateTime);
	CDebugProc::Print("Enemy%d 攻撃カウンター:%f\n", GetID(), m_AttackTime);
}

//------------------------------
//描画処理
//------------------------------
void CEnemy::Draw(void)
{
	CObject2D::Draw(); // 親の描画
}

//------------------------------
// 衝突処理
//------------------------------
void CEnemy::OnCollision(CObject* pOther)
{
	if (m_state == NORMAL || m_state == DAMAGE)
	{// 無敵時間でない
		if (pOther->GetType() == BULLET)
		{// 弾との衝突
			if (dynamic_cast<CBullet*>(pOther)->GetType() == CBullet::PLAYER)
			{// 弾のタイプがプレイヤー
				Hit(10); // ヒット処理
			}
		}
	}
}

//--------------------------
// ダメージ処理
//--------------------------
void CEnemy::Hit(int damage)
{
	m_nLife -= damage; // ダメージ
	if (m_nLife <= 0)
	{// 体力がなくなった
		m_state = DEATH; // 死

		Transform transform = GetTransform();
		CExplosion::Create(transform.pos, transform.rot, transform.scale * 100.0f, CMath::Random(CExplosion::DEFAULT, CExplosion::EXTRA), 4); // 爆発エフェクトを生成
	}
	else
	{
		m_state = DAMAGE; // ダメージ
	}
	m_StateTime = 0.0f; // 戻す
}