#include "stdafx.h"
#include "Player.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"


CPlayer::CPlayer(CCreateMgr *pCreateMgr, int nMeshes)
	: CAnimatedObject(pCreateMgr, nMeshes)
{
}


CPlayer::~CPlayer()
{
}


void CPlayer::Animate(float timeElapsed)
{
	m_fPreFrameTime = m_fFrameTime;
	m_fFrameTime += 30 * timeElapsed;


	if (m_fFrameTime > m_nAniLength[m_nCurrAnimation]) {
		while (m_fFrameTime > m_nAniLength[m_nCurrAnimation])
			m_fFrameTime -= m_nAniLength[m_nCurrAnimation];
	}
	MoveToDestination(timeElapsed);
	if (m_pathToGo&&m_CurrState != States::Walk) {
		m_state = States::Walk;
		m_CurrState = States::Walk;
		m_fFrameTime = 0;
	}
	else if (!m_pathToGo&&m_CurrState == States::Walk) {
		m_state = States::Idle;
		m_CurrState = States::Idle;
		m_fFrameTime = 0;
	}

	switch (m_CurrState) {
	case States::Idle:
		if (m_nCurrAnimation != PlayerAnimation::Idle)
			m_nCurrAnimation = PlayerAnimation::Idle;
		break;
	case States::Attack:
		if (m_fFrameTime >= m_nAniLength[m_nCurrAnimation] - 1)
		{
			m_CurrState = States::Idle;
			m_nCurrAnimation = PlayerAnimation::Idle;
			m_fFrameTime = 0;
		}
		else if (m_nCurrAnimation == PlayerAnimation::SkillQ) {
			if (m_fFrameTime >= m_nAniLength[m_nCurrAnimation] * 0.5f
				&&m_fPreFrameTime < m_nAniLength[m_nCurrAnimation] * 0.5f) {
				m_pColManager->RequestCollide(CollisionType::SPHERE, this, 24, 16);
			}
		}
		else if (m_nCurrAnimation == PlayerAnimation::SkillE) {
			if (m_fFrameTime >= m_nAniLength[m_nCurrAnimation] * 0.5f
				&&m_fPreFrameTime < m_nAniLength[m_nCurrAnimation] * 0.5f) {
				m_pColManager->RequestCollide(CollisionType::SECTERFORM, this, 24, 180);
			}
		}
		else if (m_nCurrAnimation == PlayerAnimation::SkillR) {
			if (m_fFrameTime >= m_nAniLength[m_nCurrAnimation] * 0.666f
				&&m_fPreFrameTime < m_nAniLength[m_nCurrAnimation] * 0.666f) {
				m_pColManager->RequestCollide(CollisionType::SPHERE, this, 0, 32);
			}
		}

		break;

	case States::Walk:
		if (m_nCurrAnimation != PlayerAnimation::StartWalk&&
			m_nCurrAnimation != PlayerAnimation::Walking)
			m_nCurrAnimation = PlayerAnimation::StartWalk;

		if (m_nCurrAnimation == PlayerAnimation::StartWalk) {
			if (m_fFrameTime >= m_nAniLength[m_nCurrAnimation] - 1)
			{
				m_nCurrAnimation = PlayerAnimation::Walking;
				m_fFrameTime = 0;
			}
		}
		break;
	case States::Die:

		break;
	default:

		break;
	}


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

void CPlayer::SetPathToGo(Path * path)
{
	if (m_pathToGo)
	{
		m_pathToGo->clear();
		Safe_Delete(m_pathToGo);
	}
	m_pathToGo = path;
	m_destination.x = -1;
}

void CPlayer::SetPosition(float x, float z)
{
	CBaseObject::SetPosition(x, m_pTerrain->GetHeight(x, z), z);
}

void CPlayer::ActiveSkill(PlayerAnimation act)
{
	if (m_CurrState != States::Attack) {
		if (m_pathToGo) {
			SetPathToGo(NULL);
		}
		m_CurrState = States::Attack;
		m_nCurrAnimation = act;
		m_fFrameTime = 0;
	}
}
