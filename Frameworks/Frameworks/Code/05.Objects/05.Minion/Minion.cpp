#include "stdafx.h"
#include "Minion.h"
#include "05.Objects/03.Terrain/HeightMapTerrain.h"

/// <summary>
/// ����: ��ī�� �ڽ� ��¿� ������Ʈ
/// ���� ������:  �質��
/// ������ ���:  ������, �質��
/// ���� ���� ��¥: 2018-04-20
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CMinion::CMinion(CCreateMgr * pCreateMgr) : CBaseObject(pCreateMgr)
{
}

CMinion::CMinion(CCreateMgr * pCreateMgr, int nMeshes) : CBaseObject(pCreateMgr, nMeshes)
{
}

CMinion::~CMinion()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CMinion::Animate(float timeElapsed)
{

	m_fFrameTime += 30 * timeElapsed;

	int Bcnt = m_pSkeleton[m_nCurrAnimation].GetBoneCount();

	if (m_CurrAnimationState != m_NextAnimationState) {
		
			switch (m_NextAnimationState) {
			case MinionState::Idle:
				if (m_fFrameTime > m_nAniLength[m_nCurrAnimation])
				{
					m_nCurrAnimation = 0;
				m_CurrAnimationState = m_NextAnimationState;
				m_fFrameTime = 0;
				}
				break;
			case MinionState::Attack:
					m_nCurrAnimation = 1;
					m_CurrAnimationState = m_NextAnimationState;
					m_fFrameTime = 0;
				break;
			case MinionState::Walking:
				if (m_fFrameTime > m_nAniLength[m_nCurrAnimation])
				{
				m_nCurrAnimation = 3;
				m_CurrAnimationState = m_NextAnimationState;
				m_fFrameTime = 0;
				}
				break;
			case MinionState::Die:
				m_nCurrAnimation = 5;
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
			if (m_nCurrAnimation == 1) {
				if (m_fFrameTime > m_nAniLength[m_nCurrAnimation] / 2&&
					m_CurrAnimationState != m_NextAnimationState)
				{
					m_nCurrAnimation = 2;
					m_fFrameTime = 0;
				}
			}
			else if (m_nCurrAnimation == 2)
			{
				if (m_fFrameTime > m_nAniLength[m_nCurrAnimation])
				{
					m_nCurrAnimation = 1;
					m_fFrameTime = 0;

				}
			}
			break;

		case MinionState::Walking:
			if (m_nCurrAnimation == 3) {
				if (m_fFrameTime > m_nAniLength[m_nCurrAnimation])
				{
					m_nCurrAnimation = 4;
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

	if (m_fFrameTime > m_nAniLength[m_nCurrAnimation]) {
		while (m_fFrameTime > m_nAniLength[m_nCurrAnimation])
			m_fFrameTime -= m_nAniLength[m_nCurrAnimation];
	}

	for (int i = 0; i < Bcnt; ++i) {
		m_xmf4x4Frame[i] = m_pSkeleton[m_nCurrAnimation].GetBone(i).GetFrame((int)m_fFrameTime);
	}

	if (m_pathToGo)
	{
		if (m_destination.x == -1)	// ó�� �����̱� �����ϴ� ���
		{
			m_destination = m_pathToGo->front().To();
			m_pathToGo->pop_front();
		}
		else if (IsArrive(m_destination))	//  ���� �� ���
		{
			if (m_pathToGo->size() == 0)
			{
				Safe_Delete(m_pathToGo);
				m_destination.x = -1;
			}
			else
			{
				m_destination = m_pathToGo->front().To();
				LookAt(XMFLOAT3(m_destination.x, 0, m_destination.y));
			}
		}
		else  // ���� �������� ���� ���
		{
			MoveForwardModel(1.f);
			XMFLOAT3 position = GetPosition();
			position.y = m_pTerrain->GetHeight(position.x, position.z);
			SetPosition(position);
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
		m_pCommandList->SetGraphicsRootDescriptorTable(7, m_cbvGPUDescriptorHandle);

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
	if (m_pathToGo) Safe_Delete(m_pathToGo);
	m_pathToGo = path;
	m_destination.x = -1;
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
bool CMinion::IsArrive(const XMFLOAT2 & nextPos)
{
	static int radius = 3 * 3;
	XMFLOAT3 curPos = GetPosition();

	int distance = (nextPos.x - curPos.x) * (nextPos.x - curPos.x) + (nextPos.y - curPos.z) * (nextPos.y - curPos.z);

	return distance < radius;
}