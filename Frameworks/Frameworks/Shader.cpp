#include "stdafx.h"
#include "Shader.h"
#include "CreateMgr.h"
#include "Camera.h"

/// <summary>
/// 목적: 기본 쉐이터 코드, 인터페이스 용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-01-23
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
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
// 공개 함수
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

////////////////////////////////////////////////////////////////////////
// 내부 함수
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

D3D12_SHADER_BYTECODE CShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

D3D12_SHADER_BYTECODE CShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	D3D12_SHADER_BYTECODE d3dShaderByteCode;
	d3dShaderByteCode.BytecodeLength = 0;
	d3dShaderByteCode.pShaderBytecode = NULL;

	return(d3dShaderByteCode);
}

void CShader::CreateShader(CCreateMgr *pCreateMgr)
{
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

	Safe_Release(pVertexShaderBlob);
	Safe_Release(pPixelShaderBlob);

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
}

void CShader::ReleaseObjects()
{
}

void CShader::OnPrepareRender()
{
	//파이프라인에 그래픽스 상태 객체를 설정한다.
	m_pCommandList->SetPipelineState(m_ppPipelineStates[0]);
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