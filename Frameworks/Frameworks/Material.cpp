#include "stdafx.h"
#include "Material.h"

/// <summary>
/// ����: �ؽ�ó, �˺���, ���� �� ������ Ŭ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-02-02
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CMaterial::CMaterial()
{
}

CMaterial::~CMaterial()
{
	Safe_Release(m_pTexture);
	Safe_Release(m_pShader);
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
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
// ���� �Լ�