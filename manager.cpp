//------------------------------------------
//
// マネージャー [manager.cpp]
// Author: fuma sato
//
//------------------------------------------
#include "manager.h"
#include "renderer.h"
#include "input.h"
#include "object.h"
#include "object2D.h"
#include "player.h"
#include "bg.h"
#include "bullet.h"
#include "explosion.h"
#include "math.h"
#include "enemy.h"
#include "effect.h"
#include "debug.h"

//------------------------------
//
// マネージャークラス
//
//------------------------------

// 静的メンバ変数の定義
CRenderer* CManager::m_pRenderer{};
CDebugProc* CManager::m_pDebugProc{};         // デバック表示のポインタ
CInputKeyboard* CManager::m_pInputKeyboard{};
CBg* CManager::m_pBg{};
CPlayer* CManager::m_pPlayer{};

// ポーズ状態かどうか
bool CManager::m_bPause{};

//---------------
// 初期化
//---------------
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd, BOOL bWindow)
{
	// 描画クラス
	if (m_pRenderer == NULL)
	{
		m_pRenderer = new CRenderer;   // 描画クラスのインスタンス生成
	}

	if (m_pRenderer != NULL)
	{
		if (FAILED(m_pRenderer->Init(hInstance, hWnd, bWindow)))//初期化処理
		{
			return E_FAIL;
		}
	}

	// デバック表示クラス
	if (m_pDebugProc == NULL)
	{
		m_pDebugProc = new CDebugProc;   // 描画クラスのインスタンス生成
	}

	if (m_pDebugProc != NULL)
	{
		if (FAILED(m_pDebugProc->Init()))//初期化処理
		{
			return E_FAIL;
		}
	}

	if (m_pInputKeyboard == NULL)
	{
		m_pInputKeyboard = new CInputKeyboard;   // キーボードのインスタンス生成
	}

	if (m_pInputKeyboard != NULL)
	{
		if (FAILED(m_pInputKeyboard->Init(hInstance, hWnd)))//初期化処理
		{
			return E_FAIL;
		}
	}

	if (FAILED(GameLoad()))return E_FAIL;  // ゲームのロード
	if (FAILED(GameStart()))return E_FAIL; // ゲームのスタート

	return S_OK;
}

//---------------
// 終了
//---------------
void CManager::Uninit(void)
{
	GameEnd();    // ゲームの終了
	GameUnload(); // ソースの破棄

	// キーボード破棄
	if (m_pInputKeyboard != NULL)
	{
		m_pInputKeyboard->Uninit(); //終了処理
		delete m_pInputKeyboard;    // インスタンス破棄
		m_pInputKeyboard = NULL;    // NULL
	}

	// デバック表示破棄
	if (m_pDebugProc != NULL)
	{
		m_pDebugProc->Uninit(); //終了処理
		delete m_pDebugProc;    // インスタンス破棄
		m_pDebugProc = NULL;    // NULL
	}

	// レンダラー破棄
	if (m_pRenderer != NULL)
	{
		m_pRenderer->Uninit(); //終了処理
		delete m_pRenderer;    // インスタンス破棄
		m_pRenderer = NULL;    // NULL
	}
}

//---------------
// 更新
//---------------
void CManager::Update(void)
{
	// キーボードの更新
	if (m_pInputKeyboard != NULL)
	{
		m_pInputKeyboard->Update(); // 更新処理
	}

	// レンダラーの更新
	if (m_pRenderer != NULL)
	{
		m_pRenderer->Update();
	}

	// プレイヤーリスポーン
	if (m_pPlayer->GetState() == CPlayer::DEATH)
	{// プレイヤーが死んだら
		m_pPlayer = nullptr; // そのプレイヤーの管理を終わる

		// 新しいプレイヤーの生成
		float scale = CMath::Random(0.005f, 0.05f);
		m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.5f, 0.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, RandomAngle), D3DXVECTOR3(scale, scale, 1.0f), CMath::Random(0.5f, 1.0f), CMath::Random(-0.01f, 0.1f));
		if (m_pPlayer == NULL)
		{
			return;
		}
	}

	// ゲームリセット
	if (CEnemy::GetNumAll() <= 0)
	{// エネミーがいなくなったら
		GameEnd();   // 終わらせる
		GameStart(); // 改めてスタート処理
	}

	CDebugProc::Print("ポーズ:[P]\n");
	if (m_pInputKeyboard->GetTrigger(DIK_P))
	{// ポーズ切替
		m_bPause = !m_bPause;
	}

	CDebugProc::Print("デバック表示切替:[F2]\n");
	if (m_pInputKeyboard->GetTrigger(DIK_F2))
	{// デバック表示切替
		CDebugProc::ToggleDebugDraw();
	}
}

//---------------
// 描画
//---------------
void CManager::Draw(void)
{
	// レンダラーの描画
	if (m_pRenderer != NULL)
	{
		m_pRenderer->Draw();
	}
}

//---------------------------------
// ゲームのロード処理
//---------------------------------
HRESULT CManager::GameLoad(void)
{
	//--------------------
	// ソース読み込み
    //--------------------
	const string sTextureList[] =
	{
		"data\\TEXTURE\\player.png",
		"data\\TEXTURE\\enemy000.png",
		"data\\TEXTURE\\enemy001.png",
		"data\\TEXTURE\\bullet000.png",
		"data\\TEXTURE\\C.png",
		"data\\TEXTURE\\explosion000.png",
		"data\\TEXTURE\\explosion001.png",
		"data\\TEXTURE\\shadow000.jpg"
	};
	if (FAILED(CBg::Load())) return E_FAIL;                        // 背景
	if (FAILED(CPlayer::Load(sTextureList[0]))) return E_FAIL;     // プレイヤー
	if (FAILED(CEnemy::Load(&sTextureList[1]))) return E_FAIL;     // 敵
	if (FAILED(CBullet::Load(&sTextureList[3]))) return E_FAIL;    // バレット
	if (FAILED(CExplosion::Load(&sTextureList[5]))) return E_FAIL; // 爆発
	if (FAILED(CEffect::Load(sTextureList[7]))) return E_FAIL;     // Effect

	return S_OK;
}

//---------------------------------
// ゲームのスタート処理
//---------------------------------
HRESULT CManager::GameStart(void)
{
	//--------------------
	// インスタンス生成
	//--------------------

    // 背景
	m_pBg = CBg::Create(0);
	if (m_pBg == NULL)
	{
		return E_POINTER;
	}

	// Enemy
	for (size_t cntEnemy = 0; cntEnemy < CMath::Random(size_t(2), size_t(5)); cntEnemy++)
	{
		float scale = CMath::Random(0.05f, 1.0f);
		CEnemy* pEnemy = CEnemy::Create(D3DXVECTOR3(CMath::Random(0.1f, 0.9f), CMath::Random(0.1f, 0.9f), 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f), D3DXVECTOR3(scale, scale, 1.0f), CMath::Random(0.1f, 0.2f), CMath::Random(CEnemy::DEFAULT, CEnemy::EXTRA));
		if (pEnemy == NULL)
		{
			return E_POINTER;
		}
	}

	// プレイヤー
	float scale = CMath::Random(0.005f, 0.05f);
	m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.5f, 0.5f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, RandomAngle), D3DXVECTOR3(scale, scale, 1.0f), CMath::Random(0.5f, 1.0f), CMath::Random(-0.01f, 0.1f));
	if (m_pPlayer == NULL)
	{
		return E_POINTER;
	}

	m_bPause = false; // ポーズ状態を初期化

	return S_OK;
}

//---------------------------------
// ゲームのエンド処理
//---------------------------------
void CManager::GameEnd(void)
{
	// オブジェクト破棄
	CObject::ReleaseAll();
}

//---------------------------------
// ゲームのアンロード処理
//---------------------------------
void CManager::GameUnload(void)
{
	CEffect::Unload();    // Effect
	CExplosion::Unload(); // 爆発
	CBullet::Unload();    // バレット
	CEnemy::Unload();     // 敵
	CPlayer::Unload();    // プレイヤー
	CBg::Unload();        // 背景
}

//-------------------
// レンダラーリセット
//-------------------
HRESULT CManager::RenererReset(const BOOL bWindow)
{
	if (m_pRenderer != NULL)
	{
		HRESULT hr = m_pRenderer->ReSize(bWindow);// リサイズ処理

		return hr;
	}
	else
	{
		return E_POINTER;
	}
}