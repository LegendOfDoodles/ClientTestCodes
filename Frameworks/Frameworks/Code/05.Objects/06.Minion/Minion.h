#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"
#include "00.Global/01.Utility/Enumerations.h"


class CMinion : public CAnimatedObject		// 상속 AnimatedObject에서 받는 걸로 수정  필
{
public: // 생성자, 소멸자
	CMinion(CCreateMgr *pCreateMgr, int nMeshes = 1);
	virtual ~CMinion();

public:	// 외부 함수
	virtual void Animate(float timeElapsed);
	virtual void Render(CCamera *pCamera, UINT instanceCnt = 1);
	virtual void SetPathToGo(Path *path);

	virtual void SetPosition(float x, float z);

	virtual void ReceiveDamage(float damage)
	{
		m_StatusInfo.HP -= damage * Compute_Defence(m_StatusInfo.Def);
		if (m_StatusInfo.HP <= 0)
			std::cout << "Die\n";

	}

protected:	// 내부 함수

protected:	// 변수
	States m_CurrState = States::Idle;
	
	/*
	0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
	*/

	ObjectType m_ObjectType = ObjectType::SwordMinion;

	/*
	0. SwordPlayer, 1. StaffPlayer,   2. BowPlayer,
	3. SwordMinion,	4. StaffMinion,   5. BowMinion,
	6. Loyde,   	7. CAM,     	  8. GOLEM,
	9. FirstTower,  10. SecnondTower, 11. Nexus
	*/

	CommonInfo m_StatusInfo;

};

class CSwordMinion : public CMinion
{
	enum SwordMinionAnimation {
		Idle,
		Attack1,
		Attack2,
		StartWalk,
		Walking,
		Die
	};
public: // 생성자, 소멸자
	CSwordMinion(CCreateMgr *pCreateMgr, int nMeshes=1);
	virtual ~CSwordMinion();

public:	// 외부 함수
	virtual void Animate(float timeElapsed);
	/*
	0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
	*/
};


class CMagicMinion : public CMinion
{
	enum MagicMinionAnimation {
		Idle,
		Attack1,
		Attack2,
		StartWalk,
		Walking,
		Die
	};
public: // 생성자, 소멸자
	CMagicMinion(CCreateMgr *pCreateMgr, int nMeshes=1);
	virtual ~CMagicMinion();

public:	// 외부 함수
	virtual void Animate(float timeElapsed);
	/*
	0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
	*/
};


class CBowMinion : public CMinion
{
	enum BowMinionAnimation {
		Idle		,
		Attack,
		StartWalk,
		Walking	,
		Die
	};
public: // 생성자, 소멸자
	CBowMinion(CCreateMgr *pCreateMgr, int nMeshes=1);
	virtual ~CBowMinion();

public:	// 외부 함수
	virtual void Animate(float timeElapsed);
	/*
	0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
	*/
};
