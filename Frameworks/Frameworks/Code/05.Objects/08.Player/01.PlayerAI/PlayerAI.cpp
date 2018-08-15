#include "stdafx.h"
#include "PlayerAI.h"

/// <summary>
/// 목적: 플레이어 AI 관리 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-08-13
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CPlayerAI::CPlayerAI(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CPlayer(pCreateMgr, nMeshes)
{
	m_arrUtilities = new float[m_nUtilities];
}

CPlayerAI::~CPlayerAI()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CPlayerAI::WalkCurLine(float timeElapsed)
{
	// 현재 패스 정보 필요함
	// 아 미니언 패스 필요함
}

void CPlayerAI::HelpOtherPlayer(float timeElapsed)
{
	// 다른 플레이어 정보 필요함
}

void CPlayerAI::ReturnOriginLine(float timeElapsed)
{
	// 원래 패스 정보 필요함
	// 아 미니언 패스 필요함
}

void CPlayerAI::AttackEnemy(float timeElapsed)
{
	// 적 정보 필요함
}

void CPlayerAI::RetreatFromFight(float timeElapsed)
{
	// 현재 패스 정보 필요함
	// 적 미니언 패스 필요함
}

void CPlayerAI::AvoidAttack(float timeElapsed)
{
	// SubPath 추가 필요
	// 후퇴와 구분을 어떻게 하냐?
	// 내 생각엔 회피나 후퇴 중에 포기해야 함
}

void CPlayerAI::ChaseEnemy(float timeElapsed)
{
	// 현재 Enemy 정보 필요
}


void CPlayerAI::CaculateUtility()
{
	ReceiveEnemyList();
	ReceiveTeamList();

	//라인 지원 value
	int index = 0;

	///////////////////////지원 value
	float lineDistance = GetFrontLineValue();

	float otherlinesupport = (abs(lineDistance)/3000.0f);//x좌표가 3000차이 이상일 때 최대
	m_arrUtilities[index++] = otherlinesupport;
	///////////////////////플레이어 공격 value
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

	/////////////////////////후퇴 value
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

	m_arrUtilities[index++] = 0.5f;//일반 라인전

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
// 내부 함수



void CPlayerAI::PlayAction(int index)
{
	switch (index) {
	case 0://라인 지원

		break;
	case 1://플레이어 공격

		break;
	case 2://후퇴
		break;
	case 3://라인전

		break;
	}
}

//음수일 경우 bottom라인 우세
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
