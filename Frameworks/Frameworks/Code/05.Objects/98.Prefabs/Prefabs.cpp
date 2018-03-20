#include "stdafx.h"
#include "Prefabs.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// 목적: 사용하는 매터리얼 정리용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-03-09
/// </summary>

////////////////////////////////////////////////////////////////////////
// Materials
CMaterial* Materials::CreateBrickMaterial(CCreateMgr *pCreateMgr,
	D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
	D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle)
{
	CMaterial *pMaterial{ new CMaterial(pCreateMgr) };
	//CTexture *pTexture{ new CTexture(2, RESOURCE_TEXTURE_2D, 0) };
	//pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Stones.dds", 0);
	//pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/StonesNormal.dds", 1);
	//
	//CreateShaderResourceViews(
	//	pCreateMgr, pTexture,
	//	3, true, 
	//	pSrvCPUDescriptorStartHandle,
	//	pSrvGPUDescriptorStartHandle);
	//
	pMaterial->Initialize(pCreateMgr);

	//pMaterial->SetTexture(pTexture);

	pMaterial->SetAlbedo(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
	pMaterial->SetRoughness(0.4f);
	pMaterial->SetMetalic(1.0f);

	return pMaterial;
}

// SRV 생성 함수 ...
void Materials::GetShaderResourceViewDesc(
	D3D12_RESOURCE_DESC resourceDesc, 
	UINT nTextureType, 
	D3D12_SHADER_RESOURCE_VIEW_DESC *pShaderResourceViewDesc)
{
	pShaderResourceViewDesc->Format = resourceDesc.Format;
	pShaderResourceViewDesc->Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	switch (nTextureType)
	{
	case RESOURCE_TEXTURE_2D: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 1)
		pShaderResourceViewDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		pShaderResourceViewDesc->Texture2D.MipLevels = -1;
		pShaderResourceViewDesc->Texture2D.MostDetailedMip = 0;
		pShaderResourceViewDesc->Texture2D.PlaneSlice = 0;
		pShaderResourceViewDesc->Texture2D.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE_2D_ARRAY: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize != 1)
		pShaderResourceViewDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2DARRAY;
		pShaderResourceViewDesc->Texture2DArray.MipLevels = -1;
		pShaderResourceViewDesc->Texture2DArray.MostDetailedMip = 0;
		pShaderResourceViewDesc->Texture2DArray.PlaneSlice = 0;
		pShaderResourceViewDesc->Texture2DArray.ResourceMinLODClamp = 0.0f;

		pShaderResourceViewDesc->Texture2DArray.MostDetailedMip = 0;
		pShaderResourceViewDesc->Texture2DArray.MipLevels = -1;
		pShaderResourceViewDesc->Texture2DArray.FirstArraySlice = 0;
		pShaderResourceViewDesc->Texture2DArray.ArraySize = resourceDesc.DepthOrArraySize;
		pShaderResourceViewDesc->Texture2DArray.PlaneSlice = 1;
		pShaderResourceViewDesc->Texture2DArray.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_TEXTURE_CUBE: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_TEXTURE2D)(d3dResourceDesc.DepthOrArraySize == 6)
		pShaderResourceViewDesc->ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		pShaderResourceViewDesc->TextureCube.MipLevels = 1;
		pShaderResourceViewDesc->TextureCube.MostDetailedMip = 0;
		pShaderResourceViewDesc->TextureCube.ResourceMinLODClamp = 0.0f;
		break;
	case RESOURCE_BUFFER: //(d3dResourceDesc.Dimension == D3D12_RESOURCE_DIMENSION_BUFFER)
		pShaderResourceViewDesc->ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
		pShaderResourceViewDesc->Buffer.FirstElement = 0;
		pShaderResourceViewDesc->Buffer.NumElements = 0;
		pShaderResourceViewDesc->Buffer.StructureByteStride = 0;
		pShaderResourceViewDesc->Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
		break;
	}
}

void Materials::CreateShaderResourceViews(
	CCreateMgr *pCreateMgr, CTexture *pTexture,
	UINT nRootParameterStartIndex, bool bAutoIncrement,
	D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
	D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle)
{
	UINT incrementSize = pCreateMgr->GetCbvSrvDescriptorIncrementSize();
	D3D12_CPU_DESCRIPTOR_HANDLE srvCPUDescriptorHandle = *pSrvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE srvGPUDescriptorHandle = *pSrvGPUDescriptorStartHandle;
	D3D12_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
	int nTextures = pTexture->GetTextureCount();
	int nTextureType = pTexture->GetTextureType();
	for (int i = 0; i < nTextures; i++)
	{
		ID3D12Resource *pShaderResource = pTexture->GetTexture(i);
		D3D12_RESOURCE_DESC resourceDesc = pShaderResource->GetDesc();
		GetShaderResourceViewDesc(resourceDesc, nTextureType, &shaderResourceViewDesc);
		pCreateMgr->GetDevice()->CreateShaderResourceView(pShaderResource, &shaderResourceViewDesc, srvCPUDescriptorHandle);
		srvCPUDescriptorHandle.ptr += incrementSize;

		pTexture->SetRootArgument(i, (bAutoIncrement) ? (nRootParameterStartIndex + i) : nRootParameterStartIndex, srvGPUDescriptorHandle);
		srvGPUDescriptorHandle.ptr += incrementSize;
	}
}
// ... SRV 생성 함수