#include "stdafx.h"
#include "Shader.h"
#include "CreateMgr.h"

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CShader::CShader(CCreateMgr *pCreateMgr)
{
	m_pCommandList = pCreateMgr->GetCommandList();
}

CShader::~CShader()
{
	if (m_ppPipelineStates)
	{
		for (int i = 0; i < m_nPipelineStates; i++)
		{
			if (m_ppPipelineStates[i])
			{
				m_ppPipelineStates[i]->Release();
			}
		}
		delete[] m_ppPipelineStates;
	}
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
D3D12_INPUT_LAYOUT_DESC CShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pInputElementDescs 
		= new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pInputElementDescs[0] = { 
		"POSITION", 
		0, 
		DXGI_FORMAT_R32G32B32_FLOAT, 
		0, 
		0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 
		0 };

	pInputElementDescs[1] = { 
		"COLOR", 
		0, 
		DXGI_FORMAT_R32G32B32A32_FLOAT, 
		0, 
		12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 
		0 };

	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc;
	inputLayoutDesc.pInputElementDescs = pInputElementDescs;
	inputLayoutDesc.NumElements = nInputElementDescs;

	return(inputLayoutDesc);
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

D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CompileShaderFromFile(L"Shaders.hlsl", "VSMain", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CompileShaderFromFile(L"Shaders.hlsl", "PSMain", "ps_5_1", ppd3dShaderBlob));
}

void CShader::CreateShader(CCreateMgr *pCreateMgr)
{
	//그래픽스 파이프라인 상태 객체 배열을 생성한다.
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	ID3DBlob *pVertexShaderBlob = NULL, *pPixelShaderBlob = NULL;

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

	HRESULT hResult = pCreateMgr->GetDevice()->CreateGraphicsPipelineState(&pipelineStateDesc,
		__uuidof(ID3D12PipelineState), (void **)&m_ppPipelineStates[0]);

	if (pVertexShaderBlob) pVertexShaderBlob->Release();
	if (pPixelShaderBlob) pPixelShaderBlob->Release();

	if (pipelineStateDesc.InputLayout.pInputElementDescs)
	{
		delete[] pipelineStateDesc.InputLayout.pInputElementDescs;
	}
}

void CShader::ReleaseUploadBuffers()
{
	if (!m_ppObjects) return;

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->ReleaseUploadBuffers();
		}
	}
}

void CShader::BuildObjects(CCreateMgr *pCreateMgr, void *pContext)
{
	CTriangleMesh *pTriangleMesh = new CTriangleMesh(pCreateMgr);

	m_nObjects = 1;
	m_ppObjects = new CBaseObject*[m_nObjects];

	m_ppObjects[0] = new CBaseObject();
	m_ppObjects[0]->SetMesh(pTriangleMesh);
}

void CShader::ReleaseObjects()
{
	if (!m_ppObjects) return;

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) 
		{
			delete m_ppObjects[j];
		}
	}
	delete[] m_ppObjects;
}

void CShader::AnimateObjects(float fTimeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(fTimeElapsed);
	}
}

void CShader::OnPrepareRender()
{
	//파이프라인에 그래픽스 상태 객체를 설정한다.
	m_pCommandList->SetPipelineState(m_ppPipelineStates[0]);
}

void CShader::Render()
{
	OnPrepareRender();

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->Render();
	}
}

D3D12_SHADER_BYTECODE CShader::CompileShaderFromFile(
	WCHAR *pszFileName, LPCSTR pszShaderName,
	LPCSTR pszShaderProfile, ID3DBlob **ppShaderBlob)
{
	UINT nCompileFlags = 0;

#if defined(_DEBUG)
	nCompileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	::D3DCompileFromFile(pszFileName, NULL, NULL, pszShaderName, pszShaderProfile,
		nCompileFlags, 0, ppShaderBlob, NULL);

	D3D12_SHADER_BYTECODE shaderByteCode;
	shaderByteCode.BytecodeLength = (*ppShaderBlob)->GetBufferSize();
	shaderByteCode.pShaderBytecode = (*ppShaderBlob)->GetBufferPointer();

	return(shaderByteCode);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수