//------------------------------------------
//
// デバッグ処理 [debug.cpp]
// Author: fuma sato
//
//------------------------------------------
#include "debug.h"
#include "manager.h"

//------------------------
// 
// デバッグクラス
// 
//------------------------

// 静的メンバ変数
string CDebugProc::m_sDebug{};
D3DXCOLOR CDebugProc::m_color{ D3DXCOLOR(1.0f,1.0f,1.0f,1.0f) };
UINT CDebugProc::m_flag{ DT_LEFT | DT_TOP | DT_WORDBREAK };
bool CDebugProc::m_bDraw{ true };

//---------------------
// 初期化
//---------------------
HRESULT CDebugProc::Init(void)
{
	// フォントの生成
	return D3DXCreateFont
	(
		CManager::GetRenderer()->GetDevice(),
		24, 0,                        // 高さ24 幅は高さに合わせる
		FW_BOLD,                      // 太字
		1,                            // ミップマップ数 1でいい
		FALSE,                        // 斜体文字
		SHIFTJIS_CHARSET,             // 日本語環境
		OUT_TT_PRECIS,                // TrueTypeフォント優先
		CLEARTYPE_QUALITY,            // アンチエイリアス有効
		DEFAULT_PITCH | FF_DONTCARE,  // 通常
		"Yu Gothic",                  // 書体
		&m_pFont
	);
}

//---------------------
// 終了
//---------------------
void CDebugProc::Uninit(void)
{
	// フォント破棄
	if (m_pFont != nullptr)
	{
		m_pFont->Release();
		m_pFont = nullptr;
	}
}

//---------------------
// 描画
//---------------------
void CDebugProc::Draw(void)
{
	if (m_bDraw)
	{// 表示状態
		D3DXVECTOR2 screenSize{};
		CManager::GetRenderer()->GetDxScreenSize(&screenSize);
		RECT rect{ 0,0,LONG(screenSize.x),LONG(screenSize.y) };

		m_pFont->DrawText(nullptr, m_sDebug.c_str(), -1, &rect, m_flag, D3DCOLOR_COLORVALUE(m_color.r, m_color.g, m_color.b, m_color.a));
	}
	m_sDebug.clear(); // 文字列をクリア
}

////---------------------
//// 文字列追加
////---------------------
//void CDebugProc::Print(const char* format, ...)
//{
//	va_list args{}; // 可変引数のアドレスを見るためのcharポインタ
//	va_start(args, format); // ポインタの位置を可変引数の始まりへ
//
//	while (*format != '\0')
//	{// 文字列が終わるまでループ
//		if (*format == '%')
//		{// %があった
//			format++; // その次に行く
//			switch (*format)
//			{// 型の判別
//			case 'd': // int
//				m_sDebug += std::to_string(va_arg(args, int)); // argsの現時点の位置の引数をintとしてとる
//				break;
//			case 'f': // double
//				m_sDebug += std::to_string(va_arg(args, double)); // argsの現時点の位置の引数をdoubleとしてとる
//				break;
//			case 's': // string
//				m_sDebug += va_arg(args, char*); // argsの現時点の位置の引数を文字列としてとる
//				break;
//			}
//		}
//		else
//		{// 普通の文字
//			m_sDebug += *format; // 追加
//		}
//		format++; // 次の文字へ
//	}
//
//	va_end(args); // ポインタ解放
//}

//---------------------
// 文字列追加
//---------------------
void CDebugProc::Print(const char* format, ...)
{
	char buffer[1024]{};    // 一次格納
	va_list args{};         // 可変引数のアドレスを見るためのcharポインタ
	va_start(args, format); // ポインタの位置を可変引数の始まりへ

	vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args); // 可変引数を適応する関数

	va_end(args); // ポインタ解放

	m_sDebug += buffer; // 追加する
}