#include "stdafx.h"
#include "ObjectShader.h"
#include "RotatingObject.h"
#include "CreateMgr.h"

/// <summary>
/// ����: ������Ʈ �׽�Ʈ ���̴�
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-01-12
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CObjectShader::CObjectShader(CCreateMgr *pCreateMgr)
	: CShader(pCreateMgr)
{
}

CObjectShader::~CObjectShader()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CObjectShader::ReleaseUploadBuffers()
{
	if (!m_ppObjects) return;

	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->ReleaseUploadBuffers();
	}
}

void CObjectShader::UpdateShaderVariables()
{
#if USE_INSTANCING
	m_pCommandList->SetGraphicsRootShaderResourceView(2,
		m_pInstanceBuffer->GetGPUVirtualAddress());

	for (int i = 0; i < m_nObjects; i++)
	{
		m_pMappedObjects[i].m_xmcColor =
			(i % 2) ? XMFLOAT4(0.5f, 0.0f, 0.0f, 0.0f) : XMFLOAT4(0.0f, 0.0f, 0.5f, 0.0f);
		XMStoreFloat4x4(&m_pMappedObjects[i].m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
#else
	static UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World, 
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
		pMappedObject->m_xmcColor =
			(i % 2) ? XMFLOAT4(0.5f, 0.0f, 0.0f, 0.0f) : XMFLOAT4(0.0f, 0.0f, 0.5f, 0.0f);
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

	UpdateShaderVariables();

#if USE_INSTANCING
	m_ppObjects[0]->Render(pCamera, m_nObjects);
#else
	static UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	static D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = m_pConstBuffer->GetGPUVirtualAddress();

	for (int i = 0; i < m_nObjects; i++)
	{
		if (m_ppObjects[i])
		{
			m_pCommandList->SetGraphicsRootConstantBufferView(2, gpuAddress + (i * elementBytes));
			m_ppObjects[i]->Render(pCamera);
		}
	}
#endif
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
D3D12_INPUT_LAYOUT_DESC CObjectShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new
		D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = {
		"POSITION",
		0, 
		DXGI_FORMAT_R32G32B32_FLOAT, 
		0,
		0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 
		0 };

	pd3dInputElementDescs[1] = { 
		"COLOR",
		0, 
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0, 
		12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CObjectShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
#if USE_INSTANCING
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSInstancing", "vs_5_1", ppd3dShaderBlob));
#else
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSObject", "vs_5_1", ppd3dShaderBlob));
#endif
}

D3D12_SHADER_BYTECODE CObjectShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1",	ppd3dShaderBlob));
}

void CObjectShader::CreateShader(CCreateMgr *pCreateMgr)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pCreateMgr);
}

void CObjectShader::CreateShaderVariables(CCreateMgr *pCreateMgr)
{
#if USE_INSTANCING
	m_pInstanceBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		sizeof(CB_GAMEOBJECT_INFO) * m_nObjects, 
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL);

	m_pInstanceBuffer->Map(0, NULL, (void **)&m_pMappedObjects);
#else
	UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	m_pConstBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		elementBytes * m_nObjects,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	m_pConstBuffer->Map(0, NULL, (void **)&m_pMappedObjects);
#endif

	
}

void CObjectShader::BuildObjects(CCreateMgr *pCreateMgr, void *pContext)
{
	CCubeMeshDiffused *pCubeMesh = new CCubeMeshDiffused(pCreateMgr, 12.0f, 12.0f, 12.0f);

	int xObjects = 10, yObjects = 10, zObjects = 10, i = 0;

	m_nObjects = (xObjects * 2 + 1) * (yObjects * 2 + 1) * (zObjects * 2 + 1);
	m_ppObjects = new CBaseObject*[m_nObjects];

	float fxPitch = 12.0f * 2.5f;
	float fyPitch = 12.0f * 2.5f;
	float fzPitch = 12.0f * 2.5f;

	CRotatingObject *pRotatingObject = NULL;
	for (int x = -xObjects; x <= xObjects; x++)
	{
		for (int y = -yObjects; y <= yObjects; y++)
		{
			for (int z = -zObjects; z <= zObjects; z++)
			{
				pRotatingObject = new CRotatingObject(pCreateMgr);
#if !USE_INSTANCING
				pRotatingObject->SetMesh(pCubeMesh);
#endif
				pRotatingObject->SetPosition(fxPitch*x, fyPitch*y, fzPitch*z);
				pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
				pRotatingObject->SetRotationSpeed(10.0f*(i % 10) + 3.0f);

				m_ppObjects[i++] = pRotatingObject;
			}
		}
	}

#if USE_INSTANCING
	m_ppObjects[0]->SetMesh(pCubeMesh);
#endif
	CreateShaderVariables(pCreateMgr);
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
}

void CObjectShader::ReleaseObjects()
{
	if (!m_ppObjects) return;

	for (int j = 0; j < m_nObjects; j++)
	{
		Safe_Delete(m_ppObjects[j]);
	}
	Safe_Delete_Array(m_ppObjects);
}