//------------------------------------------
//
//�`��p�̏���[object.cpp]
//Author: fuma sato
//
//------------------------------------------
#include "object.h"
#include "manager.h"

//---------------------------------------
//
// �I�u�W�F�N�g��{�N���X
//
//---------------------------------------
CObject* CObject::m_apObject[MAX__PRIORITY][MAX_OBJECT] = {};
int CObject::m_nAll = 0;

//------------------------------
// �R���X�g���N�^ (�`�揇)
//------------------------------
CObject::CObject(int priority) :m_Priority(priority), m_nID(-1), m_type{}, m_transform{}, m_bCollision{}
{
	// �I�u�W�F�N�g�z��Ɏ�����o�^����
	for (size_t cntObject = 0; cntObject < MAX_OBJECT; cntObject++)
	{
		if (m_apObject[m_Priority][cntObject] == NULL)
		{
			m_apObject[m_Priority][cntObject] = this;
			m_nID = cntObject;
			m_nAll++;
			break;
		}
	}
}

//------------------------------
// �S�j��
//------------------------------
void CObject::ReleaseAll(void)
{
	for (size_t cntPriority = 0; cntPriority < MAX__PRIORITY; cntPriority++)
	{
		for (size_t cntObject = 0; cntObject < MAX_OBJECT; cntObject++)
		{
			if (m_apObject[cntPriority][cntObject] != NULL)
			{
				m_apObject[cntPriority][cntObject]->Uninit();
				delete m_apObject[cntPriority][cntObject];
				m_apObject[cntPriority][cntObject] = NULL;
			}
		}
	}
}

//------------------------------
// �S�X�V
//------------------------------
void CObject::UpdateAll(void)
{
	CallCollision(); // �Փ˔�����Ăяo��

	for (size_t cntPriority = 0; cntPriority < MAX__PRIORITY; cntPriority++)
	{
		for (size_t cntObject = 0; cntObject < MAX_OBJECT; cntObject++)
		{
			if (m_apObject[cntPriority][cntObject] != NULL)
			{
				m_apObject[cntPriority][cntObject]->Update();
			}
		}
	}
}

//------------------------------
// �S�`��
//------------------------------
void CObject::DrawAll(void)
{
	for (size_t cntPriority = 0; cntPriority < MAX__PRIORITY; cntPriority++)
	{
		for (size_t cntObject = 0; cntObject < MAX_OBJECT; cntObject++)
		{
			if (m_apObject[cntPriority][cntObject] != NULL)
			{
				m_apObject[cntPriority][cntObject]->Draw();
			}
		}
	}
}

//------------------------------
// ������
//------------------------------
void CObject::Init(TYPE type)
{
	m_transform = {}; // �g�����X�t�H�[��������
	m_type = type;    // �I�u�W�F�N�g�̎�ނ�ݒ�
}

//------------------------------
// �j��
//------------------------------
void CObject::Release(void)
{
	if (m_apObject[m_Priority][m_nID] != NULL)
	{
		int Priority = m_Priority;
		int nID = m_nID;
		m_apObject[Priority][nID]->Uninit();
		delete m_apObject[Priority][nID];
		m_apObject[Priority][nID] = NULL;
	}
}

//------------------------------
// OnCollosion�Ăяo��
//------------------------------
void CObject::CallCollision(void)
{
	const size_t ALL_OBJECT = MAX__PRIORITY * MAX_OBJECT;

	for (size_t cntHostObject = 0; cntHostObject < ALL_OBJECT; cntHostObject++)
	{
		// �v���C�I���e�B�ƃC���f�b�N�X�̔ԍ����Z�o
		size_t HostPri = cntHostObject / MAX_OBJECT;
		size_t HostIdx = cntHostObject % MAX_OBJECT;

		if (m_apObject[HostPri][HostIdx] == nullptr || !m_apObject[HostPri][HostIdx]->m_bCollision || m_apObject[HostPri][HostIdx]->m_type == TYPE::NONE)
		{// �I�u�W�F�N�g�����݂��Ȃ��܂��͓����蔻������Ȃ��܂���TYPE��NONE�̏ꍇ�̓X�L�b�v
			continue;
		}

		for (size_t cntGuestObject = cntHostObject + 1; cntGuestObject < ALL_OBJECT; cntGuestObject++)
		{
			// �v���C�I���e�B�ƃC���f�b�N�X�̔ԍ����Z�o
			size_t GuestPri = cntGuestObject / MAX_OBJECT;
			size_t GuestIdx = cntGuestObject % MAX_OBJECT;

			if (m_apObject[GuestPri][GuestIdx] == nullptr || !m_apObject[GuestPri][GuestIdx]->m_bCollision || m_apObject[GuestPri][GuestIdx]->m_type == TYPE::NONE)
			{// �I�u�W�F�N�g�����݂��Ȃ��܂��͓����蔻������Ȃ��܂���TYPE��NONE�̏ꍇ�̓X�L�b�v
				continue;
			}

			if (CallCollisionHelper(m_apObject[HostPri][HostIdx], m_apObject[GuestPri][GuestIdx]))
			{// �Փ˂��Ă���ꍇ
				m_apObject[HostPri][HostIdx]->OnCollision(m_apObject[GuestPri][GuestIdx]); // �z�X�g�I�u�W�F�N�g�̏Փˎ��̏������Ăяo��

				if (m_apObject[HostPri][HostIdx] != nullptr)
				{// ���O��OnCollision�̍폜�G���[���
					m_apObject[GuestPri][GuestIdx]->OnCollision(m_apObject[HostPri][HostIdx]); // �Q�X�g�I�u�W�F�N�g�̏Փˎ��̏������Ăяo��
				}
			}
		}
	}
}

//------------------------------
// �Փ˔���w���p�[�֐�
//------------------------------
bool CObject::CallCollisionHelper(CObject* HostObject, CObject* GuestObject)
{
	//------------------------------------------------------
	// �ЂƂ܂��~�̓����蔻��ōs�� (�K�v�ɉ����ċ�`����)
	//------------------------------------------------------

	// �g�����X�t�H�[�����T�C�Y���擾
	Transform HostTrans = HostObject->GetTransform();
	D3DXVECTOR3 HostSize = HostObject->GetSize();
	HostSize.x *= HostTrans.scale.x;
	HostSize.y *= HostTrans.scale.y;
	Transform GuestTrans = GuestObject->GetTransform();
	D3DXVECTOR3 GuestSize = GuestObject->GetSize();
	GuestSize.x *= GuestTrans.scale.x;
	GuestSize.y *= GuestTrans.scale.y;

	// ���a
	float HostLength = D3DXVec3Length(&HostSize);
	float GuestLength = D3DXVec3Length(&GuestSize);

	// �X�N���[�����W�ɕϊ�
	D3DXVECTOR2 screensize = {};
	CManager::GetRenderer()->GetDxScreenSize(&screensize);
	HostTrans.pos.x *= screensize.x;
	HostTrans.pos.y *= screensize.y;
	GuestTrans.pos.x *= screensize.x;
	GuestTrans.pos.y *= screensize.y;

	D3DXVECTOR3 Space = GuestTrans.pos - HostTrans.pos; // �z�X�g�ƃQ�X�g�̈ʒu�x�N�g�����v�Z
	if (D3DXVec3Length(&Space) < (HostLength + GuestLength) * 0.5f)
	{// �~���ڂ��Ă���
		return true;
	}
	return false;
}