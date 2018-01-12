#include "stdafx.h"
#include "ObjectShader.h"
#include "RotatingObject.h"

/// <summary>
/// 목적: 오브젝트 테스트 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-01-12
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CObjectShader::CObjectShader(CCreateMgr *pCreateMgr)
	: CShader(pCreateMgr)
{
}

CObjectShader::~CObjectShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CObjectShader::ReleaseUploadBuffers()
{
	if (!m_ppObjects) return;

	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->ReleaseUploadBuffers();
	}
}

void CObjectShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CObjectShader::Render(CCamera *pCamera)
{
	CShader::Render(pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->Render(pCamera);
		}
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CObjectShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new
		D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pd3dInputElementDescs[0] = {
		"POSITION",
		0, 
		DXGI_FORMAT_R32G32B32_FLOAT, 
		0,
		0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 
		0 };

	pd3dInputElementDescs[1] = { 
		"COLOR",
		0, 
		DXGI_FORMAT_R32G32B32A32_FLOAT,
		0, 
		12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CObjectShader::CreateVertexShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "VSDiffused", "vs_5_1", ppd3dShaderBlob));
}

D3D12_SHADER_BYTECODE CObjectShader::CreatePixelShader(ID3DBlob **ppd3dShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"Shaders.hlsl", "PSDiffused", "ps_5_1",	ppd3dShaderBlob));
}

void CObjectShader::CreateShader(CCreateMgr *pCreateMgr)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	CShader::CreateShader(pCreateMgr);
}

void CObjectShader::BuildObjects(CCreateMgr *pCreateMgr, void *pContext)
{
	CCubeMeshDiffused *pCubeMesh = new CCubeMeshDiffused(pCreateMgr, 12.0f, 12.0f, 12.0f);

	int xObjects = 10, yObjects = 10, zObjects = 10, i = 0;

	m_nObjects = (xObjects * 2 + 1) * (yObjects * 2 + 1) * (zObjects * 2 + 1);
	m_ppObjects = new CBaseObject*[m_nObjects];

	float fxPitch = 12.0f * 2.5f;
	float fyPitch = 12.0f * 2.5f;
	float fzPitch = 12.0f * 2.5f;

	CRotatingObject *pRotatingObject = NULL;
	for (int x = -xObjects; x <= xObjects; x++)
	{
		for (int y = -yObjects; y <= yObjects; y++)
		{
			for (int z = -zObjects; z <= zObjects; z++)
			{
				pRotatingObject = new CRotatingObject();
				pRotatingObject->Initialize(pCreateMgr);
				pRotatingObject->SetMesh(pCubeMesh);
				pRotatingObject->SetPosition(fxPitch*x, fyPitch*y, fzPitch*z);
				pRotatingObject->SetRotationAxis(XMFLOAT3(0.0f, 1.0f, 0.0f));
				pRotatingObject->SetRotationSpeed(10.0f*(i % 10) + 3.0f);

				m_ppObjects[i++] = pRotatingObject;
			}
		}
	}

	CreateShaderVariables(pCreateMgr);
}

void CObjectShader::ReleaseObjects()
{
	if (!m_ppObjects) return;

	for (int j = 0; j < m_nObjects; j++)
	{
		Safe_Delete(m_ppObjects[j]);
	}
	Safe_Delete_Array(m_ppObjects);
}