#pragma once
#include "Material.h"

class CCreateMgr;

namespace Materials
{
	CMaterial* CreateBrickMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);


	// SRV ���� �Լ�
	void GetShaderResourceViewDesc(
		D3D12_RESOURCE_DESC resourceDesc,
		UINT nTextureType,
		D3D12_SHADER_RESOURCE_VIEW_DESC *pShaderResourceViewDesc);
		
	void CreateShaderResourceViews(
		CCreateMgr *pCreateMgr, CTexture *pTexture,
		UINT nRootParameterStartIndex, bool bAutoIncrement,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);
}