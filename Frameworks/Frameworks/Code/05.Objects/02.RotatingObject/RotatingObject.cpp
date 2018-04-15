#include "stdafx.h"
#include "RotatingObject.h"
#include "06.Meshes/00.Vertex/Vertex.h"
/// <summary>
/// 목적: Create 통합 중 테스트 용 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-04-09
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
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
// 공개 함수
void CRotatingObject::Animate(float timeElapsed)
{
	CBaseObject::Rotate(&m_rotationAxis, m_rotationSpeed * timeElapsed);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수





CAnimatedObject::CAnimatedObject(CCreateMgr * pCreateMgr) : CBaseObject(pCreateMgr)
{
	m_fFrameTime = 0;
	aniState = 0;
}


CAnimatedObject::CAnimatedObject(CCreateMgr * pCreateMgr,int nMeshes) : CBaseObject(pCreateMgr,nMeshes)
{
	m_fFrameTime = 0;
	aniState = 0;
}
CAnimatedObject::~CAnimatedObject()
{
	Safe_Delete(m_pSkeleton);
	Safe_Delete(m_pSkeleton1);
	Safe_Delete(m_pSkeleton2);


}

void CAnimatedObject::Animate(float timeElapsed)
{

	m_fFrameTime += 30 * timeElapsed;
	if (aniState == 0 && m_fFrameTime >= 33) {
		while (m_fFrameTime > 33)
			m_fFrameTime -= 33;
	}
	else if (aniState == 1 && m_fFrameTime > 30) {
		while(m_fFrameTime > 30)
			m_fFrameTime -= 30;
	}
	else if (aniState == 2 && m_fFrameTime > 33) {
		while (m_fFrameTime > 33)
			m_fFrameTime -= 33;
	}


	int Bcnt = m_pSkeleton->GetBoneCount(); // 이거 1 줄어야 디버그에서 오류 안남 왜인진 모르겠다.

	for (int i = 0; i < Bcnt; ++i) {
		if (aniState == 0)
			m_xmf4x4Frame[i] = m_pSkeleton->GetBone(i).GetFrame((int)m_fFrameTime);
		else if (aniState == 1)
			m_xmf4x4Frame[i] = m_pSkeleton1->GetBone(i).GetFrame((int)m_fFrameTime);
		else if (aniState == 2)
			m_xmf4x4Frame[i] = m_pSkeleton2->GetBone(i).GetFrame((int)m_fFrameTime);
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
