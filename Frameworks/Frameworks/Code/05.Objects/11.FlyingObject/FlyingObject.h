#pragma once
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "00.Global/01.Utility/Enumerations.h"

class CCollisionManager;

class CFlyingObject : public CCollisionObject
{
public:
	CFlyingObject(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes = 1);
	virtual ~CFlyingObject();

public:	// �ܺ� �Լ�
	virtual void Animate(float timeElapsed);

	virtual void SetCollisionManager(CCollisionManager* manager) { m_pColManager = manager; }

	// Warning! �� �κ��� ���߿� Gettype�̳� SetType���� ������ �� ����
	virtual FlyingObjectType GetFlyingObjectsType() { return m_flyingObjectType; }

protected: // ���� �Լ�
 
protected: // ����
	FlyingObjectType m_flyingObjectType;
	CCollisionManager * m_pColManager{ NULL };
};

