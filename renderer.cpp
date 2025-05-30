//------------------------------------------
//
//�`��p�̏���[renderer.cpp]
//Author: fuma sato
//
//------------------------------------------
#include "renderer.h"
#include "manager.h"
#include "object.h"
#include "debug.h"

//---------------------------------------
//
// �`���{�N���X
//
//---------------------------------------

//------------------------------
//����������
//------------------------------
HRESULT CRenderer::Init(HINSTANCE hInstanse, HWND hWnd, const BOOL bWindow)
{
	//�I�u�W�F�N�g����
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION); // Direct3D�I�u�W�F�N�g�̐���
	if (m_pD3D==NULL)
	{
		return E_FAIL;
	}

	//�f�B�X�v���C���[�h
	D3DDISPLAYMODE d3ddm = {};//�_�C���N�gX�f�B�X�v���C���[�h�̕ϐ��錾
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// �N���C�A���g�̈�̃T�C�Y���擾
	RECT clientRect = {};
	if (FAILED(CMain::GetClientRect(&clientRect))) return E_FAIL; // �N���C�A���g�T�C�Y�̎擾

	//�o�b�N�o�b�t�@�̐ݒ�
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

	//�f�o�C�X����
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

	SetRender(); // �`��ݒ�

	return S_OK;
}

//------------------------------
//�I������
//------------------------------
void CRenderer::Uninit(void)
{
	if (m_pD3DDevice != NULL)
	{// �f�o�C�X
		m_pD3DDevice->Release();
		m_pD3DDevice = NULL;
	}

	if (m_pD3D != NULL)
	{// �I�u�W�F�N�g
		m_pD3D->Release();
		m_pD3D = NULL;
	}
}

//------------------------------
//�X�V����
//------------------------------
void CRenderer::Update(void) const
{
	if (CManager::IsPause())
	{

	}
	else
	{
		CObject::UpdateAll(); // �I�u�W�F�N�g�̍X�V
	}
}

//------------------------------
//�`�揈��
//------------------------------
void CRenderer::Draw(void) const
{
	m_pD3DDevice->Clear//�t�����g�o�b�t�@�̃N���A
	(
		0,
		NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
		D3DCOLOR_RGBA(0, 0, 0, 0),
		1.0f,
		0
	);

	if (SUCCEEDED(m_pD3DDevice->BeginScene()))//�`��J�n
	{
		CObject::DrawAll(); // �I�u�W�F�N�g�̕`��


		CManager::GetDebugProc()->Draw(); // �f�o�b�N�\��
		m_pD3DDevice->EndScene();//�`��I��
	}
	//�o�b�N�o�b�t�@�ɕ\����؂�ւ���
	m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}

//------------------------------
// �`��ݒ�
//------------------------------
void CRenderer::SetRender(void) const
{
    // �����_�[�X�e�[�g�̐ݒ�
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);			 // �J�����O�̐ݒ�
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);			 // �v���C���[�̒��ɓ����x��������
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);     // �ʏ�̃u�����h
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA); // �ʏ�̃u�����h
	m_pD3DDevice->SetRenderState(D3DRS_ANTIALIASEDLINEENABLE, TRUE);     // ���̃A���`�G�C���A�X (�K�C�h���̈���)
	m_pD3DDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);            // �X�e���V���o�b�t�@�I�t

	// GPU �ɍ��킹���ٕ����t�B���^�ݒ�
	D3DCAPS9 caps;
	m_pD3DDevice->GetDeviceCaps(&caps);
	DWORD maxAniso = min(caps.MaxAnisotropy, static_cast<DWORD>(16));

	// �T���u���[�X�e�[�g�̐ݒ�
	for (size_t cntTex = 0; cntTex < 1; cntTex++)
	{
		// �t�B���^�[�ݒ�
		m_pD3DDevice->SetSamplerState(cntTex, D3DSAMP_MAXANISOTROPY, maxAniso);        // �ٕ����t�B���^
		m_pD3DDevice->SetSamplerState(cntTex, D3DSAMP_MINFILTER, D3DTEXF_ANISOTROPIC); // �ٕ����t�B���^
		m_pD3DDevice->SetSamplerState(cntTex, D3DSAMP_MAGFILTER, D3DTEXF_ANISOTROPIC); // �ٕ����t�B���^

		// �~�b�v�}�b�v�̎g�p�i�����ɉ������œK�ȃe�N�X�`���I���j
		m_pD3DDevice->SetSamplerState(cntTex, D3DSAMP_MAXMIPLEVEL, 0);                  // 0���ł����𑜓x
		float lodBias = -0.75f;
		m_pD3DDevice->SetSamplerState(cntTex, D3DSAMP_MIPMAPLODBIAS, *(DWORD*)&lodBias); // LOD�o�C�A�X����
		m_pD3DDevice->SetSamplerState(cntTex, D3DSAMP_MIPFILTER, D3DTEXF_ANISOTROPIC);   // �~�b�v�}�b�v�⊮

		//�e�N�X�`���J��Ԃ�
		m_pD3DDevice->SetSamplerState(cntTex, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		m_pD3DDevice->SetSamplerState(cntTex, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	}

	// �e�N�X�`���X�e�[�W�̐ݒ�
	for (size_t cntTex = 0; cntTex < 1; cntTex++)
	{
		//�J���[
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);   // �|���Z
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);   // �e�N�X�`���̐F��
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);   // ����
		//�����x
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);   // �|���Z
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);   // �e�N�X�`���̓����x��
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);    // ����
		//���̑�
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_TEXCOORDINDEX, 0);                       //�e�N�X�`���̃C���f�b�N�X
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE); // �e�N�X�`���ϊ�
		m_pD3DDevice->SetTextureStageState(0, D3DTSS_RESULTARG, D3DTA_CURRENT);               // �e�N�X�`���X�e�[�W�̌���
	}

	// �X�e�[�W 1 �ȍ~�𖳌���
	m_pD3DDevice->SetTextureStageState(1, D3DTSS_COLOROP, D3DTOP_DISABLE); // ��{��1��
	m_pD3DDevice->SetTextureStageState(1, D3DTSS_ALPHAOP, D3DTOP_DISABLE); // ��{��1��
}

//----------------------------
// �X�N���[���T�C�Y�̎擾
//----------------------------
HRESULT CRenderer::GetDxScreenSize(D3DXVECTOR2* size) const
{
	// DirectX�̃T�C�Y���擾����
	IDirect3DSurface9* pBackBuffer = NULL;
	if (SUCCEEDED(m_pD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer)))
	{// �o�b�N�o�b�t�@�̎擾
		D3DSURFACE_DESC desc;
		pBackBuffer->GetDesc(&desc);
		size->x = (float)desc.Width;
		size->y = (float)desc.Height;
		pBackBuffer->Release();  // �擾��������
		pBackBuffer = NULL;

		return S_OK;
	}
	else
	{
		return E_FAIL;
	}
}

//---------------------------------------------------
// �X�N���[���T�C�Y�ƃN���C�A���g�T�C�Y�̔䗦���擾
//---------------------------------------------------
HRESULT CRenderer::GetDxScreenToClientRatio(D3DXVECTOR2* ratio) const
{
	// DirectX�̃T�C�Y���擾����
	D3DXVECTOR2 dxSize;
	if (FAILED(GetDxScreenSize(&dxSize))) return E_FAIL;

	// �N���C�A���g�T�C�Y���擾����
	RECT clientRect;
	if (FAILED(CMain::GetClientRect(&clientRect))) return E_FAIL; // �N���C�A���g�T�C�Y�̎擾

	D3DXVECTOR2 clientSize;
	clientSize.x = (float)(clientRect.right - clientRect.left);
	clientSize.y = (float)(clientRect.bottom - clientRect.top);
	ratio->x = dxSize.x / clientSize.x;
	ratio->y = dxSize.y / clientSize.y;

	return S_OK;
}

//----------------------------
// �X�N���[���T�C�Y�ύX
//----------------------------
HRESULT CRenderer::ReSize(const BOOL bWindow)
{
	if (m_pD3DDevice==NULL)
	{
		return E_FAIL;
	}

	Sleep:

	// �f�o�C�X�̏�Ԃ��m�F
	HRESULT hr = m_pD3DDevice->TestCooperativeLevel();

	if (hr == D3DERR_DRIVERINTERNALERROR)
	{// �h���C�o�G���[(�v���W�F�N�g���s�s�\)
		return hr;
	}
	else if (hr == D3DERR_DEVICELOST)
	{// �f�o�C�X���X�g (Reset�҂�)
		Sleep(60); // 60ms�ҋ@
		goto Sleep; // �ēx�m�F
	}
	else if (hr == D3DERR_DEVICENOTRESET || hr == S_OK)
	{// �f�o�C�X���X�g(Reset�\)���͐���
		//�t�����g�o�b�t�@���N���A���Ă���
		m_pD3DDevice->Clear
		(
			0,
			NULL,
			(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER),
			D3DCOLOR_RGBA(0, 0, 0, 0),
			1.0f,
			0
		);

		// �E�B���h�E���[�h�̐ݒ�
		if (m_d3dpp.Windowed != bWindow)
		{
			m_d3dpp.Windowed = bWindow;

			//�f�B�X�v���C���[�h
			D3DDISPLAYMODE d3ddm = {};//�_�C���N�gX�f�B�X�v���C���[�h�̕ϐ��錾
			if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
			{
				return E_FAIL;
			}

			m_d3dpp.FullScreen_RefreshRateInHz = m_d3dpp.Windowed ? 0 : d3ddm.RefreshRate;
		}

		//�o�b�N�o�b�t�@�T�C�Y�̕ύX
		if (m_d3dpp.Windowed)
		{// �E�B���h�E���[�h
			m_d3dpp.BackBufferWidth = (UINT)CMain::SCREEN_WIDTH;
			m_d3dpp.BackBufferHeight = (UINT)CMain::SCREEN_HEIGHT;
		}
		else
		{// �t���X�N���[�����[�h
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

		// ���Z�b�g�O�̔j��
		ResetRelease();

		// �f�o�C�X�̃��Z�b�g
		if (FAILED(m_pD3DDevice->Reset(&m_d3dpp)))
		{
			return E_FAIL;
		}

		// �f�o�C�X�̏�Ԃ��m�F
		if (FAILED(m_pD3DDevice->TestCooperativeLevel()))
		{
			return E_FAIL;
		}

		// �f�o�C�X�̃��Z�b�g��ɕ`��ݒ���ēx�s��
		SetRender();

		// ���Z�b�g��̍Đ���
		ResetCreate();

		return S_OK;
	}
	else
	{// ���̑��̃G���[
		return hr;
	}
}

//----------------------------
// ���Z�b�g�O�̔j��
//----------------------------
void CRenderer::ResetRelease(void) const
{
	CManager::GetDebugProc()->Uninit(); // �f�o�b�N�\��
}

//----------------------------
// ���Z�b�g��̍Đ���
//----------------------------
void CRenderer::ResetCreate(void) const
{
	CManager::GetDebugProc()->Init(); // �f�o�b�N�\��
}