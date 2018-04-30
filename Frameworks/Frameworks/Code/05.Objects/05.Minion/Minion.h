#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"

enum MinionState {
	Idle,

	Attack,
	
	Walking,
	
	Die
};

class CHeightMapTerrain;

class CMinion : public CBaseObject		// 상속 AnimatedObject에서 받는 걸로 수정  필
{
public: // 생성자, 소멸자
	CMinion(CCreateMgr *pCreateMgr, int nMeshes = 1);
	virtual ~CMinion();

public:	// 외부 함수
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

	void SetTerrain(CHeightMapTerrain *pTerrain) { m_pTerrain = pTerrain; }

protected:	// 내부 함수
	bool IsArrive(XMFLOAT2& nextPos, float dst);

protected:	// 변수
	MinionState m_CurrAnimationState = MinionState::Walking;
	MinionState m_NextAnimationState = MinionState::Walking;
	CSkeleton	m_pSkeleton[20];

	int m_nAniLength[20];
	int m_nCurrAnimation{ 3 };
	/*
	0. Idle		1.Attack	2.Attack2	3.StartWalk		4.Walking	5.Die
	*/
	float m_fFrameTime{ 0 };

	int m_nAniCnt{ 0 };

	XMFLOAT2 m_destination{ -1,  -1 };	// 얘를 깔끔하게 API 같이 만들 필요 있을 듯

	CHeightMapTerrain * m_pTerrain{ NULL };	// 나중에 상속 부모 바뀌면 수정 필요
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
