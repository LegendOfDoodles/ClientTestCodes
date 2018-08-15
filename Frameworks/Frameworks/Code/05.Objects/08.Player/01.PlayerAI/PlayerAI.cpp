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
	m_arrUtilities = new float[m_nUtilities];
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


void CPlayerAI::CaculateUtility()
{
	ReceiveEnemyList();
	ReceiveTeamList();

	//���� ���� value
	int index = 0;

	///////////////////////���� value
	float lineDistance = GetFrontLineValue();

	float otherlinesupport = (abs(lineDistance)/3000.0f);//x��ǥ�� 3000���� �̻��� �� �ִ�
	m_arrUtilities[index++] = otherlinesupport;
	///////////////////////�÷��̾� ���� value
	CPlayer* target_enemy{NULL};

	std::list<CCollisionObject*> EnemiesInRange = EnemyWithinRange();
	if (EnemiesInRange.size() == 0) {
		m_arrUtilities[index++] = 0; 
	}
	else {
		for (auto i = EnemiesInRange.begin(); i != EnemiesInRange.end(); ++i) {
			if (!target_enemy) {
				target_enemy = dynamic_cast<CPlayer*>(*i);
			}
			else if(target_enemy->GetPlayerStatus()->HP > dynamic_cast<CPlayer*>(*i)->GetPlayerStatus()->HP){
				target_enemy = dynamic_cast<CPlayer*>(*i);
			}
		}
		XMFLOAT2 apos = XMFLOAT2(target_enemy->GetPosition().x, target_enemy->GetPosition().z);
		XMFLOAT2 bpos = XMFLOAT2(GetPosition().x, GetPosition().z);
		float distance = Vector2::Distance(apos, bpos);

		m_arrUtilities[index++] = CLAMP((target_enemy->GetPlayerStatus()->HP-m_StatusInfo.Atk)/target_enemy->GetPlayerStatus()->HP,0.2f,1) * CLAMP((m_sightRange-distance) / m_sightRange,0,1);
	}

	/////////////////////////���� value


	for (int i = 0; i < m_nUtilities; ++i) {
		printf("Index[%d]: %f \n", i, m_arrUtilities[i]);
	}
	printf("\n\n\n");
}

void CPlayerAI::Animate(float timeElapsed)
{
	CPlayer::Animate(timeElapsed);
	timedelay += timeElapsed;
	if (timedelay >= 2.0f) {
		CaculateUtility();
		timedelay = 0;
	}

}


////////////////////////////////////////////////////////////////////////
// ���� �Լ�



//������ ��� bottom���� �켼
float CPlayerAI::GetFrontLineValue()
{
	XMFLOAT2 TopLine = m_pColManager->GetFrontLinePosition(0, m_TeamType);
	
	XMFLOAT2 BottomLine = m_pColManager->GetFrontLinePosition(1, m_TeamType);

	float lineDistance;

	if (m_TeamType == TeamType::Blue) {
		lineDistance = (TopLine.x) - (BottomLine.x);
	}
	else if (m_TeamType == TeamType::Red) {
		lineDistance = (TERRAIN_SIZE_WIDTH - TopLine.x) - (TERRAIN_SIZE_WIDTH - BottomLine.x);
	}

	return lineDistance;
}

void CPlayerAI::ReceiveEnemyList()
{
	m_plstEnemy = m_pColManager->GetEnemyList(this,m_TeamType);
}

void CPlayerAI::ReceiveTeamList()
{
	m_plstMyTeam = m_pColManager->GetTeamList(this, m_TeamType);
}

std::list<CCollisionObject*> CPlayerAI::EnemyWithinRange()
{
	CCollisionObject* FrontObject{ NULL };
	std::list<CCollisionObject*>::iterator i;
	std::list<CCollisionObject*> retunlist;
	retunlist.clear();
	for (i = m_plstEnemy->begin(); i != m_plstEnemy->end(); ++i)
	{
		if ((*i)->GetType() < 4) {
			XMFLOAT2 apos = XMFLOAT2((*i)->GetPosition().x, (*i)->GetPosition().z);
			XMFLOAT2 bpos = XMFLOAT2(GetPosition().x, GetPosition().z);
			float distance = Vector2::Distance(apos, bpos);
			if(distance<= m_sightRange)
				retunlist.push_back((*i));
		}
	}
	return retunlist;
}
