#pragma once
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "00.Global/01.Utility/Enumerations.h"

class CCollisionManager;

class CFlyingObject : public CCollisionObject
{
public:
	CFlyingObject(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes = 1);
	virtual ~CFlyingObject();

public:	// 외부 함수
	virtual void Animate(float timeElapsed);

	virtual void SetCollisionManager(CCollisionManager* manager) { m_pColManager = manager; }

	virtual void SetFlyingObjectsType(FlyingObjectType type);
	virtual void SetDirection(const XMFLOAT3& direction);

	virtual FlyingObjectType GetFlyingObjectsType() { return m_flyingObjectType; }

protected: // 내부 함수
	void LookAt(XMFLOAT3 objPosition);
	void LookAt(XMFLOAT2 objPosition);

	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();

	void MoveToDirection(float dist);

protected: // 변수
	FlyingObjectType m_flyingObjectType;
	float	m_damage{ 0.0f };
	float	m_speed{ 0.0f };
	float	m_distance{ 0.0f };
	float	m_maxDistance{ 0.0f };
	XMFLOAT3 m_direction{};

	CCollisionManager * m_pColManager{ NULL };
};

