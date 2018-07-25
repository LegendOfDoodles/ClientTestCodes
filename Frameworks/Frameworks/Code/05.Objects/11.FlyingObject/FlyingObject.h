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

	// Warning! 이 부분은 나중에 Gettype이나 SetType으로 통합할 수 있음
	virtual FlyingObjectType GetFlyingObjectsType() { return m_flyingObjectType; }

protected: // 내부 함수
 
protected: // 변수
	FlyingObjectType m_flyingObjectType;
	CCollisionManager * m_pColManager{ NULL };
};

