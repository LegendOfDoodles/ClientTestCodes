#include "stdafx.h"
#include "Minion.h"

/// <summary>
/// 목적: 미니언 클래스 분할
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-05-11
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CMinion::CMinion(CCreateMgr * pCreateMgr, int nMeshes) : CAnimatedObject(pCreateMgr, nMeshes)
{
}

CMinion::~CMinion()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CMinion::Animate(float timeElapsed)
{
	m_fFrameTime += 30 * timeElapsed;


	if (m_fFrameTime > m_nAniLength[m_nAniIndex]) {
		while (m_fFrameTime > m_nAniLength[m_nAniIndex])
			m_fFrameTime -= m_nAniLength[m_nAniIndex];
	}
	MoveToDestination(timeElapsed);
	if (m_pathToGo&&m_curState != States::Walk) {
		m_nextState = States::Walk;
		m_curState = States::Walk;
		m_fFrameTime = 0;
	}
	else if (!m_pathToGo&&m_curState == States::Walk) {
		m_nextState = States::Idle;
		m_curState = States::Idle;
		m_fFrameTime = 0;
	}
	AdjustAnimationIndex();
	CAnimatedObject::Animate(timeElapsed);

}

void CMinion::Render(CCamera * pCamera, UINT instanceCnt)
{
	OnPrepareRender();

	if (m_pMaterial)
	{
		m_pMaterial->Render(pCamera);
		m_pMaterial->UpdateShaderVariables();
	}

	if (m_cbvGPUDescriptorHandle.ptr)
		m_pCommandList->SetGraphicsRootDescriptorTable(9, m_cbvGPUDescriptorHandle);

	if (m_pShader)
	{
		UpdateShaderVariables();
		m_pShader->Render(pCamera);
	}

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(instanceCnt);
		}
	}
}

void CMinion::SetPathToGo(Path * path)
{
	if (m_pathToGo)
	{
		m_pathToGo->clear();
		Safe_Delete(m_pathToGo);
	}
	m_pathToGo = path;
	m_destination.x = -1;
}

void CMinion::SetPosition(float x, float z)
{
	CBaseObject::SetPosition(x, m_pTerrain->GetHeight(x, z), z);
}

void CMinion::SetState(StatesType newState)
{
	m_curState = newState;

	switch (newState)
	{
	case States::Idle:
		m_nCurrAnimation = Animations::Idle;
		break;
	case States::Walk:
		m_nCurrAnimation = Animations::StartWalk;
		break;
	case States::Chase:
		m_nCurrAnimation = Animations::StartWalk;
		break;
	case States::Attack:
		m_nCurrAnimation = Animations::Attack1;
		m_fFrameTime = 0;
		break;
	case States::Die:
		m_nCurrAnimation = Animations::Die;
		m_fFrameTime = 0;
		break;
	case States::Remove:
		break;
	default:
		assert(!"Error:: There is No State");
	}
}

void CMinion::AdjustAnimationIndex()
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

////////////////////////////////////////////////////////////////////////
// 내부 함수

//////////////////////////////////////////////////////////////////////////
//근접 미니언
CSwordMinion::CSwordMinion(CCreateMgr * pCreateMgr, int nMeshes): CMinion(pCreateMgr,nMeshes)
{
	SetType(ObjectType::SwordMinion);
	m_StatusInfo.maxHP = 445;
	m_StatusInfo.HP = 445;
	m_StatusInfo.Atk = 12;
	m_StatusInfo.Def = 0;
	m_StatusInfo.Exp = 64;
}

CSwordMinion::~CSwordMinion()
{
}

void CSwordMinion::Animate(float timeElapsed)
{
		switch (m_curState) {
		case States::Idle:
			if (m_nCurrAnimation != Animations::Idle)
				m_nCurrAnimation = Animations::Idle;
			break;
		case States::Attack:
			if (m_nCurrAnimation == Animations::Attack1) {
				if (m_fFrameTime > m_nAniLength[m_nAniIndex] / 2 &&
					m_curState == m_nextState)
				{
					m_nCurrAnimation = Animations::Attack2;
					m_fFrameTime = 0;
				}
			}
			else if (m_nCurrAnimation == Animations::Attack2)
			{
				if (m_fFrameTime > m_nAniLength[m_nAniIndex])
				{
					m_nCurrAnimation = Animations::Attack1;
					m_fFrameTime = 0;

				}
			}
			break;

		case States::Walk:
			if(m_nCurrAnimation!= Animations::StartWalk&&
				m_nCurrAnimation != Animations::Walking)
				m_nCurrAnimation = Animations::StartWalk;

			if (m_nCurrAnimation == Animations::StartWalk) {
				if (m_fFrameTime >= m_nAniLength[m_nAniIndex]-1)
				{
					m_nCurrAnimation = Animations::Walking;
					m_fFrameTime = 0;
				}
			}
			break;
		case States::Die:
			m_nCurrAnimation = Animations::Die;
			m_fFrameTime = 0;
			break;
		default:

			break;
		}
	CMinion::Animate(timeElapsed);
}

//////////////////////////////////////////////////////////////////////////
//마법 미니언

CMagicMinion::CMagicMinion(CCreateMgr * pCreateMgr, int nMeshes) : CMinion(pCreateMgr, nMeshes)
{
	SetType(ObjectType::StaffMinion);
	m_StatusInfo.maxHP = 280;
	m_StatusInfo.HP = 280;
	m_StatusInfo.Atk = 23;
	m_StatusInfo.Def = 0;
	m_StatusInfo.Exp = 64;
}

CMagicMinion::~CMagicMinion()
{
}

void CMagicMinion::Animate(float timeElapsed)
{
		switch (m_curState) {
		case States::Idle:
			if (m_nCurrAnimation != Animations::Idle)
				m_nCurrAnimation = Animations::Idle;
			break;
		case States::Attack:
			if (m_nCurrAnimation == Animations::Attack1) {
				if (m_fFrameTime > m_nAniLength[m_nAniIndex] / 2 &&
					m_curState == m_nextState)
				{
					m_nCurrAnimation = Animations::Attack2;
					m_fFrameTime = 0;
				}
			}
			else if (m_nCurrAnimation == Animations::Attack2)
			{
				if (m_fFrameTime > m_nAniLength[m_nAniIndex])
				{
					m_nCurrAnimation = Animations::Attack1;
					m_fFrameTime = 0;

				}
			}
			break;

		case States::Walk:
			if (m_nCurrAnimation != Animations::StartWalk &&
				m_nCurrAnimation != Animations::Walking)
				m_nCurrAnimation = Animations::StartWalk;
			if (m_nCurrAnimation == Animations::StartWalk) {
				if (m_fFrameTime > m_nAniLength[m_nAniIndex]-1)
				{
					m_nCurrAnimation = Animations::Walking;
					m_fFrameTime = 0;
				}
			}
			break;
		case States::Die:
			m_nCurrAnimation = Animations::Die;
			m_fFrameTime = 0;
			break;
		default:

			break;
		}
	CMinion::Animate(timeElapsed);

}

//////////////////////////////////////////////////////////////////////////
//활 미니언

CBowMinion::CBowMinion(CCreateMgr * pCreateMgr, int nMeshes) : CMinion(pCreateMgr, nMeshes)
{
	SetType(ObjectType::BowMinion);
	m_StatusInfo.maxHP = 300;
	m_StatusInfo.HP = 300;
	m_StatusInfo.Atk = 20;
	m_StatusInfo.Def = 0;
	m_StatusInfo.Exp = 64;
}

CBowMinion::~CBowMinion()
{
}

void CBowMinion::Animate(float timeElapsed)
{
		switch (m_curState) {
		case States::Idle:

			if (m_nCurrAnimation != Animations::Idle)
				m_nCurrAnimation = Animations::Idle;
			break;
		case States::Attack:
			
			break;

		case States::Walk:
			if (m_nCurrAnimation != Animations::StartWalk &&
				m_nCurrAnimation != Animations::Walking)
				m_nCurrAnimation = Animations::StartWalk;
			if (m_nCurrAnimation == Animations::StartWalk) {
				if (m_fFrameTime > m_nAniLength[m_nAniIndex]-1)
				{
					m_nCurrAnimation = Animations::Walking;
					m_fFrameTime = 0;
				}
			}
			break;
		case States::Die:
			m_nCurrAnimation = Animations::Die;
			m_fFrameTime = 0;
			break;
		default:

			break;
		}
	CMinion::Animate(timeElapsed);

}

void CBowMinion::AdjustAnimationIndex()
{
	switch (m_nCurrAnimation)
	{
	case Animations::Idle:
		m_nAniIndex = 0;
		break;
	case Animations::Attack1:
		m_nAniIndex = 1;
		break;
	case Animations::StartWalk:
		m_nAniIndex = 2;
		break;
	case Animations::Walking:
		m_nAniIndex = 3;
		break;
	case Animations::Die:
		m_nAniIndex = 4;

	}
}
