//------------------------------------------
//
// �f�o�b�O���� [debug.cpp]
// Author: fuma sato
//
//------------------------------------------
#include "debug.h"
#include "manager.h"

//------------------------
// 
// �f�o�b�O�N���X
// 
//------------------------

// �ÓI�����o�ϐ�
string CDebugProc::m_sDebug{};
D3DXCOLOR CDebugProc::m_color{ D3DXCOLOR(1.0f,1.0f,1.0f,1.0f) };
UINT CDebugProc::m_flag{ DT_LEFT | DT_TOP | DT_WORDBREAK };
bool CDebugProc::m_bDraw{ true };

//---------------------
// ������
//---------------------
HRESULT CDebugProc::Init(void)
{
	// �t�H���g�̐���
	return D3DXCreateFont
	(
		CManager::GetRenderer()->GetDevice(),
		24, 0,                        // ����24 ���͍����ɍ��킹��
		FW_BOLD,                      // ����
		1,                            // �~�b�v�}�b�v�� 1�ł���
		FALSE,                        // �Α̕���
		SHIFTJIS_CHARSET,             // ���{���
		OUT_TT_PRECIS,                // TrueType�t�H���g�D��
		CLEARTYPE_QUALITY,            // �A���`�G�C���A�X�L��
		DEFAULT_PITCH | FF_DONTCARE,  // �ʏ�
		"Yu Gothic",                  // ����
		&m_pFont
	);
}

//---------------------
// �I��
//---------------------
void CDebugProc::Uninit(void)
{
	// �t�H���g�j��
	if (m_pFont != nullptr)
	{
		m_pFont->Release();
		m_pFont = nullptr;
	}
}

//---------------------
// �`��
//---------------------
void CDebugProc::Draw(void)
{
	if (m_bDraw)
	{// �\�����
		D3DXVECTOR2 screenSize{};
		CManager::GetRenderer()->GetDxScreenSize(&screenSize);
		RECT rect{ 0,0,LONG(screenSize.x),LONG(screenSize.y) };

		m_pFont->DrawText(nullptr, m_sDebug.c_str(), -1, &rect, m_flag, D3DCOLOR_COLORVALUE(m_color.r, m_color.g, m_color.b, m_color.a));
	}
	m_sDebug.clear(); // ��������N���A
}

////---------------------
//// ������ǉ�
////---------------------
//void CDebugProc::Print(const char* format, ...)
//{
//	va_list args{}; // �ψ����̃A�h���X�����邽�߂�char�|�C���^
//	va_start(args, format); // �|�C���^�̈ʒu���ψ����̎n�܂��
//
//	while (*format != '\0')
//	{// �����񂪏I���܂Ń��[�v
//		if (*format == '%')
//		{// %��������
//			format++; // ���̎��ɍs��
//			switch (*format)
//			{// �^�̔���
//			case 'd': // int
//				m_sDebug += std::to_string(va_arg(args, int)); // args�̌����_�̈ʒu�̈�����int�Ƃ��ĂƂ�
//				break;
//			case 'f': // double
//				m_sDebug += std::to_string(va_arg(args, double)); // args�̌����_�̈ʒu�̈�����double�Ƃ��ĂƂ�
//				break;
//			case 's': // string
//				m_sDebug += va_arg(args, char*); // args�̌����_�̈ʒu�̈����𕶎���Ƃ��ĂƂ�
//				break;
//			}
//		}
//		else
//		{// ���ʂ̕���
//			m_sDebug += *format; // �ǉ�
//		}
//		format++; // ���̕�����
//	}
//
//	va_end(args); // �|�C���^���
//}

//---------------------
// ������ǉ�
//---------------------
void CDebugProc::Print(const char* format, ...)
{
	char buffer[1024]{};    // �ꎟ�i�[
	va_list args{};         // �ψ����̃A�h���X�����邽�߂�char�|�C���^
	va_start(args, format); // �|�C���^�̈ʒu���ψ����̎n�܂��

	vsnprintf_s(buffer, sizeof(buffer), _TRUNCATE, format, args); // �ψ�����K������֐�

	va_end(args); // �|�C���^���

	m_sDebug += buffer; // �ǉ�����
}