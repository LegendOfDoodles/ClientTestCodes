#include "stdafx.h"
#include "Roider.h"

/// <summary>
/// ����: �߸� ����(���̴�) Ŭ���� ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-07-20
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
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
// ���� �Լ�
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

	if(MoveToDestination(timeElapsed) == States::Done) SetState(States::Idle);

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

	// �� Ÿ���� �߸� ���·� �����.
	// ���������� �����Ѵ�.
	// �� �ñ���� �ð��� �缭 ���� �ð��� ������ �ٽ� ó�� ��ġ�� �������Ѵ�.
	// Destination�� ó�� ��ġ �α��� ���� ��´�.
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
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
	// ���¸� Die���� �ش� ������ �ٲٰ� ���ݰ����� �Ѵ�.

	// ���1 ���� �� ��ü�� ��Ŀ�� ��� �ִ� Enemy�� �� ������ �����´�.
	// ���2 Ư�� ��ġ�� �ִ� ������Ʈ�� �� ������ �����´�.

	// Destination�� �� �ؼ��� ��ġ �α��� ���� ���ؿ´�.
	// Main Path�� �����Ѵ�.

	// ���¸� Walk���·� �����Ѵ�.
}