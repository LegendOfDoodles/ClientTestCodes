
#include "stdafx.h"
#include "Minion.h"
#include "06.Meshes/00.Vertex/Vertex.h"


CMinion::CMinion(CCreateMgr * pCreateMgr) : CBaseObject(pCreateMgr)
{
}

CMinion::CMinion(CCreateMgr * pCreateMgr, int nMeshes) : CBaseObject(pCreateMgr, nMeshes)
{
}

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

CMinion::~CMinion()
{
}
