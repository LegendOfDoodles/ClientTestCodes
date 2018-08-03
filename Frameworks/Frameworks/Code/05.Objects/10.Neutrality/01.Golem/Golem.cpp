#include "stdafx.h"
#include "Golem.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

/// <summary>
/// 목적: 중립 몬스터(수호 골렘) 클래스 분할
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-03
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CGolem::CGolem(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CAnimatedObject(pCreateMgr, nMeshes)
{
	m_ObjectType = ObjectType::GOLEM;

	m_sightRange = CONVERT_PaperUnit_to_InG(100.0f);
	m_detectRange = CONVERT_PaperUnit_to_InG(80.0f);
	m_speed = CONVERT_cm_to_InG(7.682f);

	// Warning! 골렘 스테이터스 설정 필요
	m_StatusInfo.HP = m_StatusInfo.maxHP = 1200.0f;
	m_StatusInfo.Def = 20.0f;
	m_StatusInfo.Atk = 200.0f;
	m_StatusInfo.Exp = 500;

	m_attackRange = CONVERT_PaperUnit_to_InG(30);

	BuildSelf(pCreateMgr);
}

CGolem::~CGolem()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CGolem::Animate(float timeElapsed)
{
	AdjustAnimationIndex();
	AnimateByCurState();

	if (m_curState != States::Remove)
	{
		m_fPreFrameTime = m_fFrameTime;
		m_fFrameTime += 30 * timeElapsed;
	}

	if (m_fFrameTime > m_nAniLength[m_nAniIndex]) {
		while (m_fFrameTime > m_nAniLength[m_nAniIndex])
			m_fFrameTime -= m_nAniLength[m_nAniIndex];
	}

	CAnimatedObject::Animate(timeElapsed);
}

void CGolem::Render(CCamera * pCamera, UINT instanceCnt)
{
	OnPrepareRender();

	if (!IsVisible(pCamera)||!m_Detected) return;

	if (m_pMaterial)
	{
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

void CGolem::SetState(StatesType newState)
{
	m_nextState = m_curState = newState;

	switch (newState)
	{
	case States::Idle:
		m_nCurrAnimation = Animations::Idle;
		break;
	case States::Walk:
		RegenerateLookAt();
		m_availableTime = 0.0f;
		m_nCurrAnimation = Animations::StartWalk;
		break;
	case States::Chase:
		m_availableTime = 0.0f;
		m_nCurrAnimation = Animations::StartWalk;
		break;
	case States::Attack:
		SetAnimation(Animations::Attack1);
		m_fFrameTime = 0;
		break;
	case States::Die:
		m_nCurrAnimation = Animations::Die;
		m_fFrameTime = 0;
		SetPathToGo(NULL);
		break;
	case States::Remove:
		m_spawnCoolTime = COOLTIME_SPAWN_GOLEM;
		break;
	case States::Win:
	case States::Defeat:
		SetPathToGo(NULL);
		break;
	default:
		assert(!"Error:: There is No State");
	}
}

void CGolem::PlayIdle(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);

	if (m_activated && m_TeamType == TeamType::Neutral)
	{
		m_deactiveTime += timeElapsed;
		if (m_deactiveTime > TIME_ACTIVATE_CHECK)
		{
			m_activated = false;
			// Warning! 회복 처리
			// 방안 1: 전체 회복
			// 방안 2: 일정 시간동안 몇 %의 체력 회복
		}
	}

	if (!m_activated) return;

	CCollisionObject* enemy{ m_pColManager->RequestNearObject(this, m_detectRange) };

	if (!enemy) return;
	if (!Chaseable(enemy)) return;

	SetEnemy(enemy);

	if (Attackable(enemy))
	{
		SetState(States::Attack);
	}
	else if (AttackableFarRange(enemy)) {
		SetState(States::Attack);
		SetAnimation(Animations::Attack2);
	}
	else SetState(States::Chase);
}

void CGolem::PlayWalk(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	if (NoneDestination(PathType::Sub))
	{
		if (MoveToDestination(timeElapsed, pWayFinder) == States::Done) SetState(States::Idle);
	}
	else
	{
		MoveToSubDestination(timeElapsed);
		// 중립인데 원래 위치로 돌아갔으면 상태를 아이들 상태로 전환한다.
		if (m_TeamType == TeamType::Neutral && NoneDestination(PathType::Sub))
		{
			SetState(States::Idle);
		}
	}
	PlayIdle(timeElapsed);
}

void CGolem::PlayChase(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	if (!Chaseable(m_pEnemy))
	{
		SetEnemy(NULL);
		if (m_TeamType == TeamType::Neutral)
			GenerateSubPathToSpawnLocation(pWayFinder);
		else
			GenerateSubPathToMainPath(pWayFinder);
		SetState(States::Walk);
	}
	else
	{
		MoveToSubDestination(timeElapsed, pWayFinder);
	}

	if (Attackable(m_pEnemy))
	{
		SetState(States::Attack);
	}
}

void CGolem::PlayAttack(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	UNREFERENCED_PARAMETER(timeElapsed);

	if (!CheckEnemyState(m_pEnemy))
	{
		SetEnemy(NULL);
		GenerateSubPathToMainPath(pWayFinder);
		SetState(States::Walk);
	}
	else if (Attackable(m_pEnemy))
	{
		if (m_nCurrAnimation != Animations::Attack1)
			m_nNextAnimation = Animations::Attack1;
	}
	else
	{
		SetNextState(States::Chase);
	}
}

void CGolem::PlayRemove(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	m_spawnCoolTime -= timeElapsed;

	if (m_TeamType == TeamType::Neutral)
	{
		ReadyToAtk(pWayFinder);
	}
	else if (m_spawnCoolTime < 0.0f)
	{
		Respawn();
	}
}

void CGolem::SaveCurrentState()
{
	m_xmf4x4SpawnWorld = m_xmf4x4World;
	m_spawnLocation = GetPosition();
}

void CGolem::BuildSelf(shared_ptr<CCreateMgr> pCreateMgr)
{
	CSkinnedMesh *pRoiderMesh = new CSkinnedMesh(pCreateMgr, "Resource//3D//Golem//Mesh//GuardGolem.meshinfo");

	CCubeMesh *pBoundingBoxMesh = new CCubeMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(28.0f), CONVERT_PaperUnit_to_InG(7.0f), CONVERT_PaperUnit_to_InG(20.0f),
		0, 0, -CONVERT_PaperUnit_to_InG(11.0f));

	//CSkeleton *pSIdle = new CSkeleton("Resource//3D//Monster//Animation//Royde_Idle.aniinfo");
	//CSkeleton *pSStartWalk = new CSkeleton("Resource//3D//Monster//Animation//Royde_Start_Walk.aniinfo");
	//CSkeleton *pSWalk = new CSkeleton("Resource//3D//Monster//Animation//Royde_Walk.aniinfo");
	//CSkeleton *pSSmash = new CSkeleton("Resource//3D//Monster//Animation//Royde_Attack1.aniinfo");
	//CSkeleton *pSThrow = new CSkeleton("Resource//3D//Monster//Animation//Royde_Attack2.aniinfo");
	//CSkeleton *pSDie = new CSkeleton("Resource//3D//Monster//Animation//Royde_Die.aniinfo");

	pRoiderMesh->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(11.0f)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(14.0f), CONVERT_PaperUnit_to_InG(7.0f), CONVERT_PaperUnit_to_InG(10.0f)));

	SetBoundingMesh(pBoundingBoxMesh);
	SetCollisionSize(CONVERT_PaperUnit_to_InG(14));

	//SetSkeleton(pSIdle);
	//SetSkeleton(pSStartWalk);
	//SetSkeleton(pSWalk);

	//SetSkeleton(pSSmash);
	//SetSkeleton(pSThrow);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CGolem::AdjustAnimationIndex()
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

void CGolem::AnimateByCurState()
{
	switch (m_curState) {
	case States::Idle:
		if (m_nCurrAnimation != Animations::Idle) m_nCurrAnimation = Animations::Idle;
		break;
	case States::Attack:
		if (GetAnimTimeRemainRatio() <= 0.05f)
		{
			LookAt(m_pEnemy->GetPosition());
		}
		if (m_nCurrAnimation == Animations::Attack1)
		{
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f
				&&m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) {
				m_pColManager->RequestCollide(CollisionType::SECTERFORM, this, m_attackRange, 180, m_StatusInfo.Atk);
			}
			if (m_curState != m_nextState)
			{
				if (GetAnimTimeRemainRatio() > 0.05f) break;
				SetState(m_nextState);
			}
			else if (m_nCurrAnimation != m_nNextAnimation)
			{
				if (GetAnimTimeRemainRatio() > 0.05f) break;
				m_nCurrAnimation = m_nNextAnimation;
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
			SetState(States::Remove);
		}
		break;
	default:
		break;
	}
}

void CGolem::ReadyToAtk(shared_ptr<CWayFinder> pWayFinder)
{
	if (m_lastDamageTeam == TeamType::Red)
		SetTeam(TeamType::Red);
	else if (m_lastDamageTeam == TeamType::Blue)
		SetTeam(TeamType::Blue);

	m_StatusInfo.HP = m_StatusInfo.maxHP;

	m_pColManager->AddCollider(this);

	CPathEdge pathBeg{ m_pathes[WayType::Golem_Exit].front() };
	XMFLOAT3 curPos{ GetPosition() };

	int wayKind = rand() % 2; // 0: 아래, 1: 위
	if (wayKind == 0)
	{
		pathBeg = m_pathes[WayType::Golem_Exit].front();
	}
	else
	{
		pathBeg = m_pathes[WayType::Golem_Exit].back();
	}

	Path *newPath{ NULL };
	if (m_TeamType == TeamType::Blue)
	{
		// 위로 가야함
		if (wayKind == 1)
		{
			newPath = new Path(m_pathes[WayType::Blue_Up]);
		}
		else
		{
			newPath = new Path(m_pathes[WayType::Blue_Down]);
		}
		newPath->remove_if([pathBeg](CPathEdge& edge) {
			return edge.To().x < pathBeg.To().x;
		});
	}
	else if (m_TeamType == TeamType::Red)
	{
		// 위로 가야함
		if (wayKind == 1)
		{
			newPath = new Path(m_pathes[WayType::Red_Up]);
		}
		else
		{
			newPath = new Path(m_pathes[WayType::Red_Down]);
		}
		newPath->remove_if([pathBeg](CPathEdge& edge) {
			return edge.To().x > pathBeg.To().x;
		});
	}

	SetPathToGo(newPath);
	GenerateSubPathToPosition(pWayFinder, XMFLOAT3(pathBeg.To().x, curPos.y, pathBeg.To().y));

	SetState(StatesType::Walk);
}

void CGolem::Respawn()
{
	m_activated = false;

	SetState(StatesType::Idle);
	SetTeam(TeamType::Neutral);

	m_StatusInfo.HP = m_StatusInfo.maxHP;

	m_pColManager->AddCollider(this);

	m_xmf4x4World = m_xmf4x4SpawnWorld;
}

void CGolem::GenerateSubPathToSpawnLocation(shared_ptr<CWayFinder> pWayFinder)
{
	ResetSubPath();
	m_subPath = pWayFinder->GetPathToPosition(
		GetPosition(), 
		m_spawnLocation);
	m_subDestination = m_subPath->front().To();
	m_subPath->pop_front();
	LookAt(m_subDestination);
}