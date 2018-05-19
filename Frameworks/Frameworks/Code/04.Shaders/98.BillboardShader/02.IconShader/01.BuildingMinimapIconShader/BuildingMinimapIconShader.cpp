#include "stdafx.h"
#include "BuildingMinimapIconShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/09.NexusTower/NexusTower.h"
#include "05.Objects/99.Material/Material.h"

/// <summary>
/// 목적: 미니맵 건물 Icon 쉐이더
/// 최종 수정자:  이용선
/// 수정자 목록:  이용선
/// 최종 수정 날짜: 2018-05-13
/// </summary>

CBuildingMinimapIconShader::CBuildingMinimapIconShader(CCreateMgr *pCreateMgr)
	:CShader(pCreateMgr)
{
}

CBuildingMinimapIconShader::~CBuildingMinimapIconShader()
{
}

void CBuildingMinimapIconShader::ReleaseUploadBuffers()
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

void CBuildingMinimapIconShader::UpdateShaderVariables()
{
	static UINT elementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAUGE_INFO *pMappedObject = (CB_GAUGE_INFO *)(m_pMappedObjects + (i * elementBytes));
		pMappedObject->m_fCurrentHP = ((CMinimapIconObjects*)m_ppObjects[i])->GetCurrentHP();
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CBuildingMinimapIconShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CBuildingMinimapIconShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		CCollisionObject* master = ((CMinimapIconObjects*)m_ppObjects[j])->GetMasterObject();

		switch (master->GetType())
		{
		case ObjectType::FirstTower:
			if (master->GetTeam() == TeamType::Blue) {
				// Blue Tower
				CShader::Render(pCamera, 0);
				m_ppMaterials[0]->UpdateShaderVariables();
			}
			else {
				// Red Tower
				CShader::Render(pCamera, 1);
				m_ppMaterials[1]->UpdateShaderVariables();
			}
			break;
		case ObjectType::Nexus:
			if (master->GetTeam() == TeamType::Blue) {
				// Box
				CShader::Render(pCamera, 2);
				m_ppMaterials[2]->UpdateShaderVariables();
			}
			else {
				// Shell
				CShader::Render(pCamera, 3);
				m_ppMaterials[3]->UpdateShaderVariables();
			}
			break;
		}

		if (m_ppObjects[j]) m_ppObjects[j]->Render(pCamera);
	}
}

void CBuildingMinimapIconShader::GetCamera(CCamera * pCamera)
{
	m_pCamera = pCamera;

	for (int i = 0; i < m_nObjects; ++i)
	{
		static_cast<CMinimapIconObjects*>(m_ppObjects[i])->SetCamera(m_pCamera);
	}
}

bool CBuildingMinimapIconShader::OnProcessKeyInput(UCHAR * pKeyBuffer)
{
	return false;
}

bool CBuildingMinimapIconShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	return false;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CBuildingMinimapIconShader::CreateInputLayout()
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

D3D12_BLEND_DESC CBuildingMinimapIconShader::CreateBlendState()
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

D3D12_SHADER_BYTECODE CBuildingMinimapIconShader::CreateVertexShader(ID3DBlob ** ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTexturedGauge",
		"vs_5_1",
		ppShaderBlob));
}

D3D12_SHADER_BYTECODE CBuildingMinimapIconShader::CreatePixelShader(ID3DBlob ** ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSTexturedIconGauge",
		"ps_5_1",
		ppShaderBlob));
}

void CBuildingMinimapIconShader::CreateShader(CCreateMgr * pCreateMgr, UINT nRenderTargets)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	m_nHeaps = 4;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets);
}

void CBuildingMinimapIconShader::CreateShaderVariables(CCreateMgr * pCreateMgr, int nBuffers)
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

void CBuildingMinimapIconShader::BuildObjects(CCreateMgr * pCreateMgr, void * pContext)
{
	m_pCamera = (CCamera*)pContext;

	m_nObjects = m_nNexusAndTower;
	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	CreateShaderVariables(pCreateMgr, m_nObjects);

	for (int i = 0; i < m_nHeaps; ++i) {
		CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 1, i);
		CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer, ncbElementBytes, i);
	}
#if USE_BATCH_MATERIAL
	m_nMaterials = m_nHeaps;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreateBlueTowerIconMaterial(pCreateMgr,	&m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[1] = Materials::CreateRedTowerIconMaterial(pCreateMgr,	&m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);
	m_ppMaterials[2] = Materials::CreateBoxNexusIconMaterial(pCreateMgr,	&m_psrvCPUDescriptorStartHandle[2], &m_psrvGPUDescriptorStartHandle[2]);
	m_ppMaterials[3] = Materials::CreateShellNexusIconMaterial(pCreateMgr,	&m_psrvCPUDescriptorStartHandle[3], &m_psrvGPUDescriptorStartHandle[3]);
#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif

	// Player Icon 생성
	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CMinimapIconObjects *pIconObject = NULL;

	for (int i = 0; i < m_nObjects; ++i) {
		pIconObject = new CMinimapIconObjects(pCreateMgr, IconUIType::NexusAndTowerIcon);

		pIconObject->SetCamera(m_pCamera);
		pIconObject->SetDistance((FRAME_BUFFER_WIDTH / 128.f) - 0.04f);	// distance 9
		pIconObject->SetObject(m_ppNexusAndTower[i]);
		pIconObject->WorldToMinimap();

		pIconObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));

		m_ppObjects[i] = pIconObject;
	}
}

void CBuildingMinimapIconShader::ReleaseObjects()
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
