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
	m_pTarget_Enemy = NULL;

	std::list<CCollisionObject*> EnemiesInRange = EnemyWithinRange();
	if (EnemiesInRange.size() == 0) {
		m_arrUtilities[index++] = 0; 
	}
	else {
		for (auto i = EnemiesInRange.begin(); i != EnemiesInRange.end(); ++i) {
			if (!m_pTarget_Enemy) {
				m_pTarget_Enemy = dynamic_cast<CPlayer*>(*i);
			}
			else if(m_pTarget_Enemy->GetPlayerStatus()->HP > dynamic_cast<CPlayer*>(*i)->GetPlayerStatus()->HP){
				m_pTarget_Enemy = dynamic_cast<CPlayer*>(*i);
			}
		}
		XMFLOAT2 apos = XMFLOAT2(m_pTarget_Enemy->GetPosition().x, m_pTarget_Enemy->GetPosition().z);
		XMFLOAT2 bpos = XMFLOAT2(GetPosition().x, GetPosition().z);
		float distance = Vector2::Distance(apos, bpos);

		m_arrUtilities[index++] = CLAMP(2*m_StatusInfo.Atk / m_pTarget_Enemy->GetPlayerStatus()->HP,0,1);
	}

	/////////////////////////���� value
	float fighting_value=0;
	if (EnemiesInRange.size() == 0) {
		m_arrUtilities[index++] = CLAMP((m_StatusInfo.maxHP - m_StatusInfo.HP) / m_StatusInfo.maxHP, 0, 1);
	}
	else {
		int cnt = 0;
		for (auto i = m_plstEnemy->begin(); i != m_plstEnemy->end(); ++i) {
			if ((*i)->GetType() < 4) {
				PlayerInfo* info = (*i)->GetPlayerStatus();
				fighting_value += Compute_Fighting_Value(info->HP, info->Atk, info->Def);
				cnt++;
			}
			else if ((*i)->GetType() < 12) {
				CommonInfo* info = (*i)->GetCommonStatus();
				fighting_value += Compute_Fighting_Value(info->HP, info->Atk, info->Def);
				cnt++;
			}
		}
		fighting_value = (fighting_value / cnt)*(1+cnt*0.1f);
		
		m_arrUtilities[index++] = CLAMP((m_StatusInfo.maxHP - m_StatusInfo.HP)/ m_StatusInfo.maxHP, 0, 1) 
										* CLAMP(Compute_Fighting_Value(m_StatusInfo.HP, m_StatusInfo.Atk, m_StatusInfo.Def),0,1);
	}

	m_arrUtilities[index++] = 0.5f;//�Ϲ� ������

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
		int index = 0;
		for (int i = 1; i < m_nUtilities; ++i) {
			if (m_arrUtilities[i] > m_arrUtilities[index]) index = i;
		}
		PlayAction(index);
		timedelay = 0;
	}

}


////////////////////////////////////////////////////////////////////////
// ���� �Լ�



void CPlayerAI::PlayAction(int index)
{
	switch (index) {
	case 0://���� ����

		break;
	case 1://�÷��̾� ����

		break;
	case 2://����
		break;
	case 3://������

		break;
	}
}

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
