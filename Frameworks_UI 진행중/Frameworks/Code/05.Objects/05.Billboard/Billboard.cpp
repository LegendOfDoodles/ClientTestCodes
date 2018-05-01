#include "stdafx.h"
#include "Billboard.h"

/// <summary>
/// 목적: UI 이용을 위한 Bilboard 클래스 제작
/// 최종 수정자:  이용선
/// 수정자 목록:  이용선
/// 최종 수정 날짜: 2018-04-24
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CBillboardObject::CBillboardObject(CCreateMgr *pCreateMgr)
	: CBaseObject(pCreateMgr)
{
	//CTexturedRectMesh *pRectMesh = new CTexturedRectMesh(pCreateMgr, 5.f, 5.0f, 0.f);
	//SetMesh(0, pRectMesh);


	CreateShaderVariables(pCreateMgr);
}

CBillboardObject::~CBillboardObject()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CBillboardObject::Animate(float fTimeElapsed)
{
	if (m_pCamera != NULL) {
		SetLookAt();
	}
}

void CBillboardObject::SetLookAt()
{
	// Up Vector를 1로 가정하고 계산
	XMFLOAT3 xmf3Up = m_pCamera->GetUpVector();
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT3 xmf3Look = Vector3::ScalarProduct(m_pCamera->GetLookVector(), -1);
	XMFLOAT3 xmf3Right = Vector3::ScalarProduct(m_pCamera->GetRightVector(), -1);

	// 이후 Up 벡터를 카메라의 방향으로 갱신한다.
	//xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4World._11 = xmf3Right.x;	m_xmf4x4World._12 = xmf3Right.y;	m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x;		m_xmf4x4World._22 = xmf3Up.y;		m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x;		m_xmf4x4World._32 = xmf3Look.y;
}

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CUIObject::CUIObject(CCreateMgr * pCreateMgr)
	:CBillboardObject(pCreateMgr)
{
	m_xmf3Position = XMFLOAT3(0,0,0);

	CreateShaderVariables(pCreateMgr);
}

CUIObject::CUIObject(CCreateMgr * pCreateMgr, Type type)
	:CBillboardObject(pCreateMgr)
{
	CTexturedRectMesh *pRectMesh = NULL;

	switch (type)
	{
	case Type::Minimap:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 320.f, FRAME_BUFFER_HEIGHT / 180.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case Type::KDA:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 458.f, FRAME_BUFFER_HEIGHT / 900.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case Type::Skill:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 200.f, FRAME_BUFFER_HEIGHT / 225.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case Type::Status:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 276.f, FRAME_BUFFER_HEIGHT / 225.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	default:
		break;
	}
	
	m_type = type;

	CreateShaderVariables(pCreateMgr);
}

CUIObject::~CUIObject()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CUIObject::SetPos(XMFLOAT3 xmf3Position)
{
	m_xmf3Position = xmf3Position;

	// UI 위 포지션
	m_xmf3Position.x -= 1.5;
	m_xmf3Position.y -= 1.5;
}

void CUIObject::Animate(float fTimeElapsed)
{
	CBillboardObject::Animate(fTimeElapsed);
	
	XMFLOAT3 newPos{0, 0, 0};

	switch (m_type)
	{
	case Minimap:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -7.f)), Vector3::ScalarProduct(m_pCamera->GetRightVector(), 14.5f));
		break;
	case KDA:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), 9.2f)), Vector3::ScalarProduct(m_pCamera->GetRightVector(), 15.5f));
		break;
	case Skill:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -7.5f)), Vector3::ScalarProduct(m_pCamera->GetRightVector(), 1.f));
		break;
	case Status:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), 7.5f)), Vector3::ScalarProduct(m_pCamera->GetRightVector(), -14.f));
		break;
	default:
		break;
	}


	m_xmf4x4World._41 = newPos.x; 
	m_xmf4x4World._42 = newPos.y; 
	m_xmf4x4World._43 = newPos.z; 
}
