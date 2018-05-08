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
	SetType(ObjectType::SwordMinion);

	m_StatusInfo.maxHP = 445;
	m_StatusInfo.HP = 445;
	m_StatusInfo.Atk = 12;
	m_StatusInfo.Def = 0;
	m_StatusInfo.Exp = 64;
}

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
		switch (m_CurrState) {
		case States::Idle:
			if (m_nCurrAnimation != SwordMinionAnimation::Idle)
				m_nCurrAnimation = SwordMinionAnimation::Idle;
			break;
		case States::Attack:
			if (m_nCurrAnimation == SwordMinionAnimation::Attack1) {
				if (m_fFrameTime > m_nAniLength[m_nCurrAnimation] / 2 &&
					m_CurrState == m_state)
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

		case States::Walk:
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
		case States::Die:

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
	SetType(ObjectType::StaffMinion);
	m_StatusInfo.maxHP = 280;
	m_StatusInfo.HP = 280;
	m_StatusInfo.Atk = 23;
	m_StatusInfo.Def = 0;
	m_StatusInfo.Exp = 64;
}

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
		switch (m_CurrState) {
		case States::Idle:
			if (m_nCurrAnimation != MagicMinionAnimation::Idle)
				m_nCurrAnimation = MagicMinionAnimation::Idle;
			break;
		case States::Attack:
			if (m_nCurrAnimation == MagicMinionAnimation::Attack1) {
				if (m_fFrameTime > m_nAniLength[m_nCurrAnimation] / 2 &&
					m_CurrState == m_state)
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

		case States::Walk:
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
		case States::Die:

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
	SetType(ObjectType::BowMinion);
	m_StatusInfo.maxHP = 300;
	m_StatusInfo.HP = 300;
	m_StatusInfo.Atk = 20;
	m_StatusInfo.Def = 0;
	m_StatusInfo.Exp = 64;
}

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
		switch (m_CurrState) {
		case States::Idle:

			if (m_nCurrAnimation != BowMinionAnimation::Idle)
				m_nCurrAnimation = BowMinionAnimation::Idle;
			break;
		case States::Attack:
			
			break;

		case States::Walk:
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
		case States::Die:

			break;
		default:

			break;
		}
	CMinion::Animate(timeElapsed);

}
