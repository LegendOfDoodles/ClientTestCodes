#include "stdafx.h"
#include "UIFrameObject.h"

/// <summary>
/// 목적: UI Frame Object
/// 최종 수정자:  이용선
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-05-23
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CUIFrameObject::CUIFrameObject(CCreateMgr * pCreateMgr) : CBillboardObject(pCreateMgr)
{
	m_xmf3Position = XMFLOAT3(0, 0, 0);
}

CUIFrameObject::CUIFrameObject(CCreateMgr * pCreateMgr, UIFrameType type) : CBillboardObject(pCreateMgr)
{
	CTexturedRectMesh *pRectMesh = NULL;

	switch (type)
	{
	case UIFrameType::Minimap:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 160.f, FRAME_BUFFER_HEIGHT / 180.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::KDA:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 458.f, FRAME_BUFFER_HEIGHT / 720.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::Skill:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 200.f, FRAME_BUFFER_HEIGHT / 225.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::Status:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 276.f, FRAME_BUFFER_HEIGHT / 225.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::Character:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 320.f, FRAME_BUFFER_HEIGHT / 180.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::HPGauge:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 256.f, FRAME_BUFFER_HEIGHT / 720.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::MPGauge:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 256.f, FRAME_BUFFER_HEIGHT / 1440.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::Special:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 320.f, FRAME_BUFFER_HEIGHT / 120.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::CharacterFrameHP:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 426.6f, FRAME_BUFFER_HEIGHT / 240.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::CharacterFrameMP:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 320.f, FRAME_BUFFER_HEIGHT / 180.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	}

	m_type = type;
}

CUIFrameObject::~CUIFrameObject()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CUIFrameObject::Animate(float fTimeElapsed)
{
	CBillboardObject::Animate(fTimeElapsed);

	XMFLOAT3 newPos{ 0, 0, 0 };

	switch (m_type)
	{
	case UIFrameType::Minimap:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 102.85f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), (FRAME_BUFFER_WIDTH / 94.8f)));
		break;
	case UIFrameType::KDA:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), (FRAME_BUFFER_HEIGHT / 80.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), (FRAME_BUFFER_WIDTH / 82.5f)));
		break;
	case UIFrameType::Skill:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 96.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), (FRAME_BUFFER_WIDTH / 1280.f)));
		break;
	case UIFrameType::Status:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), (FRAME_BUFFER_HEIGHT / 96.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), -(FRAME_BUFFER_WIDTH / 91.4f)));
		break;
	case UIFrameType::Character:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 96.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), -(FRAME_BUFFER_WIDTH / 85.3f)));
		break;
	case UIFrameType::HPGauge:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 96.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), -(FRAME_BUFFER_WIDTH / 128.f)));
		break;
	case UIFrameType::MPGauge:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 80.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), -(FRAME_BUFFER_WIDTH / 128.f)));
		break;
	case UIFrameType::Special:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 288.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), -(FRAME_BUFFER_WIDTH / 85.3f)));
		break;
	case UIFrameType::CharacterFrameHP:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 96.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), -(FRAME_BUFFER_WIDTH / 85.3f)));
		break;
	case UIFrameType::CharacterFrameMP:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 96.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), -(FRAME_BUFFER_WIDTH / 85.3f)));
		break;
	}

	m_xmf4x4World._41 = newPos.x;
	m_xmf4x4World._42 = newPos.y;
	m_xmf4x4World._43 = newPos.z;
}

void CUIFrameObject::Render(CCamera * pCamera, UINT istanceCnt)
{

	switch (m_type)
	{
	case UIFrameType::Minimap:
	case UIFrameType::KDA:
	case UIFrameType::Skill:
	case UIFrameType::Status:
	case UIFrameType::HPGauge:
	case UIFrameType::MPGauge:
	case UIFrameType::Special:
	case UIFrameType::Character:
		CBillboardObject::Render(pCamera, istanceCnt);
		break;
	case UIFrameType::CharacterFrameHP:
	case UIFrameType::CharacterFrameMP:
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
				if (m_ppMeshes[i]) m_ppMeshes[i]->Render(istanceCnt);
			}
		}
		break;
	}
}

float CUIFrameObject::GetCurrentHP()
{
	return (m_pMasterObject->GetPlayerStatus()->HP / m_pMasterObject->GetPlayerStatus()->maxHP);
}
