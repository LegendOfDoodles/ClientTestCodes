#include "stdafx.h"
#include "PlayerAI.h"

/// <summary>
/// 목적: 플레이어 AI 관리 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-10-04
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CPlayerAI::CPlayerAI(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CPlayer(pCreateMgr, nMeshes)
{
	m_detectRange = CONVERT_PaperUnit_to_InG(80.0f);
	m_attackRange = CONVERT_PaperUnit_to_InG(18);

	m_StatusInfo.WalkSpeed = 1.f;

	// 캐릭터의 성격을 랜덤으로 설정한다.
	m_characteristics[PlayerAIActions::Push_Line] = RandInRange(0.6f, 1.0f);
	m_characteristics[PlayerAIActions::Attack_Player] = RandInRange(0.5f, 0.9f);
	m_characteristics[PlayerAIActions::Fall_Back] = RandInRange(0.3f, 0.8f);
	m_characteristics[PlayerAIActions::Support_Line] = RandInRange(0.2f, 0.7f);
}

CPlayerAI::~CPlayerAI()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CPlayerAI::Animate(float timeElapsed)
{
	CPlayer::Animate(timeElapsed);
	AnimateByCurState();
	m_refreshTimeChecker += timeElapsed;

	if (m_waitForMinionSpawn < COOLTIME_SPAWN_MINION * 2.f)
	{
		m_waitForMinionSpawn += timeElapsed;
		return;
	}

	// 사망한 상태인 경우 다음 행동을 계산할 필요가 없으므로
	if (m_curState == States::Die || m_curState == States::Remove) return;

	if (m_refreshTimeChecker >= TIME_REFRESH_ACTION) 
	{
		int newActionIndex{ CaculateUtility() };
		if (m_currentActionIndex != newActionIndex)
		{
			m_dataPrepared = false;
		}
		m_refreshTimeChecker = 0;
	}
	PlayAction(m_currentActionIndex, timeElapsed);
}

void CPlayerAI::PrepareData()
{
	XMFLOAT3 curPos{ GetPosition() };
	if (curPos.z > TERRAIN_SIZE_HEIGHT * 0.5f)
	{
		m_ownLine = PlayerLine::TopLine;
		m_otherLine = PlayerLine::BottomLine;
	}
	else
	{
		m_ownLine = PlayerLine::BottomLine;
		m_otherLine = PlayerLine::TopLine;
	}
}

int CPlayerAI::CaculateUtility()
{
	ReceiveEnemyList();
	ReceiveTeamList();

	// 가장 큰 효용 확인용 변수
	PlayerAIActions biggestUtility{ PlayerAIActions::Push_Line };
	float maxUtility{ GetPushLineUtility() };
	float curUtility{ GetAttackPlayerUtility() };

	if (maxUtility < curUtility)
	{
		biggestUtility = PlayerAIActions::Attack_Player;
		maxUtility = curUtility;
	}
	
	curUtility = GetFallBackUtility();
	if (maxUtility < curUtility)
	{
		biggestUtility = PlayerAIActions::Fall_Back;
		maxUtility = curUtility;
	}

	curUtility = GetSupportLineUtility();
	if (maxUtility < curUtility)
	{
		biggestUtility = PlayerAIActions::Support_Line;
	}

	return static_cast<int>(biggestUtility);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CPlayerAI::PlayAction(int index, float timeElapsed)
{
	switch (index) {
	case Push_Line:
		PushLine(timeElapsed);
		break;
	case Attack_Player:
		AttackPlayer(timeElapsed);
		break;
	case Fall_Back:
		FallBack(timeElapsed);
		break;
	case Support_Line:
		SupportLine(timeElapsed);
		break;
	}
}

void CPlayerAI::PushLine(float timeElapsed)
{
	if (!m_dataPrepared)
	{
		XMFLOAT3 curPos{ GetPosition() };
		Path *newPath{ NULL };
		if (m_TeamType == TeamType::Blue)
		{
			// 위로 가야함
			if (m_ownLine == PlayerLine::TopLine)
			{
				newPath = new Path(m_pathes[WayType::Blue_Up]);
			}
			else
			{
				newPath = new Path(m_pathes[WayType::Blue_Down]);
			}
			newPath->remove_if([curPos](CPathEdge& edge) {
				return edge.To().x < curPos.x;
			});
		}
		else if (m_TeamType == TeamType::Red)
		{
			// 위로 가야함
			if (m_ownLine == PlayerLine::TopLine)
			{
				newPath = new Path(m_pathes[WayType::Red_Up]);
			}
			else
			{
				newPath = new Path(m_pathes[WayType::Red_Down]);
			}
			newPath->remove_if([curPos](CPathEdge& edge) {
				return edge.To().x > curPos.x;
			});
		}

		SetPathToGo(newPath);
		GenerateSubPathToPosition(m_pWayFinder, XMFLOAT3(newPath->front().From().x, curPos.y, newPath->front().From().y));

		SetState(StatesType::Walk);

		m_dataPrepared = true;
	}

	if (!CheckEnemyState(m_pEnemy))
	{
		SetEnemy(NULL);
		CCollisionObject* enemy{ m_pColManager->RequestNearObject(this, m_detectRange, m_TeamType) };

		if (enemy && Attackable(enemy))
		{
			SetEnemy(enemy);
		}
	}

	if (m_pEnemy && Attackable(m_pEnemy))
	{
		//if (m_StatusInfo.QSkillCoolTime >= 1.f)
		//{
		//	ActiveSkill(AnimationsType::SkillQ);
		//}
		//else if (m_StatusInfo.QSkillCoolTime >= 1.f)
		//{
		//	ActiveSkill(AnimationsType::SkillW);
		//}
		//else if (m_StatusInfo.QSkillCoolTime >= 1.f)
		//{
		//	ActiveSkill(AnimationsType::SkillE);
		//}
		//else if (m_StatusInfo.QSkillCoolTime >= 1.f)
		//{
		//	ActiveSkill(AnimationsType::SkillR);
		//}
		//else
		{
			ActiveSkill(AnimationsType::Attack1);
		}
	}
	else
	{
		SetNextState(States::Walk);
	}

	if (m_curState == StatesType::Walk)
	{
		if (NoneDestination(PathType::Sub))
		{
			if (MoveToDestination(timeElapsed, m_pWayFinder) == States::Done) SetState(States::Idle);
		}
		else
		{
			MoveToSubDestination(timeElapsed);
		}
	}
}

void CPlayerAI::AttackPlayer(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);
}

void CPlayerAI::FallBack(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);
}

void CPlayerAI::SupportLine(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);
}

float CPlayerAI::GetPushLineUtility()
{
	// 전체 체력 대 현재 체력의 비율 * (캐릭터 성격 계수 + 방어력 계수 + HP 흡수율 + 회피율)
	return (m_StatusInfo.HP / m_StatusInfo.maxHP) * (m_characteristics[PlayerAIActions::Push_Line] + (100.f + m_StatusInfo.Def / 100.0f) + m_StatusInfo.AbsorptionRate + m_StatusInfo.EvationRate);
}

float CPlayerAI::GetAttackPlayerUtility()
{
	//m_pTarget_Enemy = NULL;

	//CollisionObjectList EnemiesInRange = EnemyWithinRange();
	//if (EnemiesInRange.size() == 0) {
	//	m_arrUtilities[biggestUtilityIndex++] = 0;
	//}
	//else {
	//	for (auto i = EnemiesInRange.begin(); i != EnemiesInRange.end(); ++i) {
	//		if (!m_pTarget_Enemy) {
	//			m_pTarget_Enemy = dynamic_cast<CPlayer*>(*i);
	//		}
	//		else if (m_pTarget_Enemy->GetPlayerStatus()->HP > dynamic_cast<CPlayer*>(*i)->GetPlayerStatus()->HP) {
	//			m_pTarget_Enemy = dynamic_cast<CPlayer*>(*i);
	//		}
	//	}
	//	XMFLOAT2 apos = XMFLOAT2(m_pTarget_Enemy->GetPosition().x, m_pTarget_Enemy->GetPosition().z);
	//	XMFLOAT2 bpos = XMFLOAT2(GetPosition().x, GetPosition().z);

	//	m_arrUtilities[biggestUtilityIndex++] = CLAMP(2 * m_StatusInfo.Atk / m_pTarget_Enemy->GetPlayerStatus()->HP, 0, 1);
	//}
	return 0.0f;
}

float CPlayerAI::GetFallBackUtility()
{
	//float fighting_value = 0;
	//if (EnemiesInRange.size() == 0) {
	//	m_arrUtilities[biggestUtilityIndex++] = CLAMP((m_StatusInfo.maxHP - m_StatusInfo.HP) / m_StatusInfo.maxHP, 0, 1);
	//}
	//else {
	//	int cnt = 0;
	//	for (auto i = m_plstEnemy->begin(); i != m_plstEnemy->end(); ++i) {
	//		if ((*i)->GetType() < 4) {
	//			PlayerInfo* info = (*i)->GetPlayerStatus();
	//			fighting_value += Compute_Fighting_Value(info->HP, info->Atk, info->Def);
	//			cnt++;
	//		}
	//		else if ((*i)->GetType() < 12) {
	//			CommonInfo* info = (*i)->GetCommonStatus();
	//			fighting_value += Compute_Fighting_Value(info->HP, info->Atk, info->Def);
	//			cnt++;
	//		}
	//	}
	//	fighting_value = (fighting_value / cnt)*(1 + cnt * 0.1f);
	return 0.0f;
}

float CPlayerAI::GetSupportLineUtility()
{
	//	m_arrUtilities[biggestUtilityIndex++] = CLAMP((m_StatusInfo.maxHP - m_StatusInfo.HP) / m_StatusInfo.maxHP, 0, 1)
	//		* CLAMP(Compute_Fighting_Value(m_StatusInfo.HP, m_StatusInfo.Atk, m_StatusInfo.Def), 0, 1);
	//}

	//float lineDistance = GetFrontLineValue();

	//float otherlinesupport{ (abs(lineDistance) / 3000.0f) }; // x좌표가 3000차이 이상일 때 최대
	return 0.0f;
}

float CPlayerAI::GetFrontLineValue()
{
	//음수일 경우 bottom라인 우세
	XMFLOAT2 curLineFrontPos{ m_pColManager->GetFrontLinePosition(m_ownLine, m_TeamType) };
	XMFLOAT2 otherLineFrontPos{ m_pColManager->GetFrontLinePosition(m_otherLine, m_TeamType) };

	float lineDistance{ 0.f };

	if (m_TeamType == TeamType::Blue) 
	{
		lineDistance = (curLineFrontPos.x) - (otherLineFrontPos.x);
	}
	else if (m_TeamType == TeamType::Red)
	{
		lineDistance = (TERRAIN_SIZE_WIDTH - curLineFrontPos.x) - (TERRAIN_SIZE_WIDTH - otherLineFrontPos.x);
	}

	return lineDistance;
}

void CPlayerAI::ReceiveEnemyList()
{
	m_plstEnemy = m_pColManager->GetEnemyList(m_TeamType);
}

void CPlayerAI::ReceiveTeamList()
{
	m_plstMyTeam = m_pColManager->GetTeamList(this, m_TeamType);
}

std::list<CCollisionObject*> CPlayerAI::EnemyWithinRange()
{
	std::list<CCollisionObject*>::iterator i;
	std::list<CCollisionObject*> returnList;
	returnList.clear();
	for (i = m_plstEnemy->begin(); i != m_plstEnemy->end(); ++i)
	{
		if ((*i)->GetType() < 4) {
			XMFLOAT2 apos = XMFLOAT2((*i)->GetPosition().x, (*i)->GetPosition().z);
			XMFLOAT2 bpos = XMFLOAT2(GetPosition().x, GetPosition().z);
			float distance = Vector2::Distance(apos, bpos);
			if(distance<= m_sightRange)
				returnList.push_back((*i));
		}
	}
	return returnList;
}

void CPlayerAI::AnimateByCurState()
{
	switch (m_curState) {
	case States::Attack:
		if (GetAnimTimeRemainRatio() <= 0.05f)
		{
			if (m_pEnemy) LookAt(m_pEnemy->GetPosition());
		}
		break;
	default:
		break;
	}
}
