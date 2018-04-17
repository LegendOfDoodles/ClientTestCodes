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
public:
	CAnimatedObject(CCreateMgr *pCreateMgr);
	CAnimatedObject(CCreateMgr *pCreateMgr, int nMeshes);
	virtual ~CAnimatedObject();

public:
	virtual void Animate(float timeElapsed);
	virtual void Render(CCamera *pCamera, UINT instanceCnt = 1);

	void SetSkeleton(CSkeleton *skeleton) {
		m_nAniLength[m_nAniCnt] = skeleton->GetAnimationLength();
		m_pSkeleton[m_nAniCnt++] = *skeleton;
	}

	void AniStateSet() { 
		++m_nAniState;
		m_fFrameTime = 0;
		if (m_nAniState >= m_nAniCnt) { m_nAniState = 0; }
	}

	void MinionTypeSet() {
		++m_nAniState;
		m_fFrameTime = 0;
		if (m_nAniState >= m_nAniCnt) { m_nAniState = 0; }
	}

protected:
	CSkeleton	m_pSkeleton[20];
	int m_nAniLength[20];
	int m_nAniCnt{ 0 };
	int m_nAniState{ 0 };

	float m_fFrameTime{ 0 };
};

