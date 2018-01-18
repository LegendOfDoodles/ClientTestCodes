#pragma once
#include "BaseObject.h"

class CRotatingObject : public CBaseObject
{
public: // 생성자, 소멸자
	CRotatingObject(CCreateMgr *pCreateMgr);
	virtual ~CRotatingObject();

public: // 공개 함수
	virtual void Animate(float timeElapsed);

	void SetRotationSpeed(float fRotationSpeed) { m_rotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_rotationAxis = xmf3RotationAxis; }

protected: // 내부 함수

private:	 // 변수
	XMFLOAT3 m_rotationAxis;
	float m_rotationSpeed;
};