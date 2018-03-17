#include "stdafx.h"
#include "RotatingObject.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// 목적: Create 통합 중 테스트 용 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-03-17
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CRotatingObject::CRotatingObject(CCreateMgr *pCreateMgr, int nMeshes)
	: CBaseObject(pCreateMgr, nMeshes)
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
void CRotatingObject::CreateShaderVariables(CCreateMgr *pCreateMgr)
{
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255); //256의 배수
	m_pcbGameObject = pCreateMgr->CreateBufferResource(
		NULL,
		ncbElementBytes,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	m_pcbGameObject->Map(0, NULL, (void **)&m_pMappedObject);
}

void CRotatingObject::ReleaseShaderVariables()
{
}

void CRotatingObject::UpdateShaderVariables()
{
	CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObject);

	XMStoreFloat4x4(&pMappedObject->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));
}