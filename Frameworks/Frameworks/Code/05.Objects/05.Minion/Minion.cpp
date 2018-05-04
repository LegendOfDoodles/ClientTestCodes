#include "stdafx.h"
#include "Minion.h"
#include "05.Objects/03.Terrain/HeightMapTerrain.h"

/// <summary>
/// ����: �̴Ͼ� Ŭ���� ����
/// ���� ������:  ������
/// ������ ���:  ������, �質��
/// ���� ���� ��¥: 2018-04-30
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CMinion::CMinion(CCreateMgr * pCreateMgr, int nMeshes) : CAnimatedObject(pCreateMgr, nMeshes)
{
}

CMinion::~CMinion()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CMinion::Animate(float timeElapsed)
{

	if (m_fFrameTime > m_nAniLength[m_nCurrAnimation]) {
		while (m_fFrameTime > m_nAniLength[m_nCurrAnimation])
			m_fFrameTime -= m_nAniLength[m_nCurrAnimation];
	}
	int Bcnt = m_pSkeleton[m_nCurrAnimation].GetBoneCount();

	for (int i = 0; i < Bcnt; ++i) {
		m_xmf4x4Frame[i] = m_pSkeleton[m_nCurrAnimation].GetBone(i).GetFrame((int)m_fFrameTime);
	}

	if (m_pathToGo)
	{
			// Warning! IsArrive�� ������ �̵� �Ÿ��� �ٲ�� ��
		if (m_destination.x == -1 || IsArrive(m_destination, 1.0f))	//  ���� �� ���
		{
			if (m_pathToGo->empty())
			{
				Safe_Delete(m_pathToGo);
				m_destination.x = -1;
			}
			else
			{
				m_destination = m_pathToGo->front().To();
				m_pathToGo->pop_front();
				LookAt(XMFLOAT3(m_destination.x, 0, m_destination.y));
			}
		}
		else  // ���� �������� ���� ���
		{
			// Warning! �ӵ��� �����ϰ� �̵� �Ÿ��� ó���ؾ� ��
			MoveForwardModel(1.f);
			XMFLOAT3 position = GetPosition();
			position.y = m_pTerrain->GetHeight(position.x, position.z);
			CBaseObject::SetPosition(position);
			LookAt(XMFLOAT3(m_destination.x, 0, m_destination.y));
		}
	}
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
// ���� �Լ�
bool CMinion::IsArrive(XMFLOAT2 & nextPos, float dst)
{
	XMFLOAT2 curPos{ GetPosition().x, GetPosition().z };
	int distanceSqr = Vector2::DistanceSquare(curPos, nextPos);

	return distanceSqr < dst * dst;
}

//////////////////////////////////////////////////////////////////////////
//���� �̴Ͼ�
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
	m_fFrameTime += 30 * timeElapsed;


	if (m_CurrAnimationState != m_NextAnimationState) {

		switch (m_NextAnimationState) {
		case MinionState::Idle:
			if (m_fFrameTime > m_nAniLength[m_nCurrAnimation])
			{
				m_nCurrAnimation = SwordMinionAnimation::Idle;
				m_CurrAnimationState = m_NextAnimationState;
				m_fFrameTime = 0;
			}
			break;
		case MinionState::Attack:
			m_nCurrAnimation = SwordMinionAnimation::Attack1;
			m_CurrAnimationState = m_NextAnimationState;
			m_fFrameTime = 0;
			break;
		case MinionState::Walking:
			if (m_fFrameTime > m_nAniLength[m_nCurrAnimation])
			{
				m_nCurrAnimation = SwordMinionAnimation::StartWalk;
				m_CurrAnimationState = m_NextAnimationState;
				m_fFrameTime = 0;
			}
			break;
		case MinionState::Die:
			m_nCurrAnimation = SwordMinionAnimation::Die;
			m_CurrAnimationState = m_NextAnimationState;
			m_fFrameTime = 0;

			break;
		default:

			break;
		}

	}
	else {
		switch (m_CurrAnimationState) {
		case MinionState::Idle:

			break;
		case MinionState::Attack:
			if (m_nCurrAnimation == SwordMinionAnimation::Attack1) {
				if (m_fFrameTime > m_nAniLength[m_nCurrAnimation] / 2 &&
					m_CurrAnimationState == m_NextAnimationState)
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
			if (m_nCurrAnimation == SwordMinionAnimation::StartWalk) {
				if (m_fFrameTime > m_nAniLength[m_nCurrAnimation])
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
	}
	CMinion::Animate(timeElapsed);
}

//////////////////////////////////////////////////////////////////////////
//���� �̴Ͼ�
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
	m_fFrameTime += 30 * timeElapsed;


	if (m_CurrAnimationState != m_NextAnimationState) {

		switch (m_NextAnimationState) {
		case MinionState::Idle:
			if (m_fFrameTime > m_nAniLength[m_nCurrAnimation])
			{
				m_nCurrAnimation = MagicMinionAnimation::Idle;
				m_CurrAnimationState = m_NextAnimationState;
				m_fFrameTime = 0;
			}
			break;
		case MinionState::Attack:
			m_nCurrAnimation = MagicMinionAnimation::Attack1;
			m_CurrAnimationState = m_NextAnimationState;
			m_fFrameTime = 0;
			break;
		case MinionState::Walking:
			if (m_fFrameTime > m_nAniLength[m_nCurrAnimation])
			{
				m_nCurrAnimation = MagicMinionAnimation::StartWalk;
				m_CurrAnimationState = m_NextAnimationState;
				m_fFrameTime = 0;
			}
			break;
		case MinionState::Die:
			m_nCurrAnimation = MagicMinionAnimation::Die;
			m_CurrAnimationState = m_NextAnimationState;
			m_fFrameTime = 0;

			break;
		default:

			break;
		}

	}
	else {
		switch (m_CurrAnimationState) {
		case MinionState::Idle:

			break;
		case MinionState::Attack:
			if (m_nCurrAnimation == MagicMinionAnimation::Attack1) {
				if (m_fFrameTime > m_nAniLength[m_nCurrAnimation] / 2 &&
					m_CurrAnimationState == m_NextAnimationState)
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
			if (m_nCurrAnimation == MagicMinionAnimation::StartWalk) {
				if (m_fFrameTime > m_nAniLength[m_nCurrAnimation])
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
	}
	CMinion::Animate(timeElapsed);

}

//////////////////////////////////////////////////////////////////////////
//Ȱ �̴Ͼ�
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
	m_fFrameTime += 30 * timeElapsed;


	if (m_CurrAnimationState != m_NextAnimationState) {

		switch (m_NextAnimationState) {
		case MinionState::Idle:
			if (m_fFrameTime > m_nAniLength[m_nCurrAnimation])
			{
				m_nCurrAnimation = BowMinionAnimation::Idle;
				m_CurrAnimationState = m_NextAnimationState;
				m_fFrameTime = 0;
			}
			break;
		case MinionState::Attack:
			m_nCurrAnimation = BowMinionAnimation::Attack;
			m_CurrAnimationState = m_NextAnimationState;
			m_fFrameTime = 0;
			break;
		case MinionState::Walking:
			if (m_fFrameTime > m_nAniLength[m_nCurrAnimation])
			{
				m_nCurrAnimation = BowMinionAnimation::StartWalk;
				m_CurrAnimationState = m_NextAnimationState;
				m_fFrameTime = 0;
			}
			break;
		case MinionState::Die:
			m_nCurrAnimation = BowMinionAnimation::Die;
			m_CurrAnimationState = m_NextAnimationState;
			m_fFrameTime = 0;

			break;
		default:

			break;
		}

	}
	else {
		switch (m_CurrAnimationState) {
		case MinionState::Idle:

			break;
		case MinionState::Attack:
			
			break;

		case MinionState::Walking:
			if (m_nCurrAnimation == BowMinionAnimation::StartWalk) {
				if (m_fFrameTime > m_nAniLength[m_nCurrAnimation])
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
	}
	CMinion::Animate(timeElapsed);

}
