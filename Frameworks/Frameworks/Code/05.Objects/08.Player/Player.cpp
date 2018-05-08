#include "stdafx.h"
#include "Player.h"


CPlayer::CPlayer(CCreateMgr *pCreateMgr, int nMeshes)
	: CAnimatedObject(pCreateMgr, nMeshes)
{
}


CPlayer::~CPlayer()
{
}


void CPlayer::Animate(float timeElapsed)
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