#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"
#include "00.Global/01.Utility/Enumerations.h"

class CRoider : public CAnimatedObject
{
public: // 생성자, 소멸자
	CRoider(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes = 1);
	virtual ~CRoider();

public:	// 외부 함수
	virtual void Animate(float timeElapsed);
	virtual void Render(CCamera *pCamera, UINT instanceCnt = 1);

	virtual 	void SetState(StatesType newState);

	virtual void PlayIdle(float timeElapsed);
	virtual void PlayWalk(float timeElapsed);
	virtual void PlayChase(float timeElapsed, shared_ptr<CWayFinder> pWayFinder);
	virtual void PlayAttack(float timeElapsed, shared_ptr<CWayFinder> pWayFinder);
	virtual void PlayRemove(float timeElapsed);

	virtual void ReceiveDamage(float damage)
	{
		m_StatusInfo.HP -= damage * Compute_Defence(m_StatusInfo.Def);
		if (m_StatusInfo.HP <= 0&&m_curState!=States::Die) {
			SetState(States::Die);
		}
		m_activated = true;
	}

	virtual CommonInfo* GetCommonStatus() { return &m_StatusInfo; };

protected:	// 내부 함수
	virtual void AdjustAnimationIndex();
	void AnimateByCurState();
	void ReadyToAtk();

protected:	// 변수
	bool m_activated{ false };
	/*
	0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
	*/

	ObjectType m_ObjectType{ ObjectType::Loyde };

	/*
	0. SwordPlayer, 1. StaffPlayer,   2. BowPlayer,
	3. SwordMinion,	4. StaffMinion,   5. BowMinion,
	6. Loyde,   	7. CAM,     	  8. GOLEM,
	9. FirstTower,  10. SecnondTower, 11. Nexus
	*/

	CommonInfo m_StatusInfo;

	XMFLOAT3 m_spawnLocation;	// 생성 위치
	XMFLOAT3 m_nexusLocation;	// 팀으로 합류 이후 찾아가야 할 위치
};
