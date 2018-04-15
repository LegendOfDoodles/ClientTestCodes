#include "stdafx.h"
#include "BillboardShader.h"
#include "05.Objects/05.Billboard/Billboard.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"

/// <summary>
/// 목적: Billboard 테스트 쉐이더
/// 최종 수정자:  이용선
/// 수정자 목록:  이용선
/// 최종 수정 날짜: 2018-04-15
/// </summary>

////////////////////////////////////////////////////////////////////////

CBillboardShader::CBillboardShader(CCreateMgr *pCreateMgr)
	:CShader(pCreateMgr)
{
}

CBillboardShader::~CBillboardShader()
{
}

////////////////////////////////////////////////////////////////////////

void CBillboardShader::ReleaseUploadBuffers()
{
	if (!m_pBillboard) return;

	m_pBillboard->ReleaseUploadBuffers();
#if USE_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CBillboardShader::UpdateShaderVariables()
{
	XMStoreFloat4x4(&m_pMappedBillboard.m_xmf4x4World,
		XMMatrixTranspose(XMLoadFloat4x4(m_pBillboard->GetWorldMatrix())));
}

void CBillboardShader::AnimateObjects(float timeElapsed)
{
	//m_pBillboard->Animate(timeElapsed);
}

void CBillboardShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera);
	if (m_pMaterial)
	{
		m_pMaterial->Render(pCamera);
		m_pMaterial->UpdateShaderVariables();
	}
	if (m_pBillboard) m_pBillboard->Render(pCamera);
}

////////////////////////////////////////////////////////////////////////
D3D12_INPUT_LAYOUT_DESC CBillboardShader::CreateInputLayout()
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

D3D12_SHADER_BYTECODE CBillboardShader::CreateVertexShader(ID3DBlob ** ppShaderBlob)
{
	//./Code/04.Shaders/99.GraphicsShader/
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", 
		"VSBillboard", 
		"vs_5_1", 
		ppShaderBlob));
}

D3D12_SHADER_BYTECODE CBillboardShader::CreatePixelShader(ID3DBlob ** ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", 
		"PSBillboard", 
		"ps_5_1", 
		ppShaderBlob));
}

void CBillboardShader::CreateShader(CCreateMgr * pCreateMgr)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pCreateMgr);
}

void CBillboardShader::CreateShaderVariables(CCreateMgr * pCreateMgr)
{
	HRESULT hResult;

	UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	m_pConstBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		elementBytes,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	hResult = m_pConstBuffer->Map(0, NULL, (void **)&m_pMappedBillboard);
	assert(SUCCEEDED(hResult) && "m_pConstBuffer->Map Failed");
}

void CBillboardShader::BuildObjects(CCreateMgr * pCreateMgr, void * pContext)
{
	CCamera *Camera = (CCamera*)pContext;
	CTexture *pTexture = new CTexture(1, RESOURCE_TEXTURE_2D, 0);
	pTexture->LoadTextureFromFile(pCreateMgr, L"./Resource/Textures/Stones.dds", 0);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 1, 1);
	CreateShaderVariables(pCreateMgr);
	CreateConstantBufferViews(pCreateMgr, 1, m_pConstBuffer, ncbElementBytes);
	CreateShaderResourceViews(pCreateMgr, pTexture, 3, false);

	m_pMaterial = new CMaterial(pCreateMgr);

	m_pMaterial->Initialize(pCreateMgr);
	m_pMaterial->SetTexture(pTexture);

	
	m_pBillboard = new CBillboardObject(pCreateMgr);
	m_pBillboard->SetCamera(Camera);
	m_pBillboard->SetMaterial(m_pMaterial);
	m_pBillboard->SetPosition(Camera->GetPosition().x, Camera->GetPosition().y, Camera->GetPosition().z + 50.f);
	m_pBillboard->SetCbvGPUDescriptorHandlePtr(m_cbvGPUDescriptorStartHandle.ptr);
}

void CBillboardShader::ReleaseShaderVariables()
{
	if (!m_pConstBuffer) return;

	m_pConstBuffer->Unmap(0, NULL);
	Safe_Release(m_pConstBuffer);

	CShader::ReleaseShaderVariables();
}

void CBillboardShader::ReleaseObjects()
{
	if (!m_pBillboard) return;

	Safe_Delete(m_pBillboard);
	Safe_Delete_Array(m_pBillboard);

#if USE_BATCH_MATERIAL
	Safe_Delete(m_pMaterial);
#endif
}
