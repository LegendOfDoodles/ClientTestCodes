#include "stdafx.h"
#include "BaseObject.h"
#include "Shader.h"
#include "Camera.h"

/// <summary>
/// ����: �⺻ ������Ʈ Ŭ����, �������̽� ��
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-01-12
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
		m_pShader->Finalize();
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

void CBaseObject::Render(CCamera *pCamera)
{
	OnPrepareRender();

	if (m_pShader)
	{
		//���� ��ü�� ���� ��ȯ ����� ���̴��� ��� ���۷� ����(����)�Ѵ�.
		m_pShader->UpdateShaderVariable(&m_xmf4x4World);
		m_pShader->Render(pCamera);
	}

	if (m_pMesh) m_pMesh->Render();
}

void CBaseObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(XMLoadFloat3(pxmf3Axis), XMConvertToRadians(fAngle));
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�