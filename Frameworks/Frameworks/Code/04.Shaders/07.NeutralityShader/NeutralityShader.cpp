#include "stdafx.h"
#include "NeutralityShader.h"
#include "05.Objects/06.Minion/Minion.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
#include "05.Objects/99.Material/Material.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"
#include "00.Global/01.Utility/06.HPGaugeManager/HPGaugeManager.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"
#include "00.Global/02.AI/00.FSMMgr/FSMMgr.h"

/// <summary>
/// 목적: 중립 몬스터 관리 및 렌더링용 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-07-19
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CNeutralityShader::CNeutralityShader(shared_ptr<CCreateMgr> pCreateMgr) : CShader(pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;
}

CNeutralityShader::~CNeutralityShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CNeutralityShader::Initialize(shared_ptr<CCreateMgr> pCreateMgr, void *pContext)
{
	CreateShader(pCreateMgr, RENDER_TARGET_BUFFER_CNT, true, true);
	BuildObjects(pCreateMgr, pContext);
}

void CNeutralityShader::ReleaseUploadBuffers()
{
#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
#endif
}

void CNeutralityShader::UpdateShaderVariables(int opt)
{

}

void CNeutralityShader::UpdateBoundingBoxShaderVariables()
{

}

void CNeutralityShader::AnimateObjects(float timeElapsed)
{

}

void CNeutralityShader::Render(CCamera *pCamera)
{

}

void CNeutralityShader::RenderBoundingBox(CCamera * pCamera)
{
	CShader::RenderBoundingBox(pCamera);
}

void CNeutralityShader::RenderShadow(CCamera * pCamera)
{

}

bool CNeutralityShader::OnProcessKeyInput(UCHAR* pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	return true;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CNeutralityShader::CreateInputLayout()
{
	UINT nInputElementDescs = 6;
	D3D12_INPUT_ELEMENT_DESC *pInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pInputElementDescs[0] = {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[1] = {
		"NORMAL",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[2] = {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	pInputElementDescs[3] = {
		"WEIGHTS",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[4] = {
		"BONEINDICES",
		0,
		DXGI_FORMAT_R8G8B8A8_UINT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	pInputElementDescs[5] = {
		"TANGENT",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CNeutralityShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", 
		"VSBone",
		"vs_5_1", 
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CNeutralityShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", 
		"PSBone",
		"ps_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CNeutralityShader::CreateShadowVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/ShadowShader.hlsl",
		"VSBone",
		"vs_5_1",
		pShaderBlob));
}

void CNeutralityShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 2;

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CNeutralityShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;
}

void CNeutralityShader::ReleaseObjects()
{
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
