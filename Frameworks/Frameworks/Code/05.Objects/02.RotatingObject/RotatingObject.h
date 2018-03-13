#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"
class CRotatingObject : public CBaseObject
{
public: // ������, �Ҹ���
	CRotatingObject(CCreateMgr *pCreateMgr);
	virtual ~CRotatingObject();

public: // ���� �Լ�
	virtual void Animate(float timeElapsed);

	void SetRotationSpeed(float fRotationSpeed) { m_rotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_rotationAxis = xmf3RotationAxis; }

protected: // ���� �Լ�

private:	 // ����
	XMFLOAT3 m_rotationAxis;
	float m_rotationSpeed;
};

class CAnimatedObject : public CBaseObject 
{
	CSkeleton*	m_pSkeleton;
public:
	CAnimatedObject(CCreateMgr *pCreateMgr);
	void SetSkeleton(CSkeleton *skeleton) {
		m_pSkeleton = skeleton;
	}

	virtual void Animate(float timeElapsed);

	virtual ~CAnimatedObject();

};