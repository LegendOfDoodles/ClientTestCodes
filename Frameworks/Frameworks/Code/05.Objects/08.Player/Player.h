#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"
#include "00.Global/01.Utility/Enumerations.h"

class CCollisionManager;

class CPlayer : public CAnimatedObject
{
public:
	CPlayer(CCreateMgr *pCreateMgr, int nMeshes = 1);
	virtual ~CPlayer();

public:	// 외부 함수
	virtual void Animate(float timeElapsed);
	virtual void Render(CCamera *pCamera, UINT instanceCnt = 1);

	virtual void ActiveSkill(AnimationsType act);

	virtual 	void SetState(StatesType newState);

	virtual void SetCollisionManager(CCollisionManager* manager) { m_pColManager = manager; }

	virtual void ReceiveDamage(float damage) { m_StatusInfo.HP -= damage * Compute_Defence(m_StatusInfo.Def); }

protected: // 내부 함수
	virtual void AdjustAnimationIndex();
	/*
	0. Idle		1.StartWalk		2.Walking	3.Smash		4.Slash		5.Dispute
	*/

protected: // 변수
	float m_fPreFrameTime{ 0 };

	PlayerInfo m_StatusInfo;

	CCollisionManager * m_pColManager{ NULL };
};

