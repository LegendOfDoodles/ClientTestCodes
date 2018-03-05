#include "stdafx.h"
#include "DiffusedShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// ����: �⺻ ������Ʈ Ŭ����, �������̽� ��
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-01-27
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CDiffusedShader::CDiffusedShader(CCreateMgr *pCreateMgr)
	: CShader(pCreateMgr)
{
}

CDiffusedShader::~CDiffusedShader()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CDiffusedShader::Initialize(CCreateMgr *pCreateMgr, void *pContext)
{
	CreateShader(pCreateMgr);
	CreateShaderVariables(pCreateMgr);
	BuildObjects(pCreateMgr, pContext);
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
D3D12_INPUT_LAYOUT_DESC CDiffusedShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new
		D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };
	pd3dInputElementDescs[1] = { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CDiffusedShader::CreateVertexShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "VSDiffused", "vs_5_1", ppShaderBlob));
}

D3D12_SHADER_BYTECODE CDiffusedShader::CreatePixelShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "PSDiffused", "ps_5_1", ppShaderBlob));
}

void CDiffusedShader::CreateShader(CCreateMgr *pCreateMgr)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pCreateMgr);
}