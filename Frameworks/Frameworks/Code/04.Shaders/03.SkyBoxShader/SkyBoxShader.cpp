#include "stdafx.h"
#include "SkyBoxShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"

/// <summary>
/// 목적: 스카이 박스 처리용 Shader
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-04-11
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CSkyBoxShader::CSkyBoxShader(CCreateMgr *pCreateMgr) : CShader(pCreateMgr)
{
}

CSkyBoxShader::~CSkyBoxShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CSkyBoxShader::ReleaseUploadBuffers()
{
	if (m_pSkyBox)  m_pSkyBox->ReleaseUploadBuffers();
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
}

void CSkyBoxShader::UpdateShaderVariables()
{
	if (m_pMappedSkyBox)
	{
		XMStoreFloat4x4(&m_pMappedSkyBox->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_pSkyBox->GetWorldMatrix())));
	}
}

void CSkyBoxShader::Render(CCamera * pCamera)
{
	XMFLOAT3 xmf3CameraPos = pCamera->GetPosition();
	m_pSkyBox->SetPosition(xmf3CameraPos.x, xmf3CameraPos.y, xmf3CameraPos.z);

	CShader::Render(pCamera);

	if (m_pMaterial)
	{
		m_pMaterial->Render(pCamera);
		m_pMaterial->UpdateShaderVariables();
	}
	if (m_pSkyBox) m_pSkyBox->Render(pCamera);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CSkyBoxShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pInputElementDescs[0] = {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[1] = {
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

D3D12_DEPTH_STENCIL_DESC CSkyBoxShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc;
	depthStencilDesc.DepthEnable = FALSE;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_NEVER;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = 0xff;
	depthStencilDesc.StencilWriteMask = 0xff;
	depthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;
	depthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_ALWAYS;

	return(depthStencilDesc);
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreateVertexShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTextured",
		"vs_5_1",
		ppShaderBlob));
}

D3D12_SHADER_BYTECODE CSkyBoxShader::CreatePixelShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSTextured",
		"ps_5_1",
		ppShaderBlob));
}

void CSkyBoxShader::CreateShader(CCreateMgr *pCreateMgr)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr);
}

void CSkyBoxShader::CreateShaderVariables(CCreateMgr * pCreateMgr)
{
	UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	m_pConstBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		elementBytes,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	HRESULT hResult = m_pConstBuffer->Map(0, NULL, (void **)&m_pMappedSkyBox);
	assert(SUCCEEDED(hResult) && "m_pConstBuffer->Map Failed");
}

void CSkyBoxShader::BuildObjects(CCreateMgr * pCreateMgr, void * pContext)
{
	m_pSkyBox = new CSkyBox(pCreateMgr);

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 1, 6);
	CreateShaderVariables(pCreateMgr);
	CreateConstantBufferViews(pCreateMgr, 1, m_pConstBuffer, ncbElementBytes);

	m_pSkyBox->SetMaterial(Materials::CreateSkyBoxMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]));
	m_pSkyBox->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr);
}

void CSkyBoxShader::ReleaseObjects()
{
	Safe_Delete(m_pSkyBox);
	Safe_Delete(m_pMaterial);
}
