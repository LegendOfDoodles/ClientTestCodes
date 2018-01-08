#include "stdafx.h"
#include "Scene.h"
#include "CreateMgr.h"

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CScene::CScene()
{
	m_pPipelineState = NULL;
	m_pGraphicsRootSignature = NULL;
}


CScene::~CScene()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CScene::BuildObjects(CCreateMgr *pCreateMgr)
{
	m_pCommandList = pCreateMgr->GetCommandList();

	//루트 시그너쳐를 생성한다.
	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	::ZeroMemory(&rootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));
	rootSignatureDesc.NumParameters = 0;
	rootSignatureDesc.pParameters = NULL;
	rootSignatureDesc.NumStaticSamplers = 0;
	rootSignatureDesc.pStaticSamplers = NULL;
	rootSignatureDesc.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	ID3DBlob *pSignatureBlob = NULL;
	ID3DBlob *pErrorBlob = NULL;

	::D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1,
		&pSignatureBlob, &pErrorBlob);
	pCreateMgr->GetDevice()->CreateRootSignature(0, pSignatureBlob->GetBufferPointer(),
		pSignatureBlob->GetBufferSize(), __uuidof(ID3D12RootSignature), (void
			**)&m_pGraphicsRootSignature);
	if (pSignatureBlob) pSignatureBlob->Release();
	if (pErrorBlob) pErrorBlob->Release();

	//정점 셰이더와 픽셀 셰이더를 생성한다.
	ID3DBlob *pVertexShaderBlob = NULL;
	ID3DBlob *pPixelShaderBlob = NULL;
	UINT compileFlags = 0;

#if defined(_DEBUG)
	compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	D3DCompileFromFile(L"Shaders.hlsl", NULL, NULL, "VSMain", "vs_5_1", compileFlags, 0,
		&pVertexShaderBlob, NULL);
	D3DCompileFromFile(L"Shaders.hlsl", NULL, NULL, "PSMain", "ps_5_1", compileFlags, 0,
		&pPixelShaderBlob, NULL);

	//래스터라이저 상태를 설정한다.
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

	//블렌드 상태를 설정한다.
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

	//그래픽 파이프라인 상태를 설정한다.
	D3D12_GRAPHICS_PIPELINE_STATE_DESC PipelineStateDesc;
	::ZeroMemory(&PipelineStateDesc, sizeof(D3D12_GRAPHICS_PIPELINE_STATE_DESC));
	PipelineStateDesc.pRootSignature = m_pGraphicsRootSignature;
	PipelineStateDesc.VS.pShaderBytecode = pVertexShaderBlob->GetBufferPointer();
	PipelineStateDesc.VS.BytecodeLength = pVertexShaderBlob->GetBufferSize();
	PipelineStateDesc.PS.pShaderBytecode = pPixelShaderBlob->GetBufferPointer();
	PipelineStateDesc.PS.BytecodeLength = pPixelShaderBlob->GetBufferSize();
	PipelineStateDesc.RasterizerState = rasterizerDesc;
	PipelineStateDesc.BlendState = blendDesc;
	PipelineStateDesc.DepthStencilState.DepthEnable = FALSE;
	PipelineStateDesc.DepthStencilState.StencilEnable = FALSE;
	PipelineStateDesc.InputLayout.pInputElementDescs = NULL;
	PipelineStateDesc.InputLayout.NumElements = 0;
	PipelineStateDesc.SampleMask = UINT_MAX;
	PipelineStateDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	PipelineStateDesc.NumRenderTargets = 1;
	PipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	PipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
	PipelineStateDesc.SampleDesc.Count = 1;
	PipelineStateDesc.SampleDesc.Quality = 0;
	pCreateMgr->GetDevice()->CreateGraphicsPipelineState(&PipelineStateDesc,
		__uuidof(ID3D12PipelineState), (void **)&m_pPipelineState);

	if (pVertexShaderBlob) pVertexShaderBlob->Release();
	if (pPixelShaderBlob) pPixelShaderBlob->Release();
}

void CScene::ReleaseObjects()
{
	if (m_pGraphicsRootSignature) m_pGraphicsRootSignature->Release();
	if (m_pPipelineState) m_pPipelineState->Release();
}

void CScene::ProcessInput()
{
}

void CScene::AnimateObjects()
{
}

void CScene::Render()
{
	//그래픽 루트 시그너쳐를 설정한다.
	m_pCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature);
	//파이프라인 상태를 설정한다.
	m_pCommandList->SetPipelineState(m_pPipelineState);
	//프리미티브 토폴로지(삼각형 리스트)를 설정한다.
	m_pCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	//정점 3개를 사용하여 렌더링한다.
	m_pCommandList->DrawInstanced(3, 1, 0, 0);
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	return(false);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수