#include "stdafx.h"
#include "SkyBox.h"

/// <summary>
/// 목적: 스카이 박스 출력용 오브젝트
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-03-27
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
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
// 공개 함수
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