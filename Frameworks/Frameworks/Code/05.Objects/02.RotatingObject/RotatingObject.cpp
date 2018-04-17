#include "stdafx.h"
#include "RotatingObject.h"
#include "06.Meshes/00.Vertex/Vertex.h"
/// <summary>
/// ����: Create ���� �� �׽�Ʈ �� Ŭ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-04-09
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CRotatingObject::CRotatingObject(CCreateMgr *pCreateMgr)
	: CBaseObject(pCreateMgr)
{
	m_rotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_rotationSpeed = 90.0f;
}

CRotatingObject::~CRotatingObject()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CRotatingObject::Animate(float timeElapsed)
{
	CBaseObject::Rotate(&m_rotationAxis, m_rotationSpeed * timeElapsed);
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�





CAnimatedObject::CAnimatedObject(CCreateMgr * pCreateMgr) : CBaseObject(pCreateMgr)
{
	m_fFrameTime = 0;
	m_nAniState = 0;
}


CAnimatedObject::CAnimatedObject(CCreateMgr * pCreateMgr,int nMeshes) : CBaseObject(pCreateMgr,nMeshes)
{
	m_fFrameTime = 0;
	m_nAniState = 0;
}
CAnimatedObject::~CAnimatedObject()
{
	
}

void CAnimatedObject::Animate(float timeElapsed)
{

	m_fFrameTime += 30 * timeElapsed;
	if (m_fFrameTime > m_nAniLength[m_nAniState]) {
		while (m_fFrameTime > m_nAniLength[m_nAniState])
			m_fFrameTime -= m_nAniLength[m_nAniState];
	}

	int Bcnt = m_pSkeleton[m_nAniState].GetBoneCount(); // �̰� 1 �پ�� ����׿��� ���� �ȳ� ������ �𸣰ڴ�.

	for (int i = 0; i < Bcnt; ++i) {
		m_xmf4x4Frame[i] = m_pSkeleton[m_nAniState].GetBone(i).GetFrame((int)m_fFrameTime);
	}

}

void CAnimatedObject::Render(CCamera * pCamera, UINT instanceCnt)
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
