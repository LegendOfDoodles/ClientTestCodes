#include "stdafx.h"
#include "LightCamera.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// ����: ShadowMap ������ ī�޶�
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-06-29
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CLightCamera::CLightCamera(XMFLOAT3 lightDir) : CCamera()
{
	m_lightDirection = lightDir;
}

CLightCamera::~CLightCamera()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CLightCamera::Initialize(CCreateMgr * pCreateMgr)
{
	float width{ TERRAIN_SIZE_WIDTH / 2 };
	float height{ TERRAIN_SIZE_HEIGHT / 2 };
	float length{ sqrt(width * width + height * height)};
	
	m_hWnd = pCreateMgr->GetHwnd();
	m_pCommandList = pCreateMgr->GetCommandList();

	SetViewport(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE, 0.0f, 1.0f);
	SetScissorRect(0, 0, SHADOW_MAP_SIZE, SHADOW_MAP_SIZE);

	GenerateOrthographicProjectionMatrix(length * 2, length* 2, 1, TERRAIN_SIZE_HEIGHT / 2 + 2 * length);

	XMFLOAT3 lightTarget{ TERRAIN_SIZE_WIDTH / 2, 0, TERRAIN_SIZE_HEIGHT / 2 };
	XMFLOAT3 lightDirRev{ Vector3::ScalarProduct(m_lightDirection, - 2 * length, false) };
	XMFLOAT3 lightPos{ Vector3::Add(lightTarget, lightDirRev) };

	GenerateViewMatrix(
		lightPos,
		lightTarget);

	XMFLOAT4X4 Tex(
		0.5f, 0.0f, 0.0f, 0.0f,
		0.0f, -0.5f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.5f, 0.5f, 0.0f, 1.0f);

	m_xmf4x4ShadowViewProjTex = Matrix4x4::Identity();
	m_xmf4x4ShadowViewProjTex = Matrix4x4::Multiply(Matrix4x4::Multiply(m_xmf4x4View, m_xmf4x4Projection), Tex);

	CreateShaderVariables(pCreateMgr);
}

void CLightCamera::UpdateShaderVariables()
{
	XMStoreFloat4x4(&m_pMappedCamera->m_xmf4x4View, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4View)));
	XMStoreFloat4x4(&m_pMappedCamera->m_xmf4x4Projection, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4Projection)));
	XMStoreFloat4x4(&m_pMappedCamera->m_xmf4x4ShadowViewProjTex, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4ShadowViewProjTex)));

	memcpy(&m_pMappedCamera->m_xmf3Position, &m_xmf3Position, sizeof(XMFLOAT3));

	D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress = m_pConstBuffer->GetGPUVirtualAddress();
	m_pCommandList->SetGraphicsRootConstantBufferView(0, gpuVirtualAddress);
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�