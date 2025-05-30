//------------------------------------------
//
// オブジェクトの処理の定義・宣言[object.h]
// Author: fuma sato
//
//------------------------------------------
#pragma once
#include "main.h"
#include "renderer.h"

//---------------------
// オブジェクトクラス
//---------------------
class CObject
{
// 公開
public:
	// タイプ
	using TYPE = enum
	{
		NONE = 0,   // 無し
		BACKGROUND, // 背景
		PLAYER,     // プレイヤー
		ENEMY,      // 敵
		BULLET,     // 弾
		EXPLOSION,  // 爆発
		EFFECT,     // エフェクト
		TYPE_MAX    // 最大数
	};

	// トランスフォーム
	using Transform = struct
	{
		D3DXVECTOR3 pos;   // 位置
		D3DXVECTOR3 rot;   // 回転
		D3DXVECTOR3 scale; // スケール
	};

	CObject(int priority = 3); // デフォルト引数によりデフォルトコンストラクタを兼用している
	virtual ~CObject() = default;

	static void ReleaseAll(void);
	static void UpdateAll(void);
	static void DrawAll(void);

	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;

	int GetID(void) const { return m_nID; }
	TYPE GetType(void) const { return m_type; }

	Transform GetTransform(void) const { return m_transform; }
	D3DXVECTOR3 GetPosition(void) const { return m_transform.pos; }
	D3DXVECTOR3 GetRotation(void) const { return m_transform.rot; }
	D3DXVECTOR3 GetScale(void) const { return m_transform.scale; }

	D3DXVECTOR3 GetSize(void) const { return m_size; }

	void SetTransform(const Transform transform) { m_transform = transform; }
	void SetPosition(const D3DXVECTOR3 pos) { m_transform.pos = pos; }
	void SetRotation(const D3DXVECTOR3 rot) { m_transform.rot = rot; }
	void SetScale(const D3DXVECTOR3 scale) { m_transform.scale = scale; }

	void SetSize(const D3DXVECTOR3 size) { m_size = size; }

	void AddTransform(const Transform transform) { m_transform.pos += transform.pos; m_transform.rot += transform.rot; m_transform.scale += transform.scale; }
	void AddPosition(const D3DXVECTOR3 pos) { m_transform.pos += pos; }
	void AddRotation(const D3DXVECTOR3 rot) { m_transform.rot += rot; }
	void AddScale(const D3DXVECTOR3 scale) { m_transform.scale += scale; }

	void IsCollision(const bool bCollision) { m_bCollision = bCollision; }
	bool IsCollision(void) const { return m_bCollision; }

// 家族公開
protected:
	virtual void OnCollision(CObject* pOther) = 0;

	void Init(TYPE type);
	void Release(void);

// 非公開
private:
	static void CallCollision(void);
	static bool CallCollisionHelper(CObject* HostObject, CObject* GuestObject);

	static constexpr int MAX__PRIORITY = 8;                  // オブジェクトの最大数
	static constexpr int MAX_OBJECT = 5096;                  // オブジェクトの最大数
	static CObject* m_apObject[MAX__PRIORITY][MAX_OBJECT];   // オブジェクトのポインタ配列
	static int m_nAll;                                       // オブジェクトの数

	int m_Priority;          // 描画順位
	int m_nID;               // オブジェクトのID
	TYPE m_type;             // オブジェクトの種類
	Transform m_transform;   // 位置・回転・スケール
	D3DXVECTOR3 m_size;      // サイズ（当たり判定用）
	bool m_bCollision;       // あたり判定の有無
};