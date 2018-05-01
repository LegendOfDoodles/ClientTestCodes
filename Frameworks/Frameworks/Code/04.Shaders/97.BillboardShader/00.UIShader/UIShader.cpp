#include "stdafx.h"
#include "UIShader.h"
#include "05.Objects/95.Billboard/Billboard.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"

/// <summary>
/// 목적: UI MiniMap 테스트 쉐이더
/// 최종 수정자:  이용선
/// 수정자 목록:  이용선
/// 최종 수정 날짜: 2018-04-24
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CUIObjectShader::CUIObjectShader(CCreateMgr * pCreateMgr)
	: CShader(pCreateMgr)
{

}

CUIObjectShader::~CUIObjectShader()
{
}

////////////////////////////////////////////////////////////////////////
//
void CUIObjectShader::ReleaseUploadBuffers()
{
	if (!m_ppObjects) return;

	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->ReleaseUploadBuffers();
#if USE_BATCH_MATERIAL
		if (m_ppMaterials[j]) m_ppMaterials[j]->ReleaseUploadBuffers();
#endif
	}

}

void CUIObjectShader::UpdateShaderVariables()
{
	static UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CUIObjectShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CUIObjectShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
#if USE_BATCH_MATERIAL
		if (m_ppMaterials[j]) m_ppMaterials[j]->UpdateShaderVariables();
#endif
	
		if (j == 3 && OnOFF) 
			m_ppObjects[j]->Render(pCamera);
		else if (j != 3) m_ppObjects[j]->Render(pCamera);
	}
}

bool CUIObjectShader::OnProcessKeyInput(UCHAR * pKeyBuffer)
{
	return false;
}

bool CUIObjectShader::OnProcessMouseInput(UCHAR * pKeyBuffer)
{



	return false;
}

////////////////////////////////////////////////////////////////////////
// 내부함수
D3D12_INPUT_LAYOUT_DESC CUIObjectShader::CreateInputLayout()
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

D3D12_BLEND_DESC CUIObjectShader::CreateBlendState()
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

D3D12_SHADER_BYTECODE CUIObjectShader::CreateVertexShader(ID3DBlob ** ppShaderBlob)
{
	//./Code/04.Shaders/99.GraphicsShader/
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTextured",
		"vs_5_1",
		ppShaderBlob));
}

D3D12_SHADER_BYTECODE CUIObjectShader::CreatePixelShader(ID3DBlob ** ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSTextured",
		"ps_5_1",
		ppShaderBlob));
}

void CUIObjectShader::CreateShader(CCreateMgr * pCreateMgr)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr);
}

void CUIObjectShader::CreateShaderVariables(CCreateMgr * pCreateMgr)
{
	HRESULT hResult;

	UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	m_pConstBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		elementBytes * m_nObjects,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	hResult = m_pConstBuffer->Map(0, NULL, (void **)&m_pMappedObjects);
	assert(SUCCEEDED(hResult) && "m_pConstBuffer->Map Failed");
}

void CUIObjectShader::BuildObjects(CCreateMgr * pCreateMgr, void * pContext)
{
	CCamera *pCamera = (CCamera*)pContext;
	
	m_nObjects = 4;
	m_ppObjects = new CBaseObject*[m_nObjects];
	m_ppMaterials = new CMaterial*[m_nObjects];

	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE_2D, 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/grey.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 1);
	CreateShaderVariables(pCreateMgr);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer, ncbElementBytes);

	CreateShaderResourceViews(pCreateMgr, pTexture, 3, false);
	
	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CUIObject *pMiniMap = NULL;
	CMaterial *pMaterial = NULL;

	for (int i = 0; i < m_nObjects; ++i)
	{
#if USE_BATCH_MATERIAL
		pMaterial = new CMaterial(pCreateMgr);
		pMaterial->Initialize(pCreateMgr);
		pMaterial->SetTexture(pTexture);
		m_ppMaterials[i] = pMaterial;
#endif
		pMiniMap = new CUIObject(pCreateMgr, (Type)i);
		pMiniMap->SetCamera(pCamera);
		pMiniMap->SetDistance(10.f);
		pMiniMap->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));
		m_ppObjects[i] = pMiniMap;
	}
}

void CUIObjectShader::ReleaseShaderVariables()
{
	if (!m_pConstBuffer) return;

	m_pConstBuffer->Unmap(0, NULL);
	Safe_Release(m_pConstBuffer);

	CShader::ReleaseShaderVariables();
}

void CUIObjectShader::ReleaseObjects()
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
