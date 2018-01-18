#pragma once
#include "BaseObject.h"

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