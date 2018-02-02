#include "stdafx.h"
#include "Material.h"

/// <summary>
/// 목적: 텍스처, 알베도, 재질 등 정리용 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-02-02
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CMaterial::CMaterial()
{
}

CMaterial::~CMaterial()
{
	Safe_Release(m_pTexture);
	Safe_Release(m_pShader);
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CMaterial::Finalize()
{
	if (m_pShader) m_pShader->Finalize();
	if (m_pTexture) m_pTexture->Finalize();

	Release();
}

void CMaterial::ReleaseUploadBuffers()
{
	if (m_pTexture) m_pTexture->ReleaseUploadBuffers();
}

void CMaterial::UpdateShaderVariables()
{
	if (m_pTexture) m_pTexture->UpdateShaderVariables();
}

void CMaterial::SetTexture(CTexture *pTexture)
{
	if (m_pTexture) m_pTexture->Release();
	m_pTexture = pTexture;
	if (m_pTexture) m_pTexture->AddRef();
}

void CMaterial::SetShader(CShader *pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

////////////////////////////////////////////////////////////////////////
// 내부 함수