#include "stdafx.h"
#include "SkyBox.h"

/// <summary>
/// ����: ��ī�� �ڽ� ��¿� ������Ʈ
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-05-04
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CSkyBox::CSkyBox(CCreateMgr *pCreateMgr) : CBaseObject(pCreateMgr, 6)
{
	CTexturedRectMesh *pSkyBoxMesh = new CTexturedRectMesh(pCreateMgr, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, +10.0f);
	SetMesh(0, pSkyBoxMesh);
	pSkyBoxMesh = new CTexturedRectMesh(pCreateMgr, 20.0f, 20.0f, 0.0f, 0.0f, 0.0f, -10.0f);
	SetMesh(1, pSkyBoxMesh);	
	pSkyBoxMesh = new CTexturedRectMesh(pCreateMgr, 0.0f, 20.0f, 20.0f, -10.0f, 0.0f, 0.0f);
	SetMesh(2, pSkyBoxMesh);	
	pSkyBoxMesh = new CTexturedRectMesh(pCreateMgr, 0.0f, 20.0f, 20.0f, +10.0f, 0.0f, 0.0f);
	SetMesh(3, pSkyBoxMesh);	
	pSkyBoxMesh = new CTexturedRectMesh(pCreateMgr, 20.0f, 0.0f, 20.0f, 0.0f, +10.0f, 0.0f);
	SetMesh(4, pSkyBoxMesh);	
	pSkyBoxMesh = new CTexturedRectMesh(pCreateMgr, 20.0f, 0.0f, 20.0f, 0.0f, -10.0f, 0.0f);
	SetMesh(5, pSkyBoxMesh);

	CreateShaderVariables(pCreateMgr);
}

CSkyBox::~CSkyBox()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CSkyBox::Render(CCamera *pCamera, UINT istanceCnt)
{
	if (m_cbvGPUDescriptorHandle.ptr)
		m_pCommandList->SetGraphicsRootDescriptorTable(2, m_cbvGPUDescriptorHandle);

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_pMaterial) m_pMaterial->UpdateShaderVariable(i);
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(istanceCnt);
		}
	}
}