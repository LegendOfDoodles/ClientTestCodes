#include "stdafx.h"
#include "AnimatedObject.h"
#include "06.Meshes/00.Vertex/Vertex.h"
/// <summary>
/// 목적: Create 통합 중 테스트 용 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-04-24
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CAnimatedObject::CAnimatedObject(CCreateMgr * pCreateMgr, int nMeshes) : CBaseObject(pCreateMgr, nMeshes)
{
	m_fFrameTime = 0;
	m_nAniState = 0;
}

CAnimatedObject::~CAnimatedObject()
{
	
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CAnimatedObject::Animate(float timeElapsed)
{

	m_fFrameTime += 30 * timeElapsed;
	if (m_fFrameTime > m_nAniLength[m_nAniState]) {
		while (m_fFrameTime > m_nAniLength[m_nAniState])
			m_fFrameTime -= m_nAniLength[m_nAniState];
	}

	int Bcnt = m_pSkeleton[m_nAniState].GetBoneCount(); 

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

////////////////////////////////////////////////////////////////////////
// 내부 함수

