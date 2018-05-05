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

class CMinion : public CAnimatedObject		// ��� AnimatedObject���� �޴� �ɷ� ����  ��
{
public: // ������, �Ҹ���
	CMinion(CCreateMgr *pCreateMgr, int nMeshes = 1);
	virtual ~CMinion();

public:	// �ܺ� �Լ�
	virtual void Animate(float timeElapsed);
	virtual void Render(CCamera *pCamera, UINT instanceCnt = 1);
	virtual void SetPathToGo(Path *path);

	virtual void SetPosition(float x, float z);

	void SetSkeleton(CSkeleton *skeleton) {
		m_nAniLength[m_nAniCnt] = skeleton->GetAnimationLength();
		m_pSkeleton[m_nAniCnt++] = *skeleton;
	}

	void AniStateSet() {
		switch (m_CurrAnimationState) {
		case MinionState::Idle:
			m_NextAnimationState = MinionState::Attack;
			break;

		case MinionState::Attack:
			m_NextAnimationState = MinionState::Walking;
			break;

		case MinionState::Walking:
			m_NextAnimationState = MinionState::Die;
			break;
		case MinionState::Die:
			m_NextAnimationState = MinionState::Idle;
			break;

		default:
			break;
		}
		m_fFrameTime = 0;
	}



protected:	// ���� �Լ�

protected:	// ����
	MinionState m_CurrAnimationState = MinionState::Walking;
	MinionState m_NextAnimationState = MinionState::Walking;
	CSkeleton	m_pSkeleton[20];

	int m_nAniLength[20];
	int m_nCurrAnimation{ 3 };
	/*
	0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
	*/
	int m_nAniCnt{ 0 };
	
	
	float m_fFrameTime{ 0 };
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
public: // ������, �Ҹ���
	CSwordMinion(CCreateMgr *pCreateMgr);
	CSwordMinion(CCreateMgr *pCreateMgr, int nMeshes);
	virtual ~CSwordMinion();

public:	// �ܺ� �Լ�
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
public: // ������, �Ҹ���
	CMagicMinion(CCreateMgr *pCreateMgr);
	CMagicMinion(CCreateMgr *pCreateMgr, int nMeshes);
	virtual ~CMagicMinion();

public:	// �ܺ� �Լ�
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
public: // ������, �Ҹ���
	CBowMinion(CCreateMgr *pCreateMgr);
	CBowMinion(CCreateMgr *pCreateMgr, int nMeshes);
	virtual ~CBowMinion();

public:	// �ܺ� �Լ�
	virtual void Animate(float timeElapsed);
	/*
	0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
	*/
};
