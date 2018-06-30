#include "stdafx.h"
#include "NumberObject.h"

CNumberOjbect::CNumberOjbect(CCreateMgr * pCreateMgr) 
	: CBillboardObject(pCreateMgr)
{
	CTexturedRectMesh *pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 85.3f, FRAME_BUFFER_HEIGHT / 360.f, 0.f);
	SetMesh(0, pRectMesh);
}

CNumberOjbect::~CNumberOjbect()
{
}

void CNumberOjbect::Animate(float fTimeElapsed)
{
	CBillboardObject::Animate(fTimeElapsed);

	XMFLOAT3 newPos{ 0, 0, 0 };

	newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
	newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), 1.f)), Vector3::ScalarProduct(m_pCamera->GetRightVector(), 1.f));

	m_xmf4x4World._41 = newPos.x;
	m_xmf4x4World._42 = newPos.y;
	m_xmf4x4World._43 = newPos.z;
}

void CNumberOjbect::Render(CCamera * pCamera, UINT istanceCnt)
{
	CBillboardObject::Render(pCamera, istanceCnt);
}

void CNumberOjbect::Num()
{
	int cnt;		// 자리수
	int Num[4];		// 일단 최대 4로 지정

	// 자리수 검색
	for (cnt = 0; m_iNum > 0; m_iNum /= 10, cnt++) /* NULL */;

	// Num[0] 부터 1의 자리 10의 자리 증가
	for (int i = 0; i < cnt; ++i) {
		Num[i] = m_iNum % 10;

		m_iNum /= 10;
	}
}
