#include "stdafx.h"
#include "Camera.h"
#include "CreateMgr.h"

/// <summary>
/// 목적: 기본 카메라 코드, 인터 페이스 용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-01-09
/// </summary>


////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CCamera::CCamera()
{
	m_xmf4x4View = Matrix4x4::Identity();
	m_xmf4x4Projection = Matrix4x4::Identity();
	m_viewport = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT, 0.0f, 1.0f };
	m_scissorRect = { 0, 0, FRAME_BUFFER_WIDTH , FRAME_BUFFER_HEIGHT };
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
	XMFLOAT4X4 xmf4x4View;
	XMStoreFloat4x4(
		&xmf4x4View, 
		XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	m_pCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4View, 0);

	XMFLOAT4X4 xmf4x4Projection;
	XMStoreFloat4x4(
		&xmf4x4Projection, 
		XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));
	m_pCommandList->SetGraphicsRoot32BitConstants(1, 16, &xmf4x4Projection, 16);
}

void CCamera::SetViewportsAndScissorRects()
{
	m_pCommandList->RSSetViewports(1, &m_viewport);
	m_pCommandList->RSSetScissorRects(1, &m_scissorRect);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CCamera::GenerateViewMatrix(
	XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up)
{
	m_xmf4x4View = Matrix4x4::LookAtLH(xmf3Position, xmf3LookAt, xmf3Up);
}

void CCamera::GenerateProjectionMatrix(
	float fNearPlaneDistance, float fFarPlaneDistance,
	float fAspectRatio, float fFOVAngle)
{
	m_xmf4x4Projection = Matrix4x4::PerspectiveFovLH(XMConvertToRadians(fFOVAngle),
		fAspectRatio, fNearPlaneDistance, fFarPlaneDistance);
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
}

void CCamera::ReleaseShaderVariables()
{
}