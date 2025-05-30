//------------------------------------------
//
//�`��p�̏����̒�`�E�錾[renderer.h]
//Author: fuma sato
//
//------------------------------------------
#pragma once
#include "main.h"

//----------------------------
// DirectX���C�u����
//----------------------------
#include "d3dx9.h"

// ���C�u�����̃����N
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dxguid.lib")

//----------------------------
// DirectX2D�萔
//----------------------------
 namespace DirectX2D
{
	// ���_
	constexpr DWORD FVF_VERTEX = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1; // ���_�t�H�[�}�b�g
	constexpr int VT_DEF = 4;// ��{���_��

	// �F�̒萔
	constexpr D3DCOLOR WHITE = D3DCOLOR_ARGB(255, 255, 255, 255); // ��
	constexpr D3DCOLOR RED = D3DCOLOR_ARGB(255, 255, 0, 0);       // ��
	constexpr D3DCOLOR GREEN = D3DCOLOR_ARGB(255, 0, 255, 0);     // ��
	constexpr D3DCOLOR BLUE = D3DCOLOR_ARGB(255, 0, 0, 255);      // ��
	constexpr D3DCOLOR BLACK = D3DCOLOR_ARGB(255, 0, 0, 0);       // ��

	// ������ translucent
	constexpr D3DCOLOR TRANSLUCENT_WHITE = D3DCOLOR_ARGB(128, 255, 255, 255); // ��
	constexpr D3DCOLOR TRANSLUCENT_RED = D3DCOLOR_ARGB(128, 255, 0, 0);       // ��
	constexpr D3DCOLOR TRANSLUCENT_GREEN = D3DCOLOR_ARGB(128, 0, 255, 0);     // ��
	constexpr D3DCOLOR TRANSLUCENT_BLUE = D3DCOLOR_ARGB(128, 0, 0, 255);      // ��
	constexpr D3DCOLOR TRANSLUCENT_BLACK = D3DCOLOR_ARGB(128, 0, 0, 0);       // ��

	// ���� transparency
	constexpr D3DCOLOR TRANSPARENCY_WHITE = D3DCOLOR_ARGB(0, 255, 255, 255); // ��
	constexpr D3DCOLOR TRANSPARENCY_RED = D3DCOLOR_ARGB(0, 255, 0, 0);       // ��
	constexpr D3DCOLOR TRANSPARENCY_GREEN = D3DCOLOR_ARGB(0, 0, 255, 0);     // ��
	constexpr D3DCOLOR TRANSPARENCY_BLUE = D3DCOLOR_ARGB(0, 0, 0, 255);      // ��
	constexpr D3DCOLOR TRANSPARENCY_BLACK = D3DCOLOR_ARGB(0, 0, 0, 0);       // ��

	// ���_���̍\���̂̒�`
	using Vertex = struct
	{
		D3DXVECTOR3 pos;    // ���W
		float rhw;          // ���W�ϊ��W��(1.0f)
		D3DCOLOR col;       // �J���[
		D3DXVECTOR2 tex;    // �e�N�X�`�����W
	};
}

 //----------------------------
// DirectX3D�萔
//----------------------------
 namespace DirectX3D
 {
	 // ���_
	 constexpr DWORD FVF_VERTEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1; // ���_�t�H�[�}�b�g
	 constexpr int VT_DEF = 4;// ��{���_��

	 // �F�̒萔
	 constexpr D3DCOLOR WHITE = D3DCOLOR_ARGB(255, 255, 255, 255); // ��
	 constexpr D3DCOLOR RED = D3DCOLOR_ARGB(255, 255, 0, 0);       // ��
	 constexpr D3DCOLOR GREEN = D3DCOLOR_ARGB(255, 0, 255, 0);     // ��
	 constexpr D3DCOLOR BLUE = D3DCOLOR_ARGB(255, 0, 0, 255);      // ��
	 constexpr D3DCOLOR BLACK = D3DCOLOR_ARGB(255, 0, 0, 0);       // ��

	 // ������ translucent
	 constexpr D3DCOLOR TRANSLUCENT_WHITE = D3DCOLOR_ARGB(128, 255, 255, 255); // ��
	 constexpr D3DCOLOR TRANSLUCENT_RED = D3DCOLOR_ARGB(128, 255, 0, 0);       // ��
	 constexpr D3DCOLOR TRANSLUCENT_GREEN = D3DCOLOR_ARGB(128, 0, 255, 0);     // ��
	 constexpr D3DCOLOR TRANSLUCENT_BLUE = D3DCOLOR_ARGB(128, 0, 0, 255);      // ��
	 constexpr D3DCOLOR TRANSLUCENT_BLACK = D3DCOLOR_ARGB(128, 0, 0, 0);       // ��

	 // ���� transparency
	 constexpr D3DCOLOR TRANSPARENCY_WHITE = D3DCOLOR_ARGB(0, 255, 255, 255); // ��
	 constexpr D3DCOLOR TRANSPARENCY_RED = D3DCOLOR_ARGB(0, 255, 0, 0);       // ��
	 constexpr D3DCOLOR TRANSPARENCY_GREEN = D3DCOLOR_ARGB(0, 0, 255, 0);     // ��
	 constexpr D3DCOLOR TRANSPARENCY_BLUE = D3DCOLOR_ARGB(0, 0, 0, 255);      // ��
	 constexpr D3DCOLOR TRANSPARENCY_BLACK = D3DCOLOR_ARGB(0, 0, 0, 0);       // ��

	 // ���_���̍\���̂̒�`
	 using Vertex = struct
	 {
		 D3DXVECTOR3 pos;    // ���W
		 D3DXVECTOR3 nor;    // �@���x�N�g��
		 D3DCOLOR col;       // �J���[
		 D3DXVECTOR2 tex;    // �e�N�X�`�����W
	 };
 }

//------------------
// �����_���[�N���X
//------------------
class CRenderer
{
// ���J
public:
	CRenderer() : m_pD3D{}, m_pD3DDevice{}, m_d3dpp{} {}
	~CRenderer() = default;

	HRESULT Init(HINSTANCE hInstanse, HWND hWnd, const BOOL bWindow = TRUE);
	void Uninit(void);
	void Update(void) const;
	void Draw(void) const;

	LPDIRECT3DDEVICE9 GetDevice(void) const { return m_pD3DDevice; };
	void SetRender(void) const;
	HRESULT GetDxScreenSize(D3DXVECTOR2* size) const;
	HRESULT GetDxScreenToClientRatio(D3DXVECTOR2* ratio) const;
	HRESULT ReSize(const BOOL bWindow);
// ����J
private:
	void ResetRelease(void) const;
	void ResetCreate(void) const;

	LPDIRECT3D9 m_pD3D;              // DirectX�I�u�W�F�N�g
	LPDIRECT3DDEVICE9 m_pD3DDevice;  // DirectX�f�o�C�X

	D3DPRESENT_PARAMETERS m_d3dpp;     // DirectX�p�����[�^
};