#include "stdafx.h"
#include "Minion.h"

/// <summary>
/// 목적: 미니언 클래스 분할
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-05-04
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


	if (m_fFrameTime > m_nAniLength[m_nCurrAnimation]) {
		while (m_fFrameTime > m_nAniLength[m_nCurrAnimation])
			m_fFrameTime -= m_nAniLength[m_nCurrAnimation];
	}
	MoveToDestination(timeElapsed);
	if (m_pathToGo&&m_CurrState != MinionState::Walking) {
		m_NextState = MinionState::Walking;
		m_CurrState = MinionState::Walking;
		m_fFrameTime = 0;
	}
	else if (!m_pathToGo&&m_CurrState == MinionState::Walking) {
		m_NextState = MinionState::Idle;
		m_CurrState = MinionState::Idle;
		m_fFrameTime = 0;
	}

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

////////////////////////////////////////////////////////////////////////
// 내부 함수

//////////////////////////////////////////////////////////////////////////
//근접 미니언
CSwordMinion::CSwordMinion(CCreateMgr * pCreateMgr) : CMinion(pCreateMgr)
{
}

CSwordMinion::CSwordMinion(CCreateMgr * pCreateMgr, int nMeshes): CMinion(pCreateMgr,nMeshes)
{
}

CSwordMinion::~CSwordMinion()
{
}

void CSwordMinion::Animate(float timeElapsed)
{
		switch (m_CurrState) {
		case MinionState::Idle:
			if (m_nCurrAnimation != SwordMinionAnimation::Idle)
				m_nCurrAnimation = SwordMinionAnimation::Idle;
			break;
		case MinionState::Attack:
			if (m_nCurrAnimation == SwordMinionAnimation::Attack1) {
				if (m_fFrameTime > m_nAniLength[m_nCurrAnimation] / 2 &&
					m_CurrState == m_NextState)
				{
					m_nCurrAnimation = SwordMinionAnimation::Attack2;
					m_fFrameTime = 0;
				}
			}
			else if (m_nCurrAnimation == SwordMinionAnimation::Attack2)
			{
				if (m_fFrameTime > m_nAniLength[m_nCurrAnimation])
				{
					m_nCurrAnimation = SwordMinionAnimation::Attack1;
					m_fFrameTime = 0;

				}
			}
			break;

		case MinionState::Walking:
			if(m_nCurrAnimation!= SwordMinionAnimation::StartWalk&&
				m_nCurrAnimation != SwordMinionAnimation::Walking)
				m_nCurrAnimation = SwordMinionAnimation::StartWalk;

			if (m_nCurrAnimation == SwordMinionAnimation::StartWalk) {
				if (m_fFrameTime >= m_nAniLength[m_nCurrAnimation]-1)
				{
					m_nCurrAnimation = SwordMinionAnimation::Walking;
					m_fFrameTime = 0;
				}
			}
			break;
		case MinionState::Die:

			break;
		default:

			break;
		}
	CMinion::Animate(timeElapsed);
}

//////////////////////////////////////////////////////////////////////////
//마법 미니언
CMagicMinion::CMagicMinion(CCreateMgr * pCreateMgr) : CMinion(pCreateMgr)
{
}

CMagicMinion::CMagicMinion(CCreateMgr * pCreateMgr, int nMeshes) : CMinion(pCreateMgr, nMeshes)
{
}

CMagicMinion::~CMagicMinion()
{
}

void CMagicMinion::Animate(float timeElapsed)
{
		switch (m_CurrState) {
		case MinionState::Idle:
			if (m_nCurrAnimation != MagicMinionAnimation::Idle)
				m_nCurrAnimation = MagicMinionAnimation::Idle;
			break;
		case MinionState::Attack:
			if (m_nCurrAnimation == MagicMinionAnimation::Attack1) {
				if (m_fFrameTime > m_nAniLength[m_nCurrAnimation] / 2 &&
					m_CurrState == m_NextState)
				{
					m_nCurrAnimation = MagicMinionAnimation::Attack2;
					m_fFrameTime = 0;
				}
			}
			else if (m_nCurrAnimation == MagicMinionAnimation::Attack2)
			{
				if (m_fFrameTime > m_nAniLength[m_nCurrAnimation])
				{
					m_nCurrAnimation = MagicMinionAnimation::Attack1;
					m_fFrameTime = 0;

				}
			}
			break;

		case MinionState::Walking:
			if (m_nCurrAnimation != MagicMinionAnimation::StartWalk &&
				m_nCurrAnimation != MagicMinionAnimation::Walking)
				m_nCurrAnimation = MagicMinionAnimation::StartWalk;
			if (m_nCurrAnimation == MagicMinionAnimation::StartWalk) {
				if (m_fFrameTime > m_nAniLength[m_nCurrAnimation]-1)
				{
					m_nCurrAnimation = MagicMinionAnimation::Walking;
					m_fFrameTime = 0;
				}
			}
			break;
		case MinionState::Die:

			break;
		default:

			break;
		}
	CMinion::Animate(timeElapsed);

}

//////////////////////////////////////////////////////////////////////////
//활 미니언
CBowMinion::CBowMinion(CCreateMgr * pCreateMgr) : CMinion(pCreateMgr)
{
}

CBowMinion::CBowMinion(CCreateMgr * pCreateMgr, int nMeshes) : CMinion(pCreateMgr, nMeshes)
{
}

CBowMinion::~CBowMinion()
{
}

void CBowMinion::Animate(float timeElapsed)
{
		switch (m_CurrState) {
		case MinionState::Idle:

			if (m_nCurrAnimation != BowMinionAnimation::Idle)
				m_nCurrAnimation = BowMinionAnimation::Idle;
			break;
		case MinionState::Attack:
			
			break;

		case MinionState::Walking:
			if (m_nCurrAnimation != BowMinionAnimation::StartWalk &&
				m_nCurrAnimation != BowMinionAnimation::Walking)
				m_nCurrAnimation = BowMinionAnimation::StartWalk;
			if (m_nCurrAnimation == BowMinionAnimation::StartWalk) {
				if (m_fFrameTime > m_nAniLength[m_nCurrAnimation]-1)
				{
					m_nCurrAnimation = BowMinionAnimation::Walking;
					m_fFrameTime = 0;
				}
			}
			break;
		case MinionState::Die:

			break;
		default:

			break;
		}
	CMinion::Animate(timeElapsed);

}
