#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"

class CAnimatedObject : public CBaseObject 
{

protected:
	CSkeleton	m_pSkeleton[20];
	int m_nAniLength[20];
	int m_nAniCnt{ 0 };
	int m_nAniState{ 0 };

	float m_fFrameTime{ 0 };

public:
	CAnimatedObject(CCreateMgr *pCreateMgr, int nMeshes = 1);
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

};

