#include "stdafx.h"
#include "BaseObject.h"
#include "CreateMgr.h"
#include "Shader.h"
#include "Camera.h"
#include "Material.h"

/// <summary>
/// 목적: 기본 오브젝트 클래스, 인터페이스 용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-01-12
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CBaseObject::CBaseObject(CCreateMgr *pCreateMgr)
{
	m_pCommandList = pCreateMgr->GetCommandList();

	XMStoreFloat4x4(&m_xmf4x4World, XMMatrixIdentity());
}

CBaseObject::~CBaseObject()
{
	Safe_Release(m_pMesh);
	if (m_pShader){ m_pShader->Finalize(); }
	if (m_pMaterial) m_pMaterial->Finalize();
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CBaseObject::Initialize(CCreateMgr *pCreateMgr)
{
	CreateShaderVariables(pCreateMgr);
}

void CBaseObject::Finalize()
{
	ReleaseShaderVariables();
}

void CBaseObject::ReleaseUploadBuffers()
{
	if (m_pMesh) { m_pMesh->ReleaseUploadBuffers(); }
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
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
	//if (!m_pMaterial)
	//{
	//	CMaterial *pMaterial = new CMaterial();
	//	SetMaterial(pMaterial);
	//}
	//if (m_pMaterial) m_pMaterial->SetShader(pShader);
}

void CBaseObject::SetMaterial(CMaterial *pMaterial)
{
	if (m_pMaterial) m_pMaterial->Release();
	m_pMaterial = pMaterial;
	if (m_pMaterial) m_pMaterial->AddRef();
}

void CBaseObject::Animate(float timeElapsed)
{
}

void CBaseObject::Render(CCamera *pCamera, UINT istanceCnt)
{
	OnPrepareRender();

	if (m_pShader) { m_pShader->Render(pCamera); }

	if (m_pMesh) m_pMesh->Render(istanceCnt);
}

void CBaseObject::MoveStrafe(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Right = GetRight();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Right, fDistance);
	CBaseObject::SetPosition(xmf3Position);
}

void CBaseObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CBaseObject::SetPosition(xmf3Position);
}

void CBaseObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CBaseObject::SetPosition(xmf3Position);
}

void CBaseObject::Rotate(XMFLOAT3 *pxmf3Axis, float fAngle)
{
	XMMATRIX mtxRotate = XMMatrixRotationAxis(
		XMLoadFloat3(pxmf3Axis), 
		XMConvertToRadians(fAngle));

	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

void CBaseObject::Rotate(float fPitch, float fYaw, float fRoll)
{
	XMMATRIX mtxRotate =XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(fPitch),
		XMConvertToRadians(fYaw),
		XMConvertToRadians(fRoll));

	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}

XMFLOAT3 CBaseObject::GetPosition()
{
	return(XMFLOAT3(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43));
}

XMFLOAT3 CBaseObject::GetLook()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

XMFLOAT3 CBaseObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23)));
}

XMFLOAT3 CBaseObject::GetRight()
{
	return(Vector3::Normalize(XMFLOAT3( m_xmf4x4World._11, m_xmf4x4World._12, m_xmf4x4World._13)));
}

void CBaseObject::SetPosition(float x, float y, float z)
{
	m_xmf4x4World._41 = x;
	m_xmf4x4World._42 = y;
	m_xmf4x4World._43 = z;
}

void CBaseObject::SetPosition(XMFLOAT3 xmf3Position)
{
	SetPosition(xmf3Position.x, xmf3Position.y, xmf3Position.z);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CBaseObject::CreateShaderVariables(CCreateMgr *pCreateMgr)
{
}

void CBaseObject::ReleaseShaderVariables()
{
}

void CBaseObject::UpdateShaderVariables()
{
}

void CBaseObject::OnPrepareRender()
{
	UpdateShaderVariables();
}