#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"

enum MinionState {
	Walking,
	Attack_1,
	Die
};

class CMinion : public CBaseObject
{
protected:

	MinionState AnimateState = Walking;
	CSkeleton*	m_pSkeleton[20];
	int m_iAniCnt=0;
	int aniState = 0;
	float m_fFrameTime;
public:
	CMinion(CCreateMgr *pCreateMgr);
	
	void SetSkeleton(CSkeleton *skeleton) {
		m_pSkeleton[m_iAniCnt++] = *skeleton;
	}
	void AniStateSet() {
		switch (AnimateState) {
		case MinionState::Walking:
			AnimateState = MinionState::Attack_1;
			break;
		case MinionState::Attack_1:
			AnimateState = MinionState::Die;

			break;
		case MinionState::Die:
			AnimateState = MinionState::Walking;

			break;
		default:
			break;
		}
		m_fFrameTime = 0;
	}
	virtual void Animate(float timeElapsed);
	virtual void Render(CCamera *pCamera, UINT instanceCnt = 1);
	virtual ~CMinion();

};