#include "stdafx.h"
#include "Player.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"

/// <summary>
/// 목적: 플레이어 관리 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-05-11
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CPlayer::CPlayer(CCreateMgr *pCreateMgr, int nMeshes) : CAnimatedObject(pCreateMgr, nMeshes)
{
}


CPlayer::~CPlayer()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CPlayer::Animate(float timeElapsed)
{
	switch (m_curState) {
	case States::Idle:
		if (m_nCurrAnimation != Animations::Idle) m_nCurrAnimation = Animations::Idle;
		if (m_pathToGo) SetState(States::Walk);
		break;
	case States::Attack:
		if (m_fFrameTime >= m_nAniLength[m_nAniIndex] - 1)
		{
			SetState(States::Idle);
		}
		else if (m_nCurrAnimation == Animations::SkillQ) {
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f
				&&m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) {
				m_pColManager->RequestCollide(CollisionType::SPHERE, this, CONVERT_PaperUnit_to_InG(4), CONVERT_PaperUnit_to_InG(4),100);
			}
		}
		else if (m_nCurrAnimation == Animations::SkillE) {
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f
				&&m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) {
				m_pColManager->RequestCollide(CollisionType::SECTERFORM, this, 32, 180);
			}
		}
		else if (m_nCurrAnimation == Animations::SkillR) {
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.666f
				&&m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.666f) {
				m_pColManager->RequestCollide(CollisionType::SPHERE, this, 0, CONVERT_PaperUnit_to_InG(1));
			}
		}
		break;

	case States::Walk:
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
			m_curState = States::Remove;
		}
		break;
	default:
		break;
	}

	m_fPreFrameTime = m_fFrameTime;
	m_fFrameTime += 30 * timeElapsed;

	AdjustAnimationIndex();

	if (m_fFrameTime > m_nAniLength[m_nAniIndex]) {
		while (m_fFrameTime > m_nAniLength[m_nAniIndex])
			m_fFrameTime -= m_nAniLength[m_nAniIndex];
	}

	if (MoveToDestination(timeElapsed) == States::Done) SetState(States::Idle);

	CAnimatedObject::Animate(timeElapsed);
}

void CPlayer::Render(CCamera * pCamera, UINT instanceCnt)
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

void CPlayer::ActiveSkill(AnimationsType act)
{
	if (m_curState != States::Attack) {
		m_curState = States::Attack;
		m_nCurrAnimation = act;
		m_fFrameTime = 0;
	}
}

void CPlayer::SetState(StatesType newState)
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
		m_fFrameTime = 0;
		break;
	case States::Die:
		m_nCurrAnimation = Animations::Die;
		m_fFrameTime = 0;
		SetPathToGo(NULL);
		break;
	case States::Remove:
		break;
	default:
		assert(!"Error:: There is No State");
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CPlayer::AdjustAnimationIndex()
{
	switch (m_nCurrAnimation)
	{
	case Animations::Idle:
		m_nAniIndex = 0;
		break;
	case Animations::StartWalk:
		m_nAniIndex = 1;
		break;
	case Animations::Walking:
		m_nAniIndex = 2;
		break;
	case Animations::SkillQ:
		m_nAniIndex = 3;
		break;
	case Animations::SkillE:
		m_nAniIndex = 4;
		break;
	case Animations::SkillR:
		m_nAniIndex = 5;
		break;
	}
}
