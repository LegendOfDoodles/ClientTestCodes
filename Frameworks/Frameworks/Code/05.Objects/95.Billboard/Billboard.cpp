#include "stdafx.h"
#include "Billboard.h"

/// <summary>
/// 목적: UI 이용을 위한 Bilboard 클래스 제작
/// 최종 수정자:  김나단
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-05-05
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CBillboardObject::CBillboardObject(CCreateMgr *pCreateMgr) : CBaseObject(pCreateMgr)
{
	//CTexturedRectMesh *pRectMesh = new CTexturedRectMesh(pCreateMgr, 50.f, 50.0f, 0.f);
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
CUIObject::CUIObject(CCreateMgr * pCreateMgr) : CBillboardObject(pCreateMgr)
{
	m_xmf3Position = XMFLOAT3(0,0,0);

	CreateShaderVariables(pCreateMgr);
}

CUIObject::CUIObject(CCreateMgr * pCreateMgr, UIType type) :CBillboardObject(pCreateMgr)
{
	CTexturedRectMesh *pRectMesh = NULL;

	switch (type)
	{
	case UIType::Minimap:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 160.f, FRAME_BUFFER_HEIGHT / 180.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIType::KDA:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 458.f, FRAME_BUFFER_HEIGHT / 720.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIType::Skill:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 200.f, FRAME_BUFFER_HEIGHT / 225.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIType::Status:
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
}

void CUIObject::Animate(float fTimeElapsed)
{
	CBillboardObject::Animate(fTimeElapsed);
	
	XMFLOAT3 newPos{0, 0, 0};

	switch (m_type)
	{
	case Minimap:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 102.85f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), (FRAME_BUFFER_WIDTH / 94.8f)));
		break;
	case KDA:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), (FRAME_BUFFER_HEIGHT / 80.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), (FRAME_BUFFER_WIDTH / 82.5f)));
		break;
	case Skill:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 96.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), (FRAME_BUFFER_WIDTH / 1280.f)));
		break;
	case Status:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), (FRAME_BUFFER_HEIGHT / 96.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), -(FRAME_BUFFER_WIDTH / 91.4f)));
		break;
	default:
		break;
	}


	m_xmf4x4World._41 = newPos.x; 
	m_xmf4x4World._42 = newPos.y; 
	m_xmf4x4World._43 = newPos.z; 
}

void CUIObject::Render(CCamera *pCamera, UINT istanceCnt)
{
	OnPrepareRender();

	if (m_pMaterial)
	{
		m_pMaterial->Render(pCamera);
		m_pMaterial->UpdateShaderVariables();
	}

	if (m_cbvGPUDescriptorHandle.ptr)
		m_pCommandList->SetGraphicsRootDescriptorTable(10, m_cbvGPUDescriptorHandle);

	if (m_pShader)
	{
		UpdateShaderVariables();
		m_pShader->Render(pCamera);
	}

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(istanceCnt);
		}
	}
}

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CHPGaugeObjects::CHPGaugeObjects(CCreateMgr * pCreateMgr)
	: CBillboardObject(pCreateMgr)
{
	

	CreateShaderVariables(pCreateMgr);
}

CHPGaugeObjects::CHPGaugeObjects(CCreateMgr * pCreateMgr, GaugeUiType type)
	: CBillboardObject(pCreateMgr)
{
	// HP게이지 Mesh
	CTexturedRectMesh *pRectMesh = new CTexturedRectMesh(pCreateMgr, 40.f, 5.0f, 0.f);
	SetMesh(0, pRectMesh);

	m_Type = type;
}

CHPGaugeObjects::~CHPGaugeObjects()
{
}

void CHPGaugeObjects::Animate(float fTimeElapsed)
{
	CBillboardObject::Animate(fTimeElapsed);

	if (m_Type == GaugeUiType::PlayerGauge)
	{
		m_xmf4x4World._41 = m_pMasterObject->GetPosition().x;
		m_xmf4x4World._42 = m_pMasterObject->GetPosition().y + 110.f;
		m_xmf4x4World._43 = m_pMasterObject->GetPosition().z;
	}
	else if (m_Type == GaugeUiType::MinionGauge)
	{
		m_xmf4x4World._41 = m_pMasterObject->GetPosition().x;
		m_xmf4x4World._42 = m_pMasterObject->GetPosition().y + 80.f;
		m_xmf4x4World._43 = m_pMasterObject->GetPosition().z;
	}
	
}
