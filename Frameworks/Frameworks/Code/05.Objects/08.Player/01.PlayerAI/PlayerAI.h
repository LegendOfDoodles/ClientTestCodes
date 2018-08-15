#pragma once
#include "05.Objects/08.Player/Player.h"

class CPlayerAI : public CPlayer
{
public:	// 생성자, 소멸자
	CPlayerAI(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes);
	virtual ~CPlayerAI();

public:	// 외부 함수
	void WalkCurLine(float timeElapsed);		// 현재 라인 진행
	void HelpOtherPlayer(float timeElapsed);	// 다른 플레이어 돕기
	void ReturnOriginLine(float timeElapsed);	// 원래 라인으로 복귀
	void AttackEnemy(float timeElapsed);		// 적 공격
	void RetreatFromFight(float timeElapsed);	// 후퇴
	void AvoidAttack(float timeElapsed);		// 공격 회피
	void ChaseEnemy(float timeElapsed);		// 공격 회피
	void CaculateUtility();

	virtual void Animate(float timeElapsed);

protected: // 내부 함수
	float GetFrontLineValue();
	void ReceiveEnemyList();
	void ReceiveTeamList();
	std::list<CCollisionObject*> EnemyWithinRange();
protected: // 변수
	std::list<CCollisionObject*>* m_plstEnemy;
	std::list<CCollisionObject*>* m_plstMyTeam;

	float* m_arrUtilities;
	int m_nUtilities{ 2 };

	float timedelay{ 0 };
};

