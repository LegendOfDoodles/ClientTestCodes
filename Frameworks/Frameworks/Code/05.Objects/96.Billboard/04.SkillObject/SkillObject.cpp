#include "stdafx.h"
#include "SkillObject.h"

/// <summary>
/// 목적: Skill UI Icon Object
/// 최종 수정자:  이용선
/// 수정자 목록:  이용선, 
/// 최종 수정 날짜: 2018-06-11
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CSkillObject::CSkillObject(CCreateMgr * pCreateMgr)
	: CBillboardObject(pCreateMgr)
{
}

CSkillObject::CSkillObject(CCreateMgr * pCreateMgr, SkillUIType type) : CBillboardObject(pCreateMgr)
{
	CTexturedRectMesh *pRectMesh = NULL;

	switch (type)
	{
	case GreyQSkill:
	case GreyWSkill:
	case GreyESkill:
	case GreyRSkill:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 711.1f, FRAME_BUFFER_HEIGHT / 400.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	case QSkill:
	case WSkill:
	case ESkill:
	case RSkill:
		pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 711.1f, FRAME_BUFFER_HEIGHT / 400.f, 0.f);
		SetMesh(0, pRectMesh);
		break;
	default:
		break;
	}

	m_type = type;
}

CSkillObject::~CSkillObject()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CSkillObject::Animate(float fTimeElapsed)
{
	CBillboardObject::Animate(fTimeElapsed);

	XMFLOAT3 newPos{ 0, 0, 0 };

	switch (m_type)
	{
	case GreyQSkill:
	case QSkill:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 96.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), -(FRAME_BUFFER_WIDTH / 296.f)));
		break;
	case GreyWSkill:
	case WSkill:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 96.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), -(FRAME_BUFFER_WIDTH / 914.2f)));
		break;
	case GreyESkill:
	case ESkill:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 96.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), (FRAME_BUFFER_WIDTH / 914.2f)));
		break;
	case GreyRSkill:
	case RSkill:
		newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
		newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -(FRAME_BUFFER_HEIGHT / 96.f))), Vector3::ScalarProduct(m_pCamera->GetRightVector(), (FRAME_BUFFER_WIDTH / 296.f)));
		break;
	default:
		break;
	}

	m_xmf4x4World._41 = newPos.x;
	m_xmf4x4World._42 = newPos.y;
	m_xmf4x4World._43 = newPos.z;
}

void CSkillObject::Render(CCamera * pCamera, UINT istanceCnt)
{
	switch (m_type)
	{
	case GreyQSkill:
	case GreyWSkill:
	case GreyESkill:
	case GreyRSkill:
		//CBillboardObject::Render(pCamera, istanceCnt);
		//break;
	case QSkill:
	case WSkill:
	case ESkill:
	case RSkill:
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
	default:
		break;
	}
}

float CSkillObject::GetCoolTime()
{
	return 0.0f;
}
