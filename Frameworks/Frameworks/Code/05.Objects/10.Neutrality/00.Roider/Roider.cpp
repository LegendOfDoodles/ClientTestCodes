#include "stdafx.h"
#include "Roider.h"

/// <summary>
/// 목적: 중립 몬스터(로이더) 클래스 분할
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-07-20
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CRoider::CRoider(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CAnimatedObject(pCreateMgr, nMeshes)
{
	m_sightRange = CONVERT_PaperUnit_to_InG(80.0f);
	m_detectRange = CONVERT_PaperUnit_to_InG(40.0f);
	m_speed = CONVERT_cm_to_InG(3.237f);
}

CRoider::~CRoider()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CRoider::Animate(float timeElapsed)
{
	AnimateByCurState();
	AdjustAnimationIndex();

	m_fPreFrameTime = m_fFrameTime;
	m_fFrameTime += 30 * timeElapsed;

	if (m_fFrameTime > m_nAniLength[m_nAniIndex]) {
		while (m_fFrameTime > m_nAniLength[m_nAniIndex])
			m_fFrameTime -= m_nAniLength[m_nAniIndex];
	}

	// if(MoveToDestination(timeElapsed) == States::Done) SetState(States::Idle);

	CAnimatedObject::Animate(timeElapsed);
}

void CRoider::Render(CCamera * pCamera, UINT instanceCnt)
{
	OnPrepareRender();

	if (!IsVisible(pCamera)||!m_Detected) return;

	if (m_pMaterial)
	{
		m_pMaterial->Render(pCamera);
		m_pMaterial->UpdateShaderVariables();
	}

	if (m_cbvGPUDescriptorHandle.ptr)
		m_pCommandList->SetGraphicsRootDescriptorTable(6, m_cbvGPUDescriptorHandle);

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(instanceCnt);
		}
	}
}

void CRoider::SetState(StatesType newState)
{
	m_nextState = m_curState = newState;

	switch (newState)
	{
	case States::Idle:
		m_nCurrAnimation = Animations::Idle;
		break;
	case States::Walk:
		RegenerateLookAt();
		m_nCurrAnimation = Animations::StartWalk;
		break;
	case States::Chase:
		m_availableTime = 0.0f;
		m_nCurrAnimation = Animations::StartWalk;
		break;
	case States::Attack:
		m_nCurrAnimation = Animations::Attack1;
		m_fFrameTime = 0;
		break;
	case States::Die:
		m_nCurrAnimation = Animations::Die;
		m_fFrameTime = 0;
		SetPathToGo(NULL);
		break;
	case States::Remove:
		break;
	case States::Win:
	case States::Defeat:
		SetPathToGo(NULL);
		break;
	default:
		assert(!"Error:: There is No State");
	}
}

void CRoider::PlayIdle(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);
	if (!m_activated) return;

	CCollisionObject* enemy{ m_pColManager->RequestNearObject(this, m_detectRange) };

	if (!enemy) return;
	if (!Chaseable(enemy)) return;

	SetEnemy(enemy);

	if (Attackable(enemy)) SetState(States::Attack);
	else SetState(States::Chase);
}

void CRoider::PlayWalk(float timeElapsed)
{
	if (NoneDestination(PathType::Sub))
	{
		if (MoveToDestination(timeElapsed) == States::Done) SetState(States::Idle);
	}
	else
	{
		MoveToSubDestination(timeElapsed);
	}
	PlayIdle(timeElapsed);
}

void CRoider::PlayChase(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	if (!Chaseable(m_pEnemy))
	{
		SetEnemy(NULL);
		GenerateSubPathToMainPath(pWayFinder);
		SetState(States::Walk);
	}
	else
	{
		MoveToSubDestination(timeElapsed, pWayFinder);
	}

	if (Attackable(m_pEnemy)) SetState(States::Attack);
}

void CRoider::PlayAttack(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	UNREFERENCED_PARAMETER(timeElapsed);

	if (!CheckEnemyState(m_pEnemy))
	{
		SetEnemy(NULL);
		GenerateSubPathToMainPath(pWayFinder);
		SetState(States::Walk);
	}
	else if (!Attackable(m_pEnemy))
	{
		SetNextState(States::Chase);
	}
}

void CRoider::PlayRemove(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);

	// 팀 타입을 중립 상태로 만든다.
	// 랜더링에서 제외한다.
	// 이 시기부터 시간을 재서 일정 시간이 지나면 다시 처음 위치에 리스폰한다.
	// Destination을 처음 위치 인근의 노드로 잡는다.
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CRoider::AdjustAnimationIndex()
{
	switch (m_nCurrAnimation)
	{
	case Animations::Idle:
		m_nAniIndex = 0;
		break;
	case Animations::Attack1:
		m_nAniIndex = 1;
		break;
	case Animations::Attack2:
		m_nAniIndex = 2;
		break;
	case Animations::StartWalk:
		m_nAniIndex = 3;
		break;
	case Animations::Walking:
		m_nAniIndex = 4;
		break;
	case Animations::Die:
		m_nAniIndex = 5;
		break;
	}
}

void CRoider::AnimateByCurState()
{
	switch (m_curState) {
	case States::Idle:
		if (m_nCurrAnimation != Animations::Idle) m_nCurrAnimation = Animations::Idle;
		break;
	case States::Attack:
		if (m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f
			&&m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) {
			m_pColManager->RequestCollide(CollisionType::SECTERFORM, this, CONVERT_PaperUnit_to_InG(8), 180, m_StatusInfo.Atk);
		}
		if (m_nCurrAnimation == Animations::Attack1) {
			if (m_curState == m_nextState)
			{
				if (m_fFrameTime < m_nAniLength[m_nAniIndex] / 2) break;
				m_nCurrAnimation = Animations::Attack2;
				m_fFrameTime = 0;
			}
			else
			{
				if (GetAnimTimeRemainRatio() > 0.05) break;
				SetState(m_nextState);
			}
		}
		else if (m_nCurrAnimation == Animations::Attack2)
		{
			if (m_curState == m_nextState)
			{
				if (m_fFrameTime < m_nAniLength[m_nAniIndex]) break;
				m_nCurrAnimation = Animations::Attack1;
				m_fFrameTime = 0;
			}
			else
			{
				if (GetAnimTimeRemainRatio() > 0.05) break;
				SetState(m_nextState);
			}
		}
		break;
	case States::Walk:
	case States::Chase:
		if (m_nCurrAnimation != Animations::StartWalk&&
			m_nCurrAnimation != Animations::Walking)
			m_nCurrAnimation = Animations::StartWalk;

		if (m_nCurrAnimation == Animations::StartWalk) {
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] - 1)
			{
				m_nCurrAnimation = Animations::Walking;
				m_fFrameTime = 0;
			}
		}
		break;
	case States::Die:
		if (m_nCurrAnimation != Animations::Die) m_nCurrAnimation = Animations::Die;
		if (GetAnimTimeRemainRatio() < 0.05)
		{
			if (m_TeamType == TeamType::Neutral)
			{
				ReadyToAtk();
			}
			else
			{
				m_curState = States::Remove;
			}
		}
		break;
	default:
		break;
	}
}

void CRoider::ReadyToAtk()
{
	// 상태를 Die에서 해당 팀으로 바꾸고 공격가도록 한다.

	// 방안1 현재 이 객체가 포커스 잡고 있는 Enemy의 팀 정보를 가져온다.
	// 방안2 특정 위치에 있는 오브젝트의 팀 정보를 가져온다.

	// Destination을 적 넥서스 위치 인근의 노드로 구해온다.
	// Main Path를 구성한다.

	// 상태를 Walk상태로 변경한다.
}
