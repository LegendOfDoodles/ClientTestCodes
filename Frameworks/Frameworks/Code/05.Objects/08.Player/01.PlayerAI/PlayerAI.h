#pragma once
#include "05.Objects/08.Player/Player.h"

class CPlayerAI : public CPlayer
{
public:	// ������, �Ҹ���
	CPlayerAI(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes);
	virtual ~CPlayerAI();

public:	// �ܺ� �Լ�
	void WalkCurLine(float timeElapsed);		// ���� ���� ����
	void HelpOtherPlayer(float timeElapsed);	// �ٸ� �÷��̾� ����
	void ReturnOriginLine(float timeElapsed);	// ���� �������� ����
	void AttackEnemy(float timeElapsed);		// �� ����
	void RetreatFromFight(float timeElapsed);	// ����
	void AvoidAttack(float timeElapsed);		// ���� ȸ��
	void ChaseEnemy(float timeElapsed);		// ���� ȸ��
	void CaculateUtility();

	virtual void Animate(float timeElapsed);

protected: // ���� �Լ�
	void PlayAction(int index);

	float GetFrontLineValue();
	void ReceiveEnemyList();
	void ReceiveTeamList();
	std::list<CCollisionObject*> EnemyWithinRange();
protected: // ����
	std::list<CCollisionObject*>* m_plstEnemy;
	std::list<CCollisionObject*>* m_plstMyTeam;

	CPlayer* m_pTarget_Enemy;
	float* m_arrUtilities;
	int m_nUtilities{ 4 };


	float timedelay{ 0 };
};

