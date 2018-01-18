#include "stdafx.h"
#include "Camera.h"
#include "CreateMgr.h"
#include "Player.h"

/// <summary>
/// 목적: 기본 카메라 코드, 인터 페이스 용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-01-13
/// </summary>


////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CCamera::CCamera(CCamera *pCamera)
{
	if (pCamera)
	{
		//카메라가 이미 있으면 기존 카메라의 정보를 새로운 카메라에 복사한다.
		*this = *pCamera;
	}
	else
	{
		//카메라가 없으면 기본 정보를 설정한다.
		m_xmf4x4View = Matrix4x4::Identity();
		m_xmf4x4Projection = Matrix4x4::Identity();
		m_viewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
		m_scissorRect = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
	}
}

CCamera::~CCamera()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CCamera::Initialize(CCreateMgr *pCreateMgr)
{
	int width = pCreateMgr->GetWindowWidth();
	int height = pCreateMgr->GetWindowHeight();

	m_pCommandList = pCreateMgr->GetCommandList();

	SetViewport(0, 0, width, height, 0.0f, 1.0f);
	SetScissorRect(0, 0, width, height);
	GenerateProjectionMatrix(1.0f, 500.0f, float(width) / 	float(height), 90.0f);
	GenerateViewMatrix(
		XMFLOAT3(0.0f, 0.0f, -50.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(0.0f, 1.0f, 0.0f));

	CreateShaderVariables(pCreateMgr);
}

void CCamera::Finalize()
{
	ReleaseShaderVariables();
}

void CCamera::UpdateShaderVariables()
{
	XMStoreFloat4x4(&m_pMappedCamera->m_xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	XMStoreFloat4x4(&m_pMappedCamera->m_xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));

	D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress = m_pConstBuffer->GetGPUVirtualAddress();
	m_pCommandList->SetGraphicsRootConstantBufferView(1, gpuVirtualAddress);
}

void CCamera::SetViewportsAndScissorRects()
{
	m_pCommandList->RSSetViewports(1, &m_viewport);
	m_pCommandList->RSSetScissorRects(1, &m_scissorRect);
}

void CCamera::Move(XMFLOAT3& xmf3Shift)
{
	m_xmf3Position.x += xmf3Shift.x;
	m_xmf3Position.y += xmf3Shift.y; 
	m_xmf3Position.z += xmf3Shift.z;
}

void CCamera::Rotate(float fPitch, float fYaw, float fRoll)
{
}

void CCamera::Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed)
{
}

void CCamera::SetLookAt(XMFLOAT3& xmf3LookAt)
{
}

void CCamera::SetOffset(XMFLOAT3 xmf3Offset) {
	m_xmf3Offset = xmf3Offset;

	m_xmf3Position.x += xmf3Offset.x;
	m_xmf3Position.y += xmf3Offset.y;
	m_xmf3Position.z += xmf3Offset.z;
}

void CCamera::RegenerateViewMatrix()
{
	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);

	m_xmf4x4View._11 = m_xmf3Right.x; m_xmf4x4View._12 = m_xmf3Up.x; m_xmf4x4View._13 = m_xmf3Look.x;
	m_xmf4x4View._21 = m_xmf3Right.y; m_xmf4x4View._22 = m_xmf3Up.y; m_xmf4x4View._23 = m_xmf3Look.y;
	m_xmf4x4View._31 = m_xmf3Right.z; m_xmf4x4View._32 = m_xmf3Up.z; m_xmf4x4View._33 = m_xmf3Look.z;

	m_xmf4x4View._41 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Right);
	m_xmf4x4View._42 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Up);
	m_xmf4x4View._43 = -Vector3::DotProduct(m_xmf3Position, m_xmf3Look);
}

void CCamera::GenerateProjectionMatrix(
	float fNearPlaneDistance, float fFarPlaneDistance,
	float fAspectRatio, float fFOVAngle)
{
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fFOVAngle),
		fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CCamera::GenerateViewMatrix()
{
	m_xmf4x4View = Matrix4x4::LookAtLH(m_xmf3Position, m_xmf3LookAtWorld, m_xmf3Up);
}

void CCamera::GenerateViewMatrix(
	XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up)
{
	m_xmf3Position = xmf3Position;
	m_xmf3LookAtWorld = xmf3LookAt;
	m_xmf3Up = xmf3Up;

	GenerateViewMatrix();
}

void CCamera::SetViewport(
	int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ, float fMaxZ)
{
	m_viewport.TopLeftX = float(xTopLeft);
	m_viewport.TopLeftY = float(yTopLeft);
	m_viewport.Width = float(nWidth);
	m_viewport.Height = float(nHeight);
	m_viewport.MinDepth = fMinZ;
	m_viewport.MaxDepth = fMaxZ;
}

void CCamera::SetScissorRect(
	LONG xLeft, LONG yTop, LONG xRight, LONG yBottom)
{
	m_scissorRect.left = xLeft;
	m_scissorRect.top = yTop;
	m_scissorRect.right = xRight;
	m_scissorRect.bottom = yBottom;
}

void CCamera::CreateShaderVariables(CCreateMgr *pCreateMgr)
{
	UINT ncbElementBytes = ((sizeof(VS_CB_CAMERA_INFO) + 255) & ~255); //256의 배수

	m_pConstBuffer = pCreateMgr->CreateBufferResource(
		NULL, 
		ncbElementBytes, 
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, 
		NULL);

	m_pConstBuffer->Map(0, NULL, (void **)&m_pMappedCamera);
}

void CCamera::ReleaseShaderVariables()
{
	if (!m_pConstBuffer) return;

	m_pConstBuffer->Unmap(0, NULL);
	Safe_Release(m_pConstBuffer);
}