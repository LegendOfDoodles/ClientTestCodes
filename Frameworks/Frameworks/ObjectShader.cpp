#include "stdafx.h"
#include "ObjectShader.h"
#include "RotatingObject.h"
#include "CreateMgr.h"
#include "Material.h"

/// <summary>
/// 목적: 오브젝트 테스트 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-02-07
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CObjectShader::CObjectShader(CCreateMgr *pCreateMgr)
	: CShader(pCreateMgr)
{
}

CObjectShader::~CObjectShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CObjectShader::ReleaseUploadBuffers()
{
	if (!m_ppObjects) return;

	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->ReleaseUploadBuffers();
	}
#if USE_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CObjectShader::UpdateShaderVariables()
{
#if USE_INSTANCING
	m_pCommandList->SetGraphicsRootShaderResourceView(2,
		m_pInstanceBuffer->GetGPUVirtualAddress());

	for (int i = 0; i < m_nObjects; i++)
	{
		XMStoreFloat4x4(&m_pMappedObjects[i].m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	#if USE_BATCH_MATERIAL
		if (m_pMaterial) m_pMappedObjects[i].m_nMaterial = m_pMaterial->GetReflectionNum();
	#endif
	}
#else
	static UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World, 
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	#if USE_BATCH_MATERIAL
		if (m_pMaterial) pMappedObject->m_nMaterial = m_pMaterial->GetReflectionNum();
	#endif
	}
#endif
}

void CObjectShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CObjectShader::Render(CCamera *pCamera)
{
	CShader::Render(pCamera);

#if USE_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables();
#endif

#if USE_INSTANCING
	m_ppObjects[0]->Render(pCamera, m_nObjects);
#else
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->Render(pCamera);
	}
#endif
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CObjectShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pInputElementDescs = new
		D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pInputElementDescs[0] = {
		"POSITION",
		0, 
		DXGI_FORMAT_R32G32B32_FLOAT, 
		0,
		0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 
		0 };

	pInputElementDescs[1] ={
		"TEXCOORD", 
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 
		0 };

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
	inputLayoutDesc.pInputElementDescs = pInputElementDescs;
	inputLayoutDesc.NumElements = nInputElementDescs;

	return(inputLayoutDesc);
}

D3D12_SHADER_BYTECODE CObjectShader::CreateVertexShader(ID3DBlob **ppShaderBlob)
{
#if USE_INSTANCING
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSTxtInstancing", "vs_5_1", ppShaderBlob));
#else
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSTextured", "vs_5_1", ppShaderBlob));
#endif
}

D3D12_SHADER_BYTECODE CObjectShader::CreatePixelShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSTextured", "ps_5_1",	ppShaderBlob));
}

void CObjectShader::CreateShader(CCreateMgr *pCreateMgr)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pCreateMgr);
}

void CObjectShader::CreateShaderVariables(CCreateMgr *pCreateMgr)
{
	HRESULT hResult;

#if USE_INSTANCING
	m_pInstanceBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		sizeof(CB_GAMEOBJECT_INFO) * m_nObjects, 
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL);

	hResult = m_pInstanceBuffer->Map(0, NULL, (void **)&m_pMappedObjects);
	assert(SUCCEEDED(hResult) && "m_pInstanceBuffer->Map Failed");
#else
	UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	m_pConstBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		elementBytes * m_nObjects,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	hResult = m_pConstBuffer->Map(0, NULL, (void **)&m_pMappedObjects);
	assert(SUCCEEDED(hResult) && "m_pConstBuffer->Map Failed");
#endif

	
}

void CObjectShader::BuildObjects(CCreateMgr *pCreateMgr, void *pContext)
{
	int xObjects = 10, yObjects = 10, zObjects = 10, i = 0;

	m_nObjects = (xObjects * 2 + 1) * (yObjects * 2 + 1) * (zObjects * 2 + 1);
	m_ppObjects = new CBaseObject*[m_nObjects];

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE_2D, 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Stones.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

#if USE_INSTANCING
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 0, 1);
	CreateShaderVariables(pCreateMgr);
#else
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 1);
	CreateShaderVariables(pCreateMgr);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer, ncbElementBytes);
#endif
	CreateShaderResourceViews(pCreateMgr, pTexture, 3, false);

#if USE_BATCH_MATERIAL
	m_pMaterial = new CMaterial();
	m_pMaterial->SetTexture(pTexture);
#else
	CMaterial *pCubeMaterial = new CMaterial();
	pCubeMaterial->SetTexture(pTexture);
#endif

	CCubeMeshTextured *pCubeMesh = new CCubeMeshTextured(pCreateMgr, 12.0f, 12.0f, 12.0f);

	float fxPitch = 12.0f * 2.5f;
	float fyPitch = 12.0f * 2.5f;
	float fzPitch = 12.0f * 2.5f;

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };

	CRotatingObject *pRotatingObject = NULL;
	for (int x = -xObjects; x <= xObjects; x++)
	{
		for (int y = -yObjects; y <= yObjects; y++)
		{
			for (int z = -zObjects; z <= zObjects; z++)
			{
				pRotatingObject = new CRotatingObject(pCreateMgr);
#if !USE_INSTANCING
				pRotatingObject->SetMesh(0, pCubeMesh);
#endif
#if !USE_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				pRotatingObject->SetPosition(fxPitch*x, fyPitch*y, fzPitch*z);
				pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
				pRotatingObject->SetRotationSpeed(10.0f*(i % 10) + 3.0f);
#if !USE_INSTANCING
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_cbvGPUDescriptorStartHandle.ptr + (incrementSize * i));
#endif
				m_ppObjects[i++] = pRotatingObject;
			}
		}
	}

#if USE_INSTANCING
	m_ppObjects[0]->SetMesh(0, pCubeMesh);
#endif
}

void CObjectShader::ReleaseShaderVariables()
{
#if USE_INSTANCING
	if (!m_pInstanceBuffer) return;

	m_pInstanceBuffer->Unmap(0, NULL);
	Safe_Release(m_pInstanceBuffer);
#else
	if (!m_pConstBuffer) return;

	m_pConstBuffer->Unmap(0, NULL);
	Safe_Release(m_pConstBuffer);
#endif

	CShader::ReleaseShaderVariables();
}

void CObjectShader::ReleaseObjects()
{
	if (!m_ppObjects) return;

	for (int j = 0; j < m_nObjects; j++)
	{
		Safe_Delete(m_ppObjects[j]);
	}
	Safe_Delete_Array(m_ppObjects);

#if USE_BATCH_MATERIAL
	Safe_Delete(m_pMaterial);
#endif
}