//------------------------------------------
//
// �I�u�W�F�N�g�̏����̒�`�E�錾[object.h]
// Author: fuma sato
//
//------------------------------------------
#pragma once
#include "main.h"
#include "renderer.h"

//---------------------
// �I�u�W�F�N�g�N���X
//---------------------
class CObject
{
// ���J
public:
	// �^�C�v
	using TYPE = enum
	{
		NONE = 0,   // ����
		BACKGROUND, // �w�i
		PLAYER,     // �v���C���[
		ENEMY,      // �G
		BULLET,     // �e
		EXPLOSION,  // ����
		EFFECT,     // �G�t�F�N�g
		TYPE_MAX    // �ő吔
	};

	// �g�����X�t�H�[��
	using Transform = struct
	{
		D3DXVECTOR3 pos;   // �ʒu
		D3DXVECTOR3 rot;   // ��]
		D3DXVECTOR3 scale; // �X�P�[��
	};

	CObject(int priority = 3); // �f�t�H���g�����ɂ��f�t�H���g�R���X�g���N�^�����p���Ă���
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

// �Ƒ����J
protected:
	virtual void OnCollision(CObject* pOther) = 0;

	void Init(TYPE type);
	void Release(void);

// ����J
private:
	static void CallCollision(void);
	static bool CallCollisionHelper(CObject* HostObject, CObject* GuestObject);

	static constexpr int MAX__PRIORITY = 8;                  // �I�u�W�F�N�g�̍ő吔
	static constexpr int MAX_OBJECT = 5096;                  // �I�u�W�F�N�g�̍ő吔
	static CObject* m_apObject[MAX__PRIORITY][MAX_OBJECT];   // �I�u�W�F�N�g�̃|�C���^�z��
	static int m_nAll;                                       // �I�u�W�F�N�g�̐�

	int m_Priority;          // �`�揇��
	int m_nID;               // �I�u�W�F�N�g��ID
	TYPE m_type;             // �I�u�W�F�N�g�̎��
	Transform m_transform;   // �ʒu�E��]�E�X�P�[��
	D3DXVECTOR3 m_size;      // �T�C�Y�i�����蔻��p�j
	bool m_bCollision;       // �����蔻��̗L��
};