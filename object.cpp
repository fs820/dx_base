//------------------------------------------
//
//描画用の処理[object.cpp]
//Author: fuma sato
//
//------------------------------------------
#include "object.h"
#include "manager.h"

//---------------------------------------
//
// オブジェクト基本クラス
//
//---------------------------------------
CObject* CObject::m_apObject[MAX__PRIORITY][MAX_OBJECT] = {};
int CObject::m_nAll = 0;

//------------------------------
// コンストラクタ (描画順)
//------------------------------
CObject::CObject(int priority) :m_Priority(priority), m_nID(-1), m_type{}, m_transform{}, m_bCollision{}
{
	// オブジェクト配列に自分を登録する
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
// 全破棄
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
// 全更新
//------------------------------
void CObject::UpdateAll(void)
{
	CallCollision(); // 衝突判定を呼び出す

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
// 全描画
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
// 初期化
//------------------------------
void CObject::Init(TYPE type)
{
	m_transform = {}; // トランスフォーム初期化
	m_type = type;    // オブジェクトの種類を設定
}

//------------------------------
// 破棄
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
// OnCollosion呼び出し
//------------------------------
void CObject::CallCollision(void)
{
	const size_t ALL_OBJECT = MAX__PRIORITY * MAX_OBJECT;

	for (size_t cntHostObject = 0; cntHostObject < ALL_OBJECT; cntHostObject++)
	{
		// プライオリティとインデックスの番号を算出
		size_t HostPri = cntHostObject / MAX_OBJECT;
		size_t HostIdx = cntHostObject % MAX_OBJECT;

		if (m_apObject[HostPri][HostIdx] == nullptr || !m_apObject[HostPri][HostIdx]->m_bCollision || m_apObject[HostPri][HostIdx]->m_type == TYPE::NONE)
		{// オブジェクトが存在しないまたは当たり判定をしないまたはTYPEがNONEの場合はスキップ
			continue;
		}

		for (size_t cntGuestObject = cntHostObject + 1; cntGuestObject < ALL_OBJECT; cntGuestObject++)
		{
			// プライオリティとインデックスの番号を算出
			size_t GuestPri = cntGuestObject / MAX_OBJECT;
			size_t GuestIdx = cntGuestObject % MAX_OBJECT;

			if (m_apObject[GuestPri][GuestIdx] == nullptr || !m_apObject[GuestPri][GuestIdx]->m_bCollision || m_apObject[GuestPri][GuestIdx]->m_type == TYPE::NONE)
			{// オブジェクトが存在しないまたは当たり判定をしないまたはTYPEがNONEの場合はスキップ
				continue;
			}

			if (CallCollisionHelper(m_apObject[HostPri][HostIdx], m_apObject[GuestPri][GuestIdx]))
			{// 衝突している場合
				m_apObject[HostPri][HostIdx]->OnCollision(m_apObject[GuestPri][GuestIdx]); // ホストオブジェクトの衝突時の処理を呼び出す

				if (m_apObject[HostPri][HostIdx] != nullptr)
				{// 直前のOnCollisionの削除エラー回避
					m_apObject[GuestPri][GuestIdx]->OnCollision(m_apObject[HostPri][HostIdx]); // ゲストオブジェクトの衝突時の処理を呼び出す
				}
			}
		}
	}
}

//------------------------------
// 衝突判定ヘルパー関数
//------------------------------
bool CObject::CallCollisionHelper(CObject* HostObject, CObject* GuestObject)
{
	//------------------------------------------------------
	// ひとまず円の当たり判定で行う (必要に応じて矩形判定)
	//------------------------------------------------------

	// トランスフォームをサイズを取得
	Transform HostTrans = HostObject->GetTransform();
	D3DXVECTOR3 HostSize = HostObject->GetSize();
	HostSize.x *= HostTrans.scale.x;
	HostSize.y *= HostTrans.scale.y;
	Transform GuestTrans = GuestObject->GetTransform();
	D3DXVECTOR3 GuestSize = GuestObject->GetSize();
	GuestSize.x *= GuestTrans.scale.x;
	GuestSize.y *= GuestTrans.scale.y;

	// 直径
	float HostLength = D3DXVec3Length(&HostSize);
	float GuestLength = D3DXVec3Length(&GuestSize);

	// スクリーン座標に変換
	D3DXVECTOR2 screensize = {};
	CManager::GetRenderer()->GetDxScreenSize(&screensize);
	HostTrans.pos.x *= screensize.x;
	HostTrans.pos.y *= screensize.y;
	GuestTrans.pos.x *= screensize.x;
	GuestTrans.pos.y *= screensize.y;

	D3DXVECTOR3 Space = GuestTrans.pos - HostTrans.pos; // ホストとゲストの位置ベクトルを計算
	if (D3DXVec3Length(&Space) < (HostLength + GuestLength) * 0.5f)
	{// 円が接している
		return true;
	}
	return false;
}