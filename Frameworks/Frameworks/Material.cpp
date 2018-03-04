#include "stdafx.h"
#include "CreateMgr.h"
#include "Material.h"

/// <summary>
/// ����: �ؽ�ó, �˺���, ���� �� ������ Ŭ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-03-04
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CMaterial::CMaterial(CCreateMgr *pCreateMgr)
{
	m_pCommandList = pCreateMgr->GetCommandList();
}

CMaterial::~CMaterial()
{
	Safe_Release(m_pTexture);
	Safe_Release(m_pShader);
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CMaterial::Initialize(CCreateMgr *pCreateMgr)
{
	CreateShaderVariables(pCreateMgr);
}

void CMaterial::Finalize()
{
	if (m_pShader) m_pShader->Finalize();
	if (m_pTexture) m_pTexture->Finalize();

	ReleaseShaderVariables();
	Release();
}

void CMaterial::ReleaseUploadBuffers()
{
	if (m_pTexture) m_pTexture->ReleaseUploadBuffers();
}

void CMaterial::UpdateShaderVariables()
{
	if (m_pShader) m_pShader->UpdateShaderVariables();
	if (m_pTexture) m_pTexture->UpdateShaderVariables();
	UpdateShaderVariable();
}

void CMaterial::Render(CCamera *pCamera)
{
	if(m_pShader) m_pShader->Render(pCamera);
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
void CMaterial::CreateShaderVariables(CCreateMgr *pCreateMgr)
{
	UINT ncbMaterialBytes = ((sizeof(COLOR) + 255) & ~255); //256�� ���
	m_pcbColor = pCreateMgr->CreateBufferResource(
		NULL, 
		ncbMaterialBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, 
		NULL);

	m_pcbColor->Map(0, NULL, (void **)&m_pcbMappedColor);
}

void CMaterial::ReleaseShaderVariables()
{
	if (m_pcbColor)
	{
		m_pcbColor->Unmap(0, NULL);
		m_pcbColor->Release();
	}
}

void CMaterial::UpdateShaderVariable()
{
	m_pcbMappedColor->m_xmf4Diffuse = m_color.m_xmf4Diffuse;
	m_pcbMappedColor->m_cSpecularPower = m_color.m_cSpecularPower;
	m_pcbMappedColor->m_cRoughness = m_color.m_cRoughness;

	D3D12_GPU_VIRTUAL_ADDRESS cbColorGpuVirtualAddress = m_pcbColor->GetGPUVirtualAddress();
	m_pCommandList->SetGraphicsRootConstantBufferView(6, cbColorGpuVirtualAddress);
}