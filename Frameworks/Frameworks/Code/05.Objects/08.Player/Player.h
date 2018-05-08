#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"
#include "00.Global/01.Utility/Enumerations.h"

class CCollisionManager;

class CPlayer : public CAnimatedObject
{
	

public:
	enum PlayerAnimation {
		Idle,
		StartWalk,
		Walking,
		SkillQ,
		SkillE,
		SkillR
	};

public:
	CPlayer(CCreateMgr *pCreateMgr, int nMeshes = 1);
	virtual ~CPlayer();

public:	// 외부 함수
	virtual void Animate(float timeElapsed);
	virtual void Render(CCamera *pCamera, UINT instanceCnt = 1);
	virtual void SetPathToGo(Path *path);

	virtual void SetPosition(float x, float z);
	virtual void SetCollisionManager(CCollisionManager* manager) {
		m_pColManager = manager;
	}

	virtual void ActiveSkill(PlayerAnimation act);
	
	/*
	0. Idle		1.StartWalk		2.Walking	3.Smash		4.Slash		5.Dispute
	*/
	
	float m_fPreFrameTime{ 0 };

	States m_CurrState = States::Idle;
	CCollisionManager * m_pColManager{ NULL };
};

