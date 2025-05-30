//------------------------------------------
//
//描画用の処理の定義・宣言[renderer.h]
//Author: fuma sato
//
//------------------------------------------
#pragma once
#include "main.h"

//----------------------------
// DirectXライブラリ
//----------------------------
#include "d3dx9.h"

// ライブラリのリンク
#pragma comment(lib,"d3d9.lib")
#pragma comment(lib,"d3dx9.lib")
#pragma comment(lib,"dxguid.lib")

//----------------------------
// DirectX2D定数
//----------------------------
 namespace DirectX2D
{
	// 頂点
	constexpr DWORD FVF_VERTEX = D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1; // 頂点フォーマット
	constexpr int VT_DEF = 4;// 基本頂点数

	// 色の定数
	constexpr D3DCOLOR WHITE = D3DCOLOR_ARGB(255, 255, 255, 255); // 白
	constexpr D3DCOLOR RED = D3DCOLOR_ARGB(255, 255, 0, 0);       // 赤
	constexpr D3DCOLOR GREEN = D3DCOLOR_ARGB(255, 0, 255, 0);     // 緑
	constexpr D3DCOLOR BLUE = D3DCOLOR_ARGB(255, 0, 0, 255);      // 青
	constexpr D3DCOLOR BLACK = D3DCOLOR_ARGB(255, 0, 0, 0);       // 黒

	// 半透明 translucent
	constexpr D3DCOLOR TRANSLUCENT_WHITE = D3DCOLOR_ARGB(128, 255, 255, 255); // 白
	constexpr D3DCOLOR TRANSLUCENT_RED = D3DCOLOR_ARGB(128, 255, 0, 0);       // 赤
	constexpr D3DCOLOR TRANSLUCENT_GREEN = D3DCOLOR_ARGB(128, 0, 255, 0);     // 緑
	constexpr D3DCOLOR TRANSLUCENT_BLUE = D3DCOLOR_ARGB(128, 0, 0, 255);      // 青
	constexpr D3DCOLOR TRANSLUCENT_BLACK = D3DCOLOR_ARGB(128, 0, 0, 0);       // 黒

	// 透明 transparency
	constexpr D3DCOLOR TRANSPARENCY_WHITE = D3DCOLOR_ARGB(0, 255, 255, 255); // 白
	constexpr D3DCOLOR TRANSPARENCY_RED = D3DCOLOR_ARGB(0, 255, 0, 0);       // 赤
	constexpr D3DCOLOR TRANSPARENCY_GREEN = D3DCOLOR_ARGB(0, 0, 255, 0);     // 緑
	constexpr D3DCOLOR TRANSPARENCY_BLUE = D3DCOLOR_ARGB(0, 0, 0, 255);      // 青
	constexpr D3DCOLOR TRANSPARENCY_BLACK = D3DCOLOR_ARGB(0, 0, 0, 0);       // 黒

	// 頂点情報の構造体の定義
	using Vertex = struct
	{
		D3DXVECTOR3 pos;    // 座標
		float rhw;          // 座標変換係数(1.0f)
		D3DCOLOR col;       // カラー
		D3DXVECTOR2 tex;    // テクスチャ座標
	};
}

 //----------------------------
// DirectX3D定数
//----------------------------
 namespace DirectX3D
 {
	 // 頂点
	 constexpr DWORD FVF_VERTEX = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_DIFFUSE | D3DFVF_TEX1; // 頂点フォーマット
	 constexpr int VT_DEF = 4;// 基本頂点数

	 // 色の定数
	 constexpr D3DCOLOR WHITE = D3DCOLOR_ARGB(255, 255, 255, 255); // 白
	 constexpr D3DCOLOR RED = D3DCOLOR_ARGB(255, 255, 0, 0);       // 赤
	 constexpr D3DCOLOR GREEN = D3DCOLOR_ARGB(255, 0, 255, 0);     // 緑
	 constexpr D3DCOLOR BLUE = D3DCOLOR_ARGB(255, 0, 0, 255);      // 青
	 constexpr D3DCOLOR BLACK = D3DCOLOR_ARGB(255, 0, 0, 0);       // 黒

	 // 半透明 translucent
	 constexpr D3DCOLOR TRANSLUCENT_WHITE = D3DCOLOR_ARGB(128, 255, 255, 255); // 白
	 constexpr D3DCOLOR TRANSLUCENT_RED = D3DCOLOR_ARGB(128, 255, 0, 0);       // 赤
	 constexpr D3DCOLOR TRANSLUCENT_GREEN = D3DCOLOR_ARGB(128, 0, 255, 0);     // 緑
	 constexpr D3DCOLOR TRANSLUCENT_BLUE = D3DCOLOR_ARGB(128, 0, 0, 255);      // 青
	 constexpr D3DCOLOR TRANSLUCENT_BLACK = D3DCOLOR_ARGB(128, 0, 0, 0);       // 黒

	 // 透明 transparency
	 constexpr D3DCOLOR TRANSPARENCY_WHITE = D3DCOLOR_ARGB(0, 255, 255, 255); // 白
	 constexpr D3DCOLOR TRANSPARENCY_RED = D3DCOLOR_ARGB(0, 255, 0, 0);       // 赤
	 constexpr D3DCOLOR TRANSPARENCY_GREEN = D3DCOLOR_ARGB(0, 0, 255, 0);     // 緑
	 constexpr D3DCOLOR TRANSPARENCY_BLUE = D3DCOLOR_ARGB(0, 0, 0, 255);      // 青
	 constexpr D3DCOLOR TRANSPARENCY_BLACK = D3DCOLOR_ARGB(0, 0, 0, 0);       // 黒

	 // 頂点情報の構造体の定義
	 using Vertex = struct
	 {
		 D3DXVECTOR3 pos;    // 座標
		 D3DXVECTOR3 nor;    // 法線ベクトル
		 D3DCOLOR col;       // カラー
		 D3DXVECTOR2 tex;    // テクスチャ座標
	 };
 }

//------------------
// レンダラークラス
//------------------
class CRenderer
{
// 公開
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
// 非公開
private:
	void ResetRelease(void) const;
	void ResetCreate(void) const;

	LPDIRECT3D9 m_pD3D;              // DirectXオブジェクト
	LPDIRECT3DDEVICE9 m_pD3DDevice;  // DirectXデバイス

	D3DPRESENT_PARAMETERS m_d3dpp;     // DirectXパラメータ
};