#include "stdafx.h"
#include "Roider.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

/// <summary>
/// 목적: 중립 몬스터(로이더) 클래스 분할
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-07-29
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CRoider::CRoider(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CAnimatedObject(pCreateMgr, nMeshes)
{
	m_ObjectType = ObjectType::Roider;

	m_sightRange = CONVERT_PaperUnit_to_InG(80.0f);
	m_detectRange = CONVERT_PaperUnit_to_InG(40.0f);
	m_speed = CONVERT_cm_to_InG(3.237f);

	// Warning! 로이더 스테이터스 설정 필요
	m_StatusInfo.HP = m_StatusInfo.maxHP = 100.0f;
	m_StatusInfo.Def = 10.0f;
	m_StatusInfo.Atk = 200.0f;
	m_StatusInfo.Exp = 100;

	m_attackRange = CONVERT_PaperUnit_to_InG(16);
	m_farAttackRange = CONVERT_PaperUnit_to_InG(30);
}

CRoider::~CRoider()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CRoider::Animate(float timeElapsed)
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

void CRoider::Render(CCamera * pCamera, UINT instanceCnt)
{
	OnPrepareRender();

	if (!IsVisible(pCamera)) return;

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
		SetAnimation(Animations::Attack1);
		m_fFrameTime = 0;
		break;
	case States::Die:
		m_nCurrAnimation = Animations::Die;
		m_fFrameTime = 0;
		SetPathToGo(NULL);
		break;
	case States::Remove:
		m_spawnCoolTime = COOLTIME_SPAWN_ROIDER;
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

void CRoider::PlayWalk(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	if (NoneDestination(PathType::Sub))
	{
		if(MoveToDestination(timeElapsed, pWayFinder) == States::Done) SetState(States::Idle);
	}
	else
	{
		MoveToSubDestination(timeElapsed);
		// 중립인데 원래 위치로 돌아갔으면 상태를 아이들 상태로 전환한다.
		if(m_TeamType == TeamType::Neutral && NoneDestination(PathType::Sub))
		{ 
			SetState(States::Idle);
		}
	}
	PlayIdle(timeElapsed);
}

void CRoider::PlayChase(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
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
	else if (AttackableFarRange(m_pEnemy)) {
		SetState(States::Attack);
		SetAnimation(Animations::Attack2);
	}
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
	else if (Attackable(m_pEnemy))
	{
		if (m_nCurrAnimation != Animations::Attack1)
			m_nNextAnimation = Animations::Attack1;
	}
	else if (AttackableFarRange(m_pEnemy))
	{
		if (m_nCurrAnimation != Animations::Attack2)
			m_nNextAnimation = Animations::Attack2;
	}
	else
	{
		SetNextState(States::Chase);
	}
}

void CRoider::PlayRemove(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	m_spawnCoolTime -= timeElapsed;

	if (m_TeamType == TeamType::Neutral)
	{
		ReadyToAtk(pWayFinder);
	}
	else if(m_spawnCoolTime < 0.0f)
	{
		Respawn();
	}
}

void CRoider::LookAt(XMFLOAT3 objPosition)
{
	if (m_curState == States::Win) return;
	if (m_curState == States::Defeat) return;

	XMFLOAT3 upVector{ 0.f, 1.f, 0.f };
	XMFLOAT3 playerLook = GetLook();
	XMFLOAT3 playerPos = GetPosition();

	objPosition.y = playerPos.y;

	XMFLOAT3 towardVector = Vector3::Subtract(objPosition, GetPosition(), true);

	float angle{ Vector3::DotProduct(towardVector, playerLook) };
	angle = XMConvertToDegrees(acos(angle));

	if (isnan(angle)) return;

	float check{ Vector3::DotProduct(Vector3::CrossProduct(towardVector, playerLook), upVector) };

	// 캐릭터가 선택된 오브젝트 보다 오른쪽 보고 있는 경우
	if (check < 0.0f)
		Rotate(0.0f, 0.0f, -angle);
	else if (check > 0.0f)
		Rotate(0.0f, 0.0f, angle);
}

void CRoider::LookAt(XMFLOAT2 objPosition)
{
	LookAt(XMFLOAT3(objPosition.x, 0, objPosition.y));
}

void CRoider::SaveCurrentState()
{
	m_xmf4x4SpawnWorld = m_xmf4x4World;
	m_spawnLocation = GetPosition();
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
		m_nAniIndex = 3;
		break;
	case Animations::Attack2:
		m_nAniIndex = 4;
		break;
	case Animations::StartWalk:
		m_nAniIndex = 1;
		break;
	case Animations::Walking:
		m_nAniIndex = 2;
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
		if (GetAnimTimeRemainRatio() <= 0.05f)
		{
			LookAt(m_pEnemy->GetPosition());
		}
		if (m_nCurrAnimation == Animations::Attack1) 
		{
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f
				&&m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) {
				m_pColManager->RequestCollide(CollisionType::SECTERFORM, this, CONVERT_PaperUnit_to_InG(m_attackRange), 180, m_StatusInfo.Atk);
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
		else if (m_nCurrAnimation == Animations::Attack2)
		{
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f
				&&m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) {
				m_pThrowingMgr->RequestSpawn(GetPosition(), m_fCollisionSize, GetLook(), m_TeamType, FlyingObjectType::Roider_Dumbel);
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

void CRoider::ReadyToAtk(shared_ptr<CWayFinder> pWayFinder)
{
	if(m_lastDamageTeam == TeamType::Red)
		SetTeam(TeamType::Red);
	else if (m_lastDamageTeam == TeamType::Blue)
		SetTeam(TeamType::Blue);

	m_StatusInfo.HP = m_StatusInfo.maxHP;

	m_pColManager->AddCollider(this);

	if (m_TeamType == TeamType::Blue)
	{
		SetPathToGo(pWayFinder->GetPathToPosition(
			GetPosition(),
			m_redNexusLoc,
			GetCollisionSize()));
	}
	else if (m_TeamType == TeamType::Red)
	{
		SetPathToGo(pWayFinder->GetPathToPosition(
			GetPosition(),
			m_blueNexusLoc,
			GetCollisionSize()));
	}

	ResetSubPath();
	SetState(StatesType::Walk);
}

void CRoider::Respawn()
{
	m_activated = false;

	SetState(StatesType::Idle);
	SetTeam(TeamType::Neutral);

	m_StatusInfo.HP = m_StatusInfo.maxHP;

	m_pColManager->AddCollider(this);

	m_xmf4x4World = m_xmf4x4SpawnWorld;
}

void CRoider::GenerateSubPathToSpawnLocation(shared_ptr<CWayFinder> pWayFinder)
{
	ResetSubPath();
	m_subPath = pWayFinder->GetPathToPosition(GetPosition(), m_spawnLocation, GetCollisionSize());
	m_subDestination = m_subPath->front().To();
	m_subPath->pop_front();
	LookAt(m_subDestination);
}
