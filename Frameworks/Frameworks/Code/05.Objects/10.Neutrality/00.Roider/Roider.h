#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"
#include "00.Global/01.Utility/Enumerations.h"

class CRoider : public CAnimatedObject
{
public: // ������, �Ҹ���
	CRoider(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes = 1);
	virtual ~CRoider();

public:	// �ܺ� �Լ�
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

protected:	// ���� �Լ�
	virtual void AdjustAnimationIndex();
	void AnimateByCurState();
	void ReadyToAtk();

protected:	// ����
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

	XMFLOAT3 m_spawnLocation;	// ���� ��ġ
	XMFLOAT3 m_nexusLocation;	// ������ �շ� ���� ã�ư��� �� ��ġ
};
