#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"

struct PlayerInfo
{
	float maxHP;
	float HP;

	float maxMP;
	float MP;

	float Atk;
	float Def;

	UINT Level;

	UINT MaxExp;
	UINT Exp;
};

struct CommonInfo
{
	float maxHP;
	float HP;

	float Atk;
	float Def;

	UINT Exp;
};

struct StaticInfo 
{
	float maxHP;
	float HP;

	float Atk;
	float Def;

	UINT Exp;
};

class CCollisionObject : public CBaseObject
{
public:	// 생성자, 소멸자
	CCollisionObject(CCreateMgr *pCreateMgr, int nMeshes = 1);
	virtual ~CCollisionObject();

public: // 공개 함수
	virtual void SetCollisionSize(float size) { m_fCollisionSize = size; }
	virtual float GetCollisionSize() { return m_fCollisionSize; }
	virtual XMFLOAT2 GetCollisionLevel() { return m_xmf2CollisionLevel; }

protected: // 내부 함수
	void ResetCollisionLevel() {
		m_xmf2CollisionLevel.x = floor(GetPosition().x/ (TERRAIN_SIZE_WIDTH / 100));
		m_xmf2CollisionLevel.y = floor(GetPosition().z/ (TERRAIN_SIZE_WIDTH / 100));
	}
protected: // 변수
	float m_fCollisionSize{ 1 };
	XMFLOAT2 m_xmf2CollisionLevel{ 0,0 };
};
