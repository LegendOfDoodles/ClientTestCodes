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

	/////////////////////////후퇴 value


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
// 내부 함수



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
