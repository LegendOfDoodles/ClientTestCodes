#include "stdafx.h"
#include "Texture.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// ����: �ؽ�ó�� �ʿ��� ó�� ����ϴ� Ŭ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-02-07
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CTexture::CTexture(int nTextures, UINT nTextureType, int nSamplers)
{
	m_textureType = nTextureType;
	m_nTextures = nTextures;
	if (m_nTextures > 0)
	{
		m_pRootArgumentInfos = new SRVROOTARGUMENTINFO[m_nTextures];
		m_ppTextureUploadBuffers = new ID3D12Resource*[m_nTextures];
		m_ppTextures = new ID3D12Resource*[m_nTextures];
	}

	m_nSamplers = nSamplers;
	if (m_nSamplers > 0) m_pSamplerGpuDescriptorHandles = new D3D12_GPU_DESCRIPTOR_HANDLE[m_nSamplers];
}

CTexture::~CTexture()
{
	if (m_ppTextures)
	{
		for (int i = 0; i < m_nTextures; i++) if (m_ppTextures[i]) m_ppTextures[i]->Release();
	}

	if (m_pRootArgumentInfos)
	{
		delete[] m_pRootArgumentInfos;
	}

	if (m_pSamplerGpuDescriptorHandles) delete[] m_pSamplerGpuDescriptorHandles;
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CTexture::Finalize()
{
	ReleaseShaderVariables();
}

void CTexture::ReleaseUploadBuffers()
{
	if (!m_ppTextureUploadBuffers) return;

	for (int i = 0; i < m_nTextures; i++)
	{
		if (m_ppTextureUploadBuffers[i])
		{
			m_ppTextureUploadBuffers[i]->Release();
		}
	}

	Safe_Delete_Array(m_ppTextureUploadBuffers);
}

void CTexture::SetRootArgument(int nIndex, UINT nRootParameterIndex, D3D12_GPU_DESCRIPTOR_HANDLE srvGpuDescriptorHandle)
{
	m_pRootArgumentInfos[nIndex].m_nRootParameterIndex = nRootParameterIndex;
	m_pRootArgumentInfos[nIndex].m_srvGpuDescriptorHandle = srvGpuDescriptorHandle;
}

void CTexture::SetSampler(int nIndex, D3D12_GPU_DESCRIPTOR_HANDLE samplerGpuDescriptorHandle)
{
	m_pSamplerGpuDescriptorHandles[nIndex] = samplerGpuDescriptorHandle;
}

void CTexture::UpdateShaderVariables()
{
	for (int i = 0; i < m_nTextures; i++)
	{
		m_pCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[i].m_nRootParameterIndex, m_pRootArgumentInfos[i].m_srvGpuDescriptorHandle);
	}
}

void CTexture::UpdateShaderVariable(int nIndex)
{
	m_pCommandList->SetGraphicsRootDescriptorTable(m_pRootArgumentInfos[nIndex].m_nRootParameterIndex, m_pRootArgumentInfos[nIndex].m_srvGpuDescriptorHandle);
}

void CTexture::LoadTextureFromFile(CCreateMgr *pCreateMgr, wchar_t *pszFileName, UINT nIndex)
{
	m_pCommandList = pCreateMgr->GetCommandList();
	m_ppTextures[nIndex] = pCreateMgr->CreateTextureResourceFromFile(pszFileName, &m_ppTextureUploadBuffers[nIndex], D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CTexture::ReleaseShaderVariables()
{
}