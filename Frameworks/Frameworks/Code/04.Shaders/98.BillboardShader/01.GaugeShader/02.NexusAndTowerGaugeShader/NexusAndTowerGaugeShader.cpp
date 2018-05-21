#include "stdafx.h"
#include "NexusAndTowerGaugeShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/96.Billboard/Billboard.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"

/// <summary>
/// ����: Nexus Tower HP Gauge ���̴�
/// ���� ������:  �̿뼱
/// ������ ���:  �̿뼱
/// ���� ���� ��¥: 2018-05-13
/// </summary>

CNexusAndTowerHPGaugeShader::CNexusAndTowerHPGaugeShader(CCreateMgr * pCreateMgr)
	:CShader(pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;
}

CNexusAndTowerHPGaugeShader::~CNexusAndTowerHPGaugeShader()
{
}

////////////////////////////////////////////////////////////////////////
//

void CNexusAndTowerHPGaugeShader::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			m_ppObjects[j]->ReleaseUploadBuffers();
		}
	}

#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
#endif
}

void CNexusAndTowerHPGaugeShader::UpdateShaderVariables()
{
	static UINT elementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAUGE_INFO *pMappedObject = (CB_GAUGE_INFO *)(m_pMappedObjects + (i * elementBytes));
		pMappedObject->m_fCurrentHP = ((CHPGaugeObjects*)m_ppObjects[i])->GetCurrentHP();
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CNexusAndTowerHPGaugeShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CNexusAndTowerHPGaugeShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera);

#if USE_BATCH_MATERIAL
	if (m_ppMaterials) m_ppMaterials[0]->UpdateShaderVariables();
#endif
	
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->Render(pCamera);
	}
}

void CNexusAndTowerHPGaugeShader::GetCamera(CCamera * pCamera)
{
	m_pCamera = pCamera;

	for (int i = 0; i < m_nObjects; ++i) {
		static_cast<CHPGaugeObjects*>(m_ppObjects[i])->SetCamera(m_pCamera);
	}
}

bool CNexusAndTowerHPGaugeShader::OnProcessKeyInput(UCHAR * pKeyBuffer)
{
	return false;
}

bool CNexusAndTowerHPGaugeShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	return false;
}

////////////////////////////////////////////////////////////////////////
//
D3D12_INPUT_LAYOUT_DESC CNexusAndTowerHPGaugeShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pd3dInputElementDescs[1] = {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_BLEND_DESC CNexusAndTowerHPGaugeShader::CreateBlendState()
{
	D3D12_BLEND_DESC blendDesc;
	::ZeroMemory(&blendDesc, sizeof(D3D12_BLEND_DESC));

	blendDesc.AlphaToCoverageEnable = TRUE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(blendDesc);
}

D3D12_SHADER_BYTECODE CNexusAndTowerHPGaugeShader::CreateVertexShader(ID3DBlob ** ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTexturedGauge",
		"vs_5_1",
		ppShaderBlob));
}

D3D12_SHADER_BYTECODE CNexusAndTowerHPGaugeShader::CreatePixelShader(ID3DBlob ** ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSTexturedGauge",
		"ps_5_1",
		ppShaderBlob));
}

void CNexusAndTowerHPGaugeShader::CreateShader(CCreateMgr * pCreateMgr, UINT nRenderTargets)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets);
}

void CNexusAndTowerHPGaugeShader::CreateShaderVariables(CCreateMgr * pCreateMgr, int nBuffers)
{
	HRESULT hResult;
	UINT elementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	m_pConstBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		elementBytes * nBuffers,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	hResult = m_pConstBuffer->Map(0, NULL, (void **)&m_pMappedObjects);
	assert(SUCCEEDED(hResult) && "m_pConstBuffer->Map Failed");
}

void CNexusAndTowerHPGaugeShader::BuildObjects(CCreateMgr * pCreateMgr, void * pContext)
{
	m_pCamera = (CCamera*)pContext;

	m_nObjects = m_nNexusAndTower;
	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 1);
	CreateShaderVariables(pCreateMgr, m_nObjects);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer, ncbElementBytes);

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CHPGaugeObjects *pGaugeObject = NULL;

	for (int i = 0; i < m_nObjects; ++i) {
		pGaugeObject = new CHPGaugeObjects(pCreateMgr, GagueUIType::NexusAndTower);
		
		pGaugeObject->SetMaterial(Materials::CreateRedGaugeMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]));
		pGaugeObject->SetCamera(m_pCamera);
		
		pGaugeObject->SetObject(m_ppNexusAndTower[i]);
		
		XMFLOAT3 HPGaugePosition = m_ppNexusAndTower[i]->GetPosition();
		HPGaugePosition.y += 200.f;
		pGaugeObject->SetPosition(HPGaugePosition);

		pGaugeObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));

		m_ppObjects[i] = pGaugeObject;
	}
}

void CNexusAndTowerHPGaugeShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			delete m_ppObjects[j];
		}
		Safe_Delete_Array(m_ppObjects);
	}

#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i < m_nMaterials; ++i)
		{
			if (m_ppMaterials[i]) delete m_ppMaterials[i];
		}
		Safe_Delete(m_ppMaterials);
	}
#endif
}

