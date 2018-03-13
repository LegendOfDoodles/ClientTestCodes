#include "stdafx.h"
#include "Shader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"
#include "05.Objects/99.Material/00.Texture/Texture.h"

/// <summary>
/// ����: �⺻ ������ �ڵ�, �������̽� ��
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-03-07
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CShader::CShader(CCreateMgr *pCreateMgr)
{
	m_pCommandList = pCreateMgr->GetCommandList();
}

CShader::~CShader()
{
	if (!m_ppPipelineStates) return;

	for (int i = 0; i < m_nPipelineStates; i++)
	{
		Safe_Release(m_ppPipelineStates[i]);
	}

	Safe_Delete_Array(m_ppPipelineStates);
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CShader::Initialize(CCreateMgr *pCreateMgr, void *pContext)
{
	CreateShader(pCreateMgr);
	BuildObjects(pCreateMgr, pContext);
}

void CShader::Finalize()
{
	ReleaseShaderVariables();
	ReleaseObjects();
	Release();
}

void CShader::ReleaseUploadBuffers()
{
}

void CShader::UpdateShaderVariables()
{
}

void CShader::UpdateShaderVariable(XMFLOAT4X4 *pxmf4x4World)
{
}

void CShader::AnimateObjects(float timeElapsed)
{
}

void CShader::Render(CCamera *pCamera)
{
	OnPrepareRender();
}

void CShader::OnProcessKeyUp(WPARAM wParam)
{
}

void CShader::OnProcessKeyDown(WPARAM wParam)
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = NULL;
	d3dInputLayoutDesc.NumElements = 0;

	return(d3dInputLayoutDesc);
}

D3D12_RASTERIZER_DESC CShader::CreateRasterizerState()
{
	D3D12_RASTERIZER_DESC rasterizerDesc;
	::ZeroMemory(&rasterizerDesc, sizeof(D3D12_RASTERIZER_DESC));

	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	rasterizerDesc.FrontCounterClockwise = FALSE;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;
	rasterizerDesc.DepthClipEnable = TRUE;
	rasterizerDesc.MultisampleEnable = FALSE;
	rasterizerDesc.AntialiasedLineEnable = FALSE;
	rasterizerDesc.ForcedSampleCount = 0;
	rasterizerDesc.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;

	return(rasterizerDesc);
}

D3D12_BLEND_DESC CShader::CreateBlendState()
{
	D3D12_BLEND_DESC blendDesc;
	::ZeroMemory(&blendDesc, sizeof(D3D12_BLEND_DESC));

	blendDesc.AlphaToCoverageEnable = FALSE;
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

D3D12_DEPTH_STENCIL_DESC CShader::CreateDepthStencilState()
{
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc;
	::ZeroMemory(&depthStencilDesc, sizeof(D3D12_DEPTH_STENCIL_DESC));

	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS;
	depthStencilDesc.StencilEnable = FALSE;
	depthStencilDesc.StencilReadMask = 0x00;
	depthStencilDesc.StencilWriteMask = 0x00;
	depthStencilDesc.FrontFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;
	depthStencilDesc.BackFace.StencilFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilPassOp = D3D12_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D12_COMPARISON_FUNC_NEVER;

	return(depthStencilDesc);
}

void CShader::CreateCbvAndSrvDescriptorHeaps(CCreateMgr *pCreateMgr, int nConstantBufferViews, int nShaderResourceViews)
{
	UINT incrementSize = pCreateMgr->GetCbvSrvDescriptorIncrementSize();
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;
	descriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descriptorHeapDesc.NodeMask = 0;
	pCreateMgr->GetDevice()->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_pCbvSrvDescriptorHeap));

	m_cbvCPUDescriptorStartHandle = m_pCbvSrvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_cbvGPUDescriptorStartHandle = m_pCbvSrvDescriptorHeap->GetGPUDescriptorHandleForHeapStart();
	m_srvCPUDescriptorStartHandle.ptr = m_cbvCPUDescriptorStartHandle.ptr + (incrementSize * nConstantBufferViews);
	m_srvGPUDescriptorStartHandle.ptr = m_cbvGPUDescriptorStartHandle.ptr + (incrementSize * nConstantBufferViews);
}

void CShader::CreateConstantBufferViews(
	CCreateMgr *pCreateMgr, int nConstantBufferViews, ID3D12Resource *pConstantBuffers, UINT nStride)
{
	UINT incrementSize = pCreateMgr->GetCbvSrvDescriptorIncrementSize();
	D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress = pConstantBuffers->GetGPUVirtualAddress();
	D3D12_CONSTANT_BUFFER_VIEW_DESC CBVDesc;
	CBVDesc.SizeInBytes = nStride;
	for (int j = 0; j < nConstantBufferViews; j++)
	{
		CBVDesc.BufferLocation = gpuVirtualAddress + (nStride * j);
		D3D12_CPU_DESCRIPTOR_HANDLE cbvCPUDescriptorHandle;
		cbvCPUDescriptorHandle.ptr = m_cbvCPUDescriptorStartHandle.ptr + (incrementSize * j);
		pCreateMgr->GetDevice()->CreateConstantBufferView(&CBVDesc, cbvCPUDescriptorHandle);
	}
}

void GetShaderResourceViewDesc(
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

void CShader::CreateShaderResourceViews(
	CCreateMgr *pCreateMgr, CTexture *pTexture, 
	UINT nRootParameterStartIndex, bool bAutoIncrement)
{
	UINT incrementSize = pCreateMgr->GetCbvSrvDescriptorIncrementSize();
	D3D12_CPU_DESCRIPTOR_HANDLE srvCPUDescriptorHandle = m_srvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE srvGPUDescriptorHandle = m_srvGPUDescriptorStartHandle;
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

D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE shaderByteCode;
	shaderByteCode.BytecodeLength = 0;
	shaderByteCode.pShaderBytecode = NULL;

	return(shaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob **ppShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

void CShader::CreateShader(CCreateMgr *pCreateMgr)
{
	ComPtr<ID3DBlob> pVertexShaderBlob{ NULL }, pPixelShaderBlob{ NULL };

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pipelineStateDesc;
	::ZeroMemory(&pipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));

	pipelineStateDesc.pRootSignature = pCreateMgr->GetGraphicsRootSignature();
	pipelineStateDesc.VS = CreateVertexShader(&pVertexShaderBlob);
	pipelineStateDesc.PS = CreatePixelShader(&pPixelShaderBlob);
	pipelineStateDesc.RasterizerState = CreateRasterizerState();
	pipelineStateDesc.BlendState = CreateBlendState();
	pipelineStateDesc.DepthStencilState = CreateDepthStencilState();
	pipelineStateDesc.InputLayout = CreateInputLayout();
	pipelineStateDesc.SampleMask = UINT_MAX;
	pipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	pipelineStateDesc.NumRenderTargets = 1;
	pipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	pipelineStateDesc.SampleDesc.Count = 1;
	pipelineStateDesc.Flags = D3D12_PIPELINE_STATE_FLAG_NONE;

	HRESULT hResult = pCreateMgr->GetDevice()->CreateGraphicsPipelineState(
		&pipelineStateDesc,
		IID_PPV_ARGS(&m_ppPipelineStates[0]));
	assert(SUCCEEDED(hResult) && "Device->CreateGraphicsPipelineState Failed");

	Safe_Delete_Array(pipelineStateDesc.InputLayout.pInputElementDescs);
}

void CShader::CreateShaderVariables(CCreateMgr *pCreateMgr)
{
}

void CShader::BuildObjects(CCreateMgr *pCreateMgr, void *pContext)
{
}

void CShader::ReleaseShaderVariables()
{
	if (!m_pCbvSrvDescriptorHeap) return;
	Safe_Release(m_pCbvSrvDescriptorHeap);
}

void CShader::ReleaseObjects()
{
}

void CShader::OnPrepareRender()
{
	//���������ο� �׷��Ƚ� ���� ��ü�� �����Ѵ�.
	m_pCommandList->SetPipelineState(m_ppPipelineStates[0]);
	m_pCommandList->SetDescriptorHeaps(1, &m_pCbvSrvDescriptorHeap);

	UpdateShaderVariables();
}

D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(
	WCHAR *pszFileName, LPCSTR pszShaderName,
	LPCSTR pszShaderProfile, ID3DBlob **ppShaderBlob)
{
	HRESULT hResult;
	UINT nCompileFlags{ 0 };

#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	ComPtr<ID3DBlob> pErrorBlob{ NULL };
	hResult = D3DCompileFromFile(
		  pszFileName
		, NULL
		, D3D_COMPILE_STANDARD_FILE_INCLUDE
		, pszShaderName
		, pszShaderProfile
		, nCompileFlags
		, 0
		, ppShaderBlob
		, &pErrorBlob);
	assert(SUCCEEDED(hResult) && "D3DCompileFromFile Failed");
	//ExptProcess::PrintErrorBlob(pErrorBlob);

	D3D12_SHADER_BYTECODE shaderByteCode;
	shaderByteCode.BytecodeLength = (*ppShaderBlob)->GetBufferSize();
	shaderByteCode.pShaderBytecode = (*ppShaderBlob)->GetBufferPointer();

	return(shaderByteCode);
}