#include "stdafx.h"
#include "Minion.h"
#include "06.Meshes/00.Vertex/Vertex.h"


CMinion::CMinion(CCreateMgr * pCreateMgr) : CBaseObject(pCreateMgr)
{
	m_fFrameTime = 0;

}

void CMinion::Animate(float timeElapsed)
{

	m_fFrameTime += 30 * timeElapsed;

	CSkinnedMesh* pMesh = dynamic_cast<CSkinnedMesh*>(m_ppMeshes[0]);

	int Bcnt = m_pSkeleton->GetBoneCount() - 1; // 이거 1 줄어야 디버그에서 오류 안남 왜인진 모르겠다.
	switch (AnimateState) {
	case MinionState::Walking:
		if (m_fFrameTime >= 33) {
			m_fFrameTime =0;
		}
		for (int i = 0; i < Bcnt; ++i) {
				m_xmf4x4Frame[i] = m_pSkeleton[0].GetBone(i).GetFrame((int)m_fFrameTime);
		}
		break;
	case MinionState::Attack_1:
		if (m_fFrameTime >= 49) {
			m_fFrameTime =0;
		}
		for (int i = 0; i < Bcnt; ++i) {
			m_xmf4x4Frame[i] = m_pSkeleton[1].GetBone(i).GetFrame((int)m_fFrameTime);
		}
		break;
	case MinionState::Die:
		if (m_fFrameTime >= 33) {
			m_fFrameTime =32;
		}
		for (int i = 0; i < Bcnt; ++i) {
			m_xmf4x4Frame[i] = m_pSkeleton[2].GetBone(i).GetFrame((int)m_fFrameTime);
		}
		break;
	default:
		XMFLOAT4X4 mat{
			1,0,0,0,
			0,1,0,0,
			0,0,1,0,
			0,0,0,1
		};

		for (int i = 0; i < Bcnt; ++i) {
			m_xmf4x4Frame[i] = mat;
		}
		break;
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

	if (m_d3dCbvGPUDescriptorHandle.ptr)
		m_pCommandList->SetGraphicsRootDescriptorTable(7, m_d3dCbvGPUDescriptorHandle);

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
