//------------------------------------------
//
//描画用の処理[renderer.cpp]
//Author: fuma sato
//
//------------------------------------------
#include "renderer.h"
#include "manager.h"
#include "object.h"
#include "debug.h"

//---------------------------------------
//
// 描画基本クラス
//
//---------------------------------------

//------------------------------
//初期化処理
//------------------------------
HRESULT CRenderer::Init(HINSTANCE hInstanse, HWND hWnd, const BOOL bWindow)
{
	//オブジェクト生成
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION); // Direct3Dオブジェクトの生成
	if (m_pD3D==NULL)
	{
		return E_FAIL;
	}

	//ディスプレイモード
	D3DDISPLAYMODE d3ddm = {};//ダイレクトXディスプレイモードの変数宣言
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// クライアント領域のサイズを取得
	RECT clientRect = {};
	if (FAILED(CMain::GetClientRect(&clientRect))) return E_FAIL; // クライアントサイズの取得

	//バックバッファの設定
	m_d3dpp.hDeviceWindow = hWnd;
	m_d3dpp.BackBufferWidth = clientRect.right - clientRect.left;
	m_d3dpp.BackBufferHeight = clientRect.bottom - clientRect.top;
	m_d3dpp.BackBufferFormat = d3ddm.Format;
	m_d3dpp.BackBufferCount = 1;
	m_d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_d3dpp.EnableAutoDepthStencil = TRUE;
	m_d3dpp.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_d3dpp.Flags = D3DPRESENTFLAG_DISCARD_DEPTHSTENCIL;
	m_d3dpp.Windowed = bWindow;
	m_d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
	m_d3dpp.FullScreen_RefreshRateInHz = m_d3dpp.Windowed ? 0 : d3ddm.RefreshRate;

	DWORD qualityLevels = 0;
	const D3DMULTISAMPLE_TYPE samples[] =
	{
		D3DMULTISAMPLE_16_SAMPLES,
		D3DMULTISAMPLE_8_SAMPLES,
		D3DMULTISAMPLE_4_SAMPLES,
		D3DMULTISAMPLE_2_SAMPLES,
		D3DMULTISAMPLE_NONE
	};

	for (auto sample : samples)
	{
		if (SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType
		(
			D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
			m_d3dpp.BackBufferFormat, m_d3dpp.Windowed,
			sample, &qualityLevels))
		)
		{
			m_d3dpp.MultiSampleType = sample;
			m_d3dpp.MultiSampleQuality = (sample == D3DMULTISAMPLE_NONE) ? 0 : qualityLevels - 1;
			break;
		}
	}

	//デバイス生成
	if (FAILED(m_pD3D->CreateDevice
	(
		D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&m_d3dpp,
		&m_pD3DDevice
	)))
	{
		if (FAILED(m_pD3D->CreateDevice
		(
			D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&m_d3dpp,
			&m_pD3DDevice
		)))
		{
			if (FAILED(m_pD3D->CreateDevice
			(
				D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&m_d3dpp,
				&m_pD3DDevice
			)))
			{
				return E_FAIL;
			}
		}
	}

	SetRender(); // 描画設定

	return S_OK;
}

//------------------------------
//終了処理
//------------------------------
void CRenderer::Uninit(void)
{
	if (m_pD3DDevice != NULL)
	{// デバイス
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}

	if (m_pD3D != NULL)
	{// オブジェクト
		m_pD3D->Release();
		m_pD3D = NULL;
	}
}

//------------------------------
//更新処理
//------------------------------
void CRenderer::Update(void) const
{
	if (CManager::IsPause())
	{

	}
	else
	{
		CObject::UpdateAll(); // オブジェクトの更新
	}
}

//------------------------------
//描画処理
//------------------------------
void CRenderer::Draw(void) const
{
	m_pD3DDevice->Clear//フロントバッファのクリア
	(
		0,
		NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 0, 0, 0),
		1.0f,
		0
	);

	if (SUCCEEDED(m_pD3DDevice->BeginScene()))//描画開始
	{
		CObject::DrawAll(); // オブジェクトの描画


		CManager::GetDebugProc()->Draw(); // デバック表示
		m_pD3DDevice->EndScene();//描画終了
	}
	//バックバッファに表示を切り替える
	m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

//------------------------------
// 描画設定
//------------------------------
void CRenderer::SetRender(void) const
{
    // レンダーステートの設定
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);			 // カリングの設定
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);			 // プレイヤーの中に透明度を加える
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);     // 通常のブレンド
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA); // 通常のブレンド
	m_pD3DDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);     // 線のアンチエイリアス (ガイド線の安定)
	m_pD3DDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);            // ステンシルバッファオフ

	// GPU に合わせた異方性フィルタ設定
	D3DCAPS9 caps;
	m_pD3DDevice->GetDeviceCaps(&caps);
	DWORD maxAniso = min(caps.MaxAnisotropy, static_cast<DWORD>(16));

	// サンブラーステートの設定
	for (size_t cntTex = 0; cntTex < 1; cntTex++)
	{
		// フィルター設定
		m_pD3DDevice->SetSamplerState(cntTex, D3DSAMP_MAXANISOTROPY, maxAniso);        // 異方性フィルタ
		m_pD3DDevice->SetSamplerState(cntTex, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC); // 異方性フィルタ
		m_pD3DDevice->SetSamplerState(cntTex, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC); // 異方性フィルタ

		// ミップマップの使用（距離に応じた最適なテクスチャ選択）
		m_pD3DDevice->SetSamplerState(cntTex, D3DSAMP_MAXMIPLEVEL, 0);                  // 0が最も高解像度
		float lodBias = -0.75f;
		m_pD3DDevice->SetSamplerState(cntTex, D3DSAMP_MIPMAPLODBIAS, *(DWORD*)&lodBias); // LODバイアス調整
		m_pD3DDevice->SetSamplerState(cntTex, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);   // ミップマップ補完

		//テクスチャ繰り返し
		m_pD3DDevice->SetSamplerState(cntTex, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_pD3DDevice->SetSamplerState(cntTex, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	}

	// テクスチャステージの設定
	for (size_t cntTex = 0; cntTex < 1; cntTex++)
	{
		//カラー
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);   // 掛け算
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);   // テクスチャの色と
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);   // 光を
		//透明度
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);   // 掛け算
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);   // テクスチャの透明度と
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);    // 光を
		//その他
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);                       //テクスチャのインデックス
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE); // テクスチャ変換
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_RESULTARG, D3DTA_CURRENT);               // テクスチャステージの結果
	}

	// ステージ 1 以降を無効化
	m_pD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE); // 基本は1枚
	m_pD3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE); // 基本は1枚
}

//----------------------------
// スクリーンサイズの取得
//----------------------------
HRESULT CRenderer::GetDxScreenSize(D3DXVECTOR2* size) const
{
	// DirectXのサイズを取得する
	IDirect3DSurface9* pBackBuffer = NULL;
	if (SUCCEEDED(m_pD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer)))
	{// バックバッファの取得
		D3DSURFACE_DESC desc;
		pBackBuffer->GetDesc(&desc);
		size->x = (float)desc.Width;
		size->y = (float)desc.Height;
		pBackBuffer->Release();  // 取得したら解放
		pBackBuffer = NULL;

		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

//---------------------------------------------------
// スクリーンサイズとクライアントサイズの比率を取得
//---------------------------------------------------
HRESULT CRenderer::GetDxScreenToClientRatio(D3DXVECTOR2* ratio) const
{
	// DirectXのサイズを取得する
	D3DXVECTOR2 dxSize;
	if (FAILED(GetDxScreenSize(&dxSize))) return E_FAIL;

	// クライアントサイズを取得する
	RECT clientRect;
	if (FAILED(CMain::GetClientRect(&clientRect))) return E_FAIL; // クライアントサイズの取得

	D3DXVECTOR2 clientSize;
	clientSize.x = (float)(clientRect.right - clientRect.left);
	clientSize.y = (float)(clientRect.bottom - clientRect.top);
	ratio->x = dxSize.x / clientSize.x;
	ratio->y = dxSize.y / clientSize.y;

	return S_OK;
}

//----------------------------
// スクリーンサイズ変更
//----------------------------
HRESULT CRenderer::ReSize(const BOOL bWindow)
{
	if (m_pD3DDevice==NULL)
	{
		return E_FAIL;
	}

	Sleep:

	// デバイスの状態を確認
	HRESULT hr = m_pD3DDevice->TestCooperativeLevel();

	if (hr == D3DERR_DRIVERINTERNALERROR)
	{// ドライバエラー(プロジェクト実行不能)
		return hr;
	}
	else if (hr == D3DERR_DEVICELOST)
	{// デバイスロスト (Reset待ち)
		Sleep(60); // 60ms待機
		goto Sleep; // 再度確認
	}
	else if (hr == D3DERR_DEVICENOTRESET || hr == S_OK)
	{// デバイスロスト(Reset可能)又は正常
		//フロントバッファをクリアしておく
		m_pD3DDevice->Clear
		(
			0,
			NULL,
			(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
			D3DCOLOR_RGBA(0, 0, 0, 0),
			1.0f,
			0
		);

		// ウィンドウモードの設定
		if (m_d3dpp.Windowed != bWindow)
		{
			m_d3dpp.Windowed = bWindow;

			//ディスプレイモード
			D3DDISPLAYMODE d3ddm = {};//ダイレクトXディスプレイモードの変数宣言
			if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
			{
				return E_FAIL;
			}

			m_d3dpp.FullScreen_RefreshRateInHz = m_d3dpp.Windowed ? 0 : d3ddm.RefreshRate;
		}

		//バックバッファサイズの変更
		if (m_d3dpp.Windowed)
		{// ウィンドウモード
			m_d3dpp.BackBufferWidth = (UINT)CMain::SCREEN_WIDTH;
			m_d3dpp.BackBufferHeight = (UINT)CMain::SCREEN_HEIGHT;
		}
		else
		{// フルスクリーンモード
			RECT clientRect;
			if (FAILED(CMain::GetClientRect(&clientRect)))return E_FAIL;
			m_d3dpp.BackBufferWidth = clientRect.right - clientRect.left;
			m_d3dpp.BackBufferHeight = clientRect.bottom - clientRect.top;
		}

		DWORD qualityLevels = 0;
		const D3DMULTISAMPLE_TYPE samples[] =
		{
			D3DMULTISAMPLE_16_SAMPLES,
			D3DMULTISAMPLE_8_SAMPLES,
			D3DMULTISAMPLE_4_SAMPLES,
			D3DMULTISAMPLE_2_SAMPLES,
			D3DMULTISAMPLE_NONE
		};

		for (auto sample : samples)
		{
			if (SUCCEEDED(m_pD3D->CheckDeviceMultiSampleType
			(
				D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL,
				m_d3dpp.BackBufferFormat, m_d3dpp.Windowed,
				sample, &qualityLevels))
				)
			{
				m_d3dpp.MultiSampleType = sample;
				m_d3dpp.MultiSampleQuality = (sample == D3DMULTISAMPLE_NONE) ? 0 : qualityLevels - 1;
				break;
			}
		}

		// リセット前の破棄
		ResetRelease();

		// デバイスのリセット
		if (FAILED(m_pD3DDevice->Reset(&m_d3dpp)))
		{
			return E_FAIL;
		}

		// デバイスの状態を確認
		if (FAILED(m_pD3DDevice->TestCooperativeLevel()))
		{
			return E_FAIL;
		}

		// デバイスのリセット後に描画設定を再度行う
		SetRender();

		// リセット後の再生成
		ResetCreate();

		return S_OK;
	}
	else
	{// その他のエラー
		return hr;
	}
}

//----------------------------
// リセット前の破棄
//----------------------------
void CRenderer::ResetRelease(void) const
{
	CManager::GetDebugProc()->Uninit(); // デバック表示
}

//----------------------------
// リセット後の再生成
//----------------------------
void CRenderer::ResetCreate(void) const
{
	CManager::GetDebugProc()->Init(); // デバック表示
}