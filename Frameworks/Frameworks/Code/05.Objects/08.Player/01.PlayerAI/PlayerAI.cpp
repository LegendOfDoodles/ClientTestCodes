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

////////////////////////////////////////////////////////////////////////
// 내부 함수