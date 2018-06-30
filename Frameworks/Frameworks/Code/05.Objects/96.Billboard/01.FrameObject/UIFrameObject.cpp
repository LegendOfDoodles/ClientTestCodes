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
}

CUIFrameObject::CUIFrameObject(CCreateMgr * pCreateMgr, UIFrameType type) : CBillboardObject(pCreateMgr)
{
	CTexturedRectMesh *pRectMesh = NULL;

	switch (type)
	{
	case UIFrameType::MinimapFrame:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 150.58f, FRAME_BUFFER_HEIGHT / 160.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::KDAFrame:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 458.f, FRAME_BUFFER_HEIGHT / 720.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::SkillFrameQ:
	case UIFrameType::SkillFrameW:
	case UIFrameType::SkillFrameE:
	case UIFrameType::SkillFrameR:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 640.f, FRAME_BUFFER_HEIGHT / 360.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::StatusFrame:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 256.f, FRAME_BUFFER_HEIGHT / 288.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::CharacterFrame:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 320.f, FRAME_BUFFER_HEIGHT / 180.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case UIFrameType::SpecialFrame:
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
	case UIFrameType::Minimap:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 160.f, FRAME_BUFFER_HEIGHT / 180.f, 0.f);
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
	case UIFrameType::MinimapFrame:
	case UIFrameType::Minimap:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 102.85f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), (FRAME_BUFFER_WIDTH / 94.8f)));
		break;
	case UIFrameType::KDAFrame:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), (FRAME_BUFFER_HEIGHT / 80.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), (FRAME_BUFFER_WIDTH / 82.5f)));
		break;
	case UIFrameType::SkillFrameQ:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 96.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), -(FRAME_BUFFER_WIDTH / 296.f)));
		break;
	case UIFrameType::SkillFrameW:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 96.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), -(FRAME_BUFFER_WIDTH / 914.2f)));
		break;
	case UIFrameType::SkillFrameE:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 96.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), (FRAME_BUFFER_WIDTH / 914.2f)));
		break;
	case UIFrameType::SkillFrameR:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 96.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), (FRAME_BUFFER_WIDTH / 296.f)));
		break;
	case UIFrameType::StatusFrame:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), (FRAME_BUFFER_HEIGHT / 84.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), (FRAME_BUFFER_WIDTH / FRAME_BUFFER_WIDTH)));
		break;
	case UIFrameType::CharacterFrame:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 96.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), -(FRAME_BUFFER_WIDTH / 85.3f)));
		break;
	case UIFrameType::SpecialFrame:
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
	case UIFrameType::MinimapFrame:
	case UIFrameType::KDAFrame:
	case UIFrameType::SkillFrameQ:
	case UIFrameType::SkillFrameW:
	case UIFrameType::SkillFrameE:
	case UIFrameType::SkillFrameR:
	case UIFrameType::StatusFrame:
	case UIFrameType::SpecialFrame:
	case UIFrameType::CharacterFrame:
	case UIFrameType::Minimap:
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

float CUIFrameObject::GetCurrentMP()
{
	return (m_pMasterObject->GetPlayerStatus()->MP / m_pMasterObject->GetPlayerStatus()->maxMP);
}
