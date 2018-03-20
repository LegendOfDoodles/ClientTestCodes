#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"
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

class CAnimatedObject : public CBaseObject 
{
	CSkeleton*	m_pSkeleton;
	CSkeleton*	m_pSkeleton1;
	CSkeleton*	m_pSkeleton2;
	int aniState = 0;
	float m_fFrameTime;
public:
	CAnimatedObject(CCreateMgr *pCreateMgr);
	void SetSkeleton(CSkeleton *skeleton) {
		m_pSkeleton = skeleton;
	}
	void SetSkeleton1(CSkeleton *skeleton) {
		m_pSkeleton1 = skeleton;
	}
	void SetSkeleton2(CSkeleton *skeleton) {
		m_pSkeleton2 = skeleton;
	}
	void AniStateSet() { 
		++aniState;
		m_fFrameTime = 0;
		if (aniState >= 3) { aniState = 0; }
	}
	virtual void Animate(float timeElapsed);
	virtual void Render(CCamera *pCamera, UINT instanceCnt = 1);
	virtual ~CAnimatedObject();

};