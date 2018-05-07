#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/02.AnimatedObject/AnimatedObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"

enum MinionState {
	Idle,

	Attack,
	
	Walking,
	
	Die
};

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

protected:	// 내부 함수

protected:	// 변수
	MinionState m_CurrState = MinionState::Idle;
	MinionState m_NextState = MinionState::Idle;
	
	/*
	0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
	*/
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
	CSwordMinion(CCreateMgr *pCreateMgr);
	CSwordMinion(CCreateMgr *pCreateMgr, int nMeshes);
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
	CMagicMinion(CCreateMgr *pCreateMgr);
	CMagicMinion(CCreateMgr *pCreateMgr, int nMeshes);
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
	CBowMinion(CCreateMgr *pCreateMgr);
	CBowMinion(CCreateMgr *pCreateMgr, int nMeshes);
	virtual ~CBowMinion();

public:	// 외부 함수
	virtual void Animate(float timeElapsed);
	/*
	0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
	*/
};
