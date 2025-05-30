//------------------------------------------
//
// プレイヤーの処理 [player.cpp]
// Author: fuma sato
//
//------------------------------------------
#include "player.h"
#include "renderer.h"
#include "manager.h"
#include "math.h"
#include "bullet.h"
#include "explosion.h"
#include "input.h"
#include "particle.h"
#include "debug.h"

//---------------------------------------
//
// プレイヤークラス
//
//---------------------------------------

// 静的メンバ変数の定義
const D3DXVECTOR2 CPlayer::m_imageBlock = D3DXVECTOR2(1.0f, 1.0f);              // 画像のブロック
const float CPlayer::m_AnimationTime = 0.05f;                                   // アニメーションタイム
LPDIRECT3DTEXTURE9 CPlayer::m_pTexture = NULL;                                  // 共有テクスチャのポインタ
D3DXVECTOR2 CPlayer::m_imageSize = {};                                          // テクスチャサイズ

//-----------------------------
// ソース読み込み
//-----------------------------
HRESULT CPlayer::Load(const string sTexturePass)
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
	m_imageSize = D3DXVECTOR2((float)imageInfo.Width, (float)imageInfo.Height);

	if (FAILED(D3DXCreateTextureFromFile
	(
		pDevice,
		sTexturePass.c_str(),
		&m_pTexture
	)))
	{
		return E_FAIL;
	}

	return S_OK;
}

//-----------------------------
// ソース破棄
//-----------------------------
void CPlayer::Unload(void)
{
	// テクスチャ
	if (m_pTexture != NULL)
	{
		m_pTexture->Release();
		m_pTexture = NULL;
	}
}

//------------------------------
// 生成
//------------------------------
CPlayer* CPlayer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fSpeed, float fRotSpeed, int priority)
{
	CPlayer* pPlayer = new CPlayer(priority); // インスタンス生成

	if (pPlayer == NULL)
	{// 生成失敗
		return NULL;
	}

	pPlayer->SetSize(D3DXVECTOR3(m_imageSize.x, m_imageSize.y, 0.0f)); // サイズの設定

	// 初期化
	if (FAILED(pPlayer->Init(pos, rot, scale, fSpeed, fRotSpeed)))
	{
		delete pPlayer;
		pPlayer = NULL;
		return NULL;
	}

	// テクスチャの登録
	pPlayer->BindTexture(m_pTexture);

	return pPlayer;
}

//------------------------------
//初期化処理
//------------------------------
HRESULT CPlayer::Init(D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 scale, float fSpeed, float fRotSpeed)
{
	CObject2D::Init(pos, rot, scale, PLAYER); // 親の初期化

	IsCollision(true); // 当たり判定をする	

	// スクリーンサイズの取得
	D3DXVECTOR2 screenSize = {};
	if (FAILED(CManager::GetRenderer()->GetDxScreenSize(&screenSize)))
	{
		return E_FAIL;
	}

	Transform transform = GetTransform();

	m_state = APPEAR;
	m_StateTime = CMain::GetElapsedTimeGameSpeed();

	m_nLife = LIFE; // プレイヤーのライフ

	m_fSpeed = fSpeed;             // ポリゴンの移動速度
	m_fRotSpeed = fRotSpeed;       // ポリゴンの回転速度

	m_nAnimationCount = 0;                   // アニメーションカウント
	m_aniLastTime = CMain::GetElapsedTime(); // アニメーション更新時間

	D3DXVECTOR3 size = GetSize(); // サイズの取得
	size.x /= m_imageBlock.x;     // 画像のブロックを考慮
	size.y /= m_imageBlock.y;     // 画像のブロックを考慮
	SetSize(size);                // テクスチャサイズの設定

	// 頂点情報の設定
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();
	Vertex* pVtx;
	if (FAILED(pVtxBuff->Lock(0, 0, (void**)&pVtx, 0))) { return E_FAIL; }

	D3DXVECTOR3 screenPos = D3DXVECTOR3(transform.pos.x * screenSize.x, transform.pos.y * screenSize.y, 0.0f);

	D3DXVECTOR3 resultPos[VT_DEF] = {};

	size = GetSize(); // サイズの取得
	size.x *= transform.scale.x;
	size.y *= transform.scale.y;
	CMath::Rotation(resultPos, D3DXVECTOR2(size.x, size.y), transform.rot.z);

	for (size_t cntVtx = 0; cntVtx < VT_DEF; cntVtx++)
	{
		resultPos[cntVtx] += screenPos;

		pVtx[cntVtx].pos = resultPos[cntVtx];
		pVtx[cntVtx].rhw = 1.0f;
		pVtx[cntVtx].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[cntVtx].tex = D3DXVECTOR2((float)(cntVtx % 2) * (1.0f / m_imageBlock.x), (float)(cntVtx / 2) * (1.0f / m_imageBlock.y));
	}

	if (FAILED(pVtxBuff->Unlock())) { return E_FAIL; }

	return S_OK;
}

//------------------------------
//終了処理
//------------------------------
void CPlayer::Uninit(void)
{
	CObject2D::Uninit(); // 親の終了
}

//------------------------------
//更新処理
//------------------------------
void CPlayer::Update(void)
{
	CObject2D::Update(); // 親の更新

	CRenderer* pRenderer = CManager::GetRenderer();
	LPDIRECT3DDEVICE9 pDevice = pRenderer->GetDevice(); // レンダラーからデバイスを取得

	// スクリーンサイズの取得
	D3DXVECTOR2 screenSize = {};
	if (FAILED(pRenderer->GetDxScreenSize(&screenSize)))
	{
		return;
	}

	if (m_state != DEATH)
	{// 死んでなければ
		//移動関係
		CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();
		Transform transform = GetTransform();
		float deltaTime = CMain::GetDeltaTimeGameSpeed();

		D3DXVECTOR3 move = { 0.0f, 0.0f, 0.0f };
		float fAngle = GetRotation().z;

		if (pKeyboard->GetPress(DIK_A))//Aキー
		{
			if (pKeyboard->GetPress(DIK_W))//Wキー
			{//左上
				move.x += sinf(D3DX_PI * -0.75f) * m_fSpeed * deltaTime;
				move.y += cosf(D3DX_PI * -0.75f) * m_fSpeed * deltaTime;
				fAngle = D3DX_PI * -0.25f;
			}
			else if (pKeyboard->GetPress(DIK_S))//Sキー
			{//左下
				move.x += sinf(D3DX_PI * -0.25f) * m_fSpeed * deltaTime;
				move.y += cosf(D3DX_PI * -0.25f) * m_fSpeed * deltaTime;
				fAngle = D3DX_PI * -0.75f;
			}
			else
			{//左
				move.x += sinf(D3DX_PI * -0.5f) * m_fSpeed * deltaTime;
				move.y += cosf(D3DX_PI * -0.5f) * m_fSpeed * deltaTime;
				fAngle = D3DX_PI * -0.5f;
			}
		}
		else if (pKeyboard->GetPress(DIK_D))//Dキー
		{
			if (pKeyboard->GetPress(DIK_W) == true)//Wキー
			{//右上
				move.x += sinf(D3DX_PI * 0.75f) * m_fSpeed * deltaTime;
				move.y += cosf(D3DX_PI * 0.75f) * m_fSpeed * deltaTime;
				fAngle = D3DX_PI * 0.25f;
			}
			else if (pKeyboard->GetPress(DIK_S) == true)//Sキー
			{//右下
				move.x += sinf(D3DX_PI * 0.25f) * m_fSpeed * deltaTime;
				move.y += cosf(D3DX_PI * 0.25f) * m_fSpeed * deltaTime;
				fAngle = D3DX_PI * 0.75f;
			}
			else
			{//右
				move.x += sinf(D3DX_PI * 0.5f) * m_fSpeed * deltaTime;
				move.y += cosf(D3DX_PI * 0.5f) * m_fSpeed * deltaTime;
				fAngle = D3DX_PI * 0.5f;
			}
		}
		else if (pKeyboard->GetPress(DIK_W) == true)//Wキー
		{//上
			move.x += sinf(D3DX_PI) * m_fSpeed * deltaTime;
			move.y += cosf(D3DX_PI) * m_fSpeed * deltaTime;
			fAngle = 0.0f;
		}
		else if (pKeyboard->GetPress(DIK_S) == true)//Sキー
		{//下
			move.x += sinf(0.0f) * m_fSpeed * deltaTime;
			move.y += cosf(0.0f) * m_fSpeed * deltaTime;
			fAngle = D3DX_PI;
		}

		AddPosition(move);
		SetRotation(D3DXVECTOR3(0.0f, 0.0f, fAngle));

		if (pKeyboard->GetTrigger(DIK_SPACE))
		{
			size_t randomNum = CMath::Random(4, 8);
			float angleSet = 2.0f * D3DX_PI / (float)randomNum; // 弾の角度間隔
			for (size_t cntBullet = 0; cntBullet < randomNum; cntBullet++)
			{
				float angle = transform.rot.z - cntBullet * angleSet;
				CBullet::Create(transform.pos, D3DXVECTOR3(0.0f, 0.0f, atan2f(-cosf(angle), -sinf(angle))), transform.scale * 10.0f, CMath::Random(0.05f, 0.5f), CBullet::PLAYER, 2);
			}
		}
	}

	// 画面ループ
	D3DXVECTOR3 pos = GetPosition();
	pos.x = pos.x < 0.0f ? 1.0f : pos.x;
	pos.x = pos.x > 1.0f ? 0.0f : pos.x;
	pos.y = pos.y < 0.0f ? 1.0f : pos.y;
	pos.y = pos.y > 1.0f ? 0.0f : pos.y;
	SetPosition(pos);
	
	Transform transform = GetTransform();

	if ((CMain::GetElapsedTime() - m_aniLastTime) >= m_AnimationTime)
	{// アニメーションの更新
		m_nAnimationCount++;
		m_nAnimationCount %= (int)(m_imageBlock.x * m_imageBlock.y);

		m_aniLastTime = CMain::GetElapsedTime();
	}

	// 頂点情報の設定
	LPDIRECT3DVERTEXBUFFER9 pVtxBuff = GetVtxBuff();
	Vertex* pVtx;
	if (FAILED(pVtxBuff->Lock(0, 0, (void**)&pVtx, 0))) { return; }

	// 回転関係処理
	D3DXVECTOR3 screenPos = D3DXVECTOR3(transform.pos.x * screenSize.x, transform.pos.y * screenSize.y, 0.0f);
	D3DXVECTOR3 resultPos[VT_DEF] = {};

	D3DXVECTOR3 size = GetSize(); // サイズの取得
	size.x *= transform.scale.x;
	size.y *= transform.scale.y;
	CMath::Rotation(resultPos, D3DXVECTOR2(size.x, size.y), transform.rot.z);

	// 頂点情報の設定
	for (size_t cntVtx = 0; cntVtx < VT_DEF; cntVtx++)
	{
		resultPos[cntVtx] += screenPos;

		pVtx[cntVtx].pos = resultPos[cntVtx];
		pVtx[cntVtx].rhw = 1.0f;
		pVtx[cntVtx].col = m_state == APPEAR ? TRANSLUCENT_WHITE : m_state == DAMAGE ? RED : WHITE; // 出現は半透明 ダメージ状態は赤
		pVtx[cntVtx].tex = D3DXVECTOR2((float)(cntVtx % 2) * (1.0f / m_imageBlock.x) + (float)(m_nAnimationCount % (int)(m_imageBlock.x)) * (1.0f / m_imageBlock.x), (float)(cntVtx / 2) * (1.0f / m_imageBlock.y) + (float)(m_nAnimationCount / (int)(m_imageBlock.x)) * (1.0f / m_imageBlock.y));
	}

	if (FAILED(pVtxBuff->Unlock())) { return; }

	// 状態管理
	switch (m_state)
	{
	case NONE:
		break;
	case APPEAR:
		if (CMain::GetElapsedTimeGameSpeed() - m_StateTime >= APPEAR_TIME)
		{// 一定時間経過
			m_state = NORMAL; // 通常状態
			m_StateTime = CMain::GetElapsedTimeGameSpeed(); // 時間記録
		}
		break;
	case NORMAL:
		break;
	case DAMAGE:
		if (CMain::GetElapsedTimeGameSpeed() - m_StateTime >= DAMAGE_TIME)
		{// 一定時間経過
			m_state = NORMAL; // 通常状態
			m_StateTime = CMain::GetElapsedTimeGameSpeed(); // 時間記録
		}
		break;
	case DEATH:
		if (CMain::GetElapsedTimeGameSpeed() - m_StateTime >= DEATH_TIME)
		{// 一定時間経過
			Release();
			return;
		}
		break;
	}

	CDebugProc::Print("Player%d:%d\n", GetID(), m_state);
	CDebugProc::Print("Player%d 状態カウンター:%f\n", GetID(), m_StateTime);
}

//------------------------------
//描画処理
//------------------------------
void CPlayer::Draw(void)
{
	CObject2D::Draw(); // 親の描画
}

//------------------------------
// 衝突処理
//------------------------------
void CPlayer::OnCollision(CObject* pOther)
{
	if (m_state == NORMAL)
	{// 無敵時間でない
		if (pOther->GetType() == ENEMY)
		{// 敵であれば
			Hit(10); // ヒット処理
		}

		if (pOther->GetType() == BULLET)
		{// 弾との衝突
			if (dynamic_cast<CBullet*>(pOther)->GetType() == CBullet::ENEMY)
			{// 弾のタイプがプレイヤー
				Hit(10); // ヒット処理
			}
		}
	}
}

//--------------------------
// ダメージ処理
//--------------------------
void CPlayer::Hit(int damage)
{
	m_nLife -= damage; // ダメージ
	if (m_nLife <= 0)
	{// 体力がなくなった
		m_state = DEATH; // 死

		Transform transform = GetTransform();
		CExplosion::Create(transform.pos, transform.rot, transform.scale * 100.0f, CMath::Random(CExplosion::DEFAULT, CExplosion::EXTRA),4); // 爆発エフェクトを生成
	}
	else
	{
		m_state = DAMAGE; // ダメージ
	}
	m_StateTime = CMain::GetElapsedTimeGameSpeed(); // 時間
}