#include "stdafx.h"
#include "PlayerAI.h"

/// <summary>
/// ����: �÷��̾� AI ���� Ŭ����
/// ���� ������:  �質��
/// ������ ���:  ������, �質��
/// ���� ���� ��¥: 2018-08-13
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CPlayerAI::CPlayerAI(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CPlayer(pCreateMgr, nMeshes)
{
}

CPlayerAI::~CPlayerAI()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CPlayerAI::WalkCurLine(float timeElapsed)
{
	// ���� �н� ���� �ʿ���
	// �� �̴Ͼ� �н� �ʿ���
}

void CPlayerAI::HelpOtherPlayer(float timeElapsed)
{
	// �ٸ� �÷��̾� ���� �ʿ���
}

void CPlayerAI::ReturnOriginLine(float timeElapsed)
{
	// ���� �н� ���� �ʿ���
	// �� �̴Ͼ� �н� �ʿ���
}

void CPlayerAI::AttackEnemy(float timeElapsed)
{
	// �� ���� �ʿ���
}

void CPlayerAI::RetreatFromFight(float timeElapsed)
{
	// ���� �н� ���� �ʿ���
	// �� �̴Ͼ� �н� �ʿ���
}

void CPlayerAI::AvoidAttack(float timeElapsed)
{
	// SubPath �߰� �ʿ�
	// ����� ������ ��� �ϳ�?
	// �� ������ ȸ�ǳ� ���� �߿� �����ؾ� ��
}

void CPlayerAI::ChaseEnemy(float timeElapsed)
{
	// ���� Enemy ���� �ʿ�
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�