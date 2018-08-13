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

protected: // ���� �Լ�

protected: // ����
};

