#include "stdafx.h"
#include "BaseObject.h"
#include "Shader.h"

/// <summary>
/// ����: �⺻ ������Ʈ Ŭ����, �������̽� ��
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-01-09
/// </summary>


////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CBaseObject::CBaseObject()
{
	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

CBaseObject::~CBaseObject()
{
	if (m_pMesh) m_pMesh->Release();
	if (m_pShader)
	{
		m_pShader->ReleaseShaderVariables();
		m_pShader->Release();
	}
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CBaseObject::ReleaseUploadBuffers()
{
	if (m_pMesh) { m_pMesh->ReleaseUploadBuffers(); }
}

void CBaseObject::SetMesh(CMesh *pMesh)
{
	if (m_pMesh) m_pMesh->Release();
	m_pMesh = pMesh;
	if (m_pMesh) m_pMesh->AddRef();
}

void CBaseObject::SetShader(CShader *pShader)
{
	if (m_pShader) m_pShader->Release();
	m_pShader = pShader;
	if (m_pShader) m_pShader->AddRef();
}

void CBaseObject::Animate(float timeElapsed)
{
}

void CBaseObject::OnPrepareRender()
{
}

void CBaseObject::Render()
{
	OnPrepareRender();

	if (m_pShader) m_pShader->Render();
	if (m_pMesh) m_pMesh->Render();
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�