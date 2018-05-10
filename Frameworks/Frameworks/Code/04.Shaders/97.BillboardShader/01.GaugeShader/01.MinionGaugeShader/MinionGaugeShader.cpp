#include "stdafx.h"
#include "MinionGaugeShader.h"
#include "05.Objects/95.Billboard/Billboard.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/06.Minion/Minion.h"
#include "05.Objects/99.Material/Material.h"
#include "00.Global/01.Utility/06.HPGaugeManager/HPGaugeManager.h"

/// <summary>
/// 목적: Billboard 테스트 쉐이더
/// 최종 수정자:  이용선
/// 수정자 목록:  이용선
/// 최종 수정 날짜: 2018-04-16
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CMinionHPGaugeShader::CMinionHPGaugeShader(CCreateMgr *pCreateMgr)
	: CShader(pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;
}

CMinionHPGaugeShader::~CMinionHPGaugeShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CMinionHPGaugeShader::ReleaseUploadBuffers()
{
	for (auto& iter = m_GaugeObjectList.begin(); iter != m_GaugeObjectList.end(); ++iter) {
		(*iter)->ReleaseUploadBuffers();
	}

#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
#endif
}

void CMinionHPGaugeShader::UpdateShaderVariables()
{
	static UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (auto& iter = m_GaugeObjectList.begin(); iter != m_GaugeObjectList.end(); ++iter) {
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects + ((*iter)->GetIndex() * elementBytes));
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4((*iter)->GetWorldMatrix())));
	}
}

void CMinionHPGaugeShader::AnimateObjects(float timeElapsed)
{
	for (auto& iter = m_GaugeObjectList.begin(); iter != m_GaugeObjectList.end(); ++iter) {
		(*iter)->Animate(timeElapsed);
	}

	for (int i = 0; i < m_pGaugeManger->GetCount(); ++i) {
		SpawnGauge();

		if (i == (m_pGaugeManger->GetCount()) - 1) m_pGaugeManger->ResetCount();
	}
}

void CMinionHPGaugeShader::Render(CCamera *pCamera)
{
	CShader::Render(pCamera);
	if (m_ppMaterials) m_ppMaterials[0]->UpdateShaderVariables();

	for (auto& iter = m_GaugeObjectList.begin(); iter != m_GaugeObjectList.end(); ++iter) {
		(*iter)->Render(pCamera);
	}
}

void CMinionHPGaugeShader::GetCamera(CCamera * pCamera)
{
	m_pCamera = pCamera;

	for (auto& iter = m_GaugeObjectList.begin(); iter != m_GaugeObjectList.end(); ++iter) {
		static_cast<CHPGaugeObjects*>(*iter)->SetCamera(m_pCamera);
	}
}

bool CMinionHPGaugeShader::OnProcessKeyInput(UCHAR * pKeyBuffer)
{

	return false;
}

bool CMinionHPGaugeShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	return false;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CMinionHPGaugeShader::CreateInputLayout()
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

D3D12_SHADER_BYTECODE CMinionHPGaugeShader::CreateVertexShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTextured",
		"vs_5_1",
		ppShaderBlob));
}

D3D12_SHADER_BYTECODE CMinionHPGaugeShader::CreatePixelShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSTextured",
		"ps_5_1",
		ppShaderBlob));
}

void CMinionHPGaugeShader::CreateShader(CCreateMgr *pCreateMgr)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr);
}

void CMinionHPGaugeShader::CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers)
{
	HRESULT hResult;

	UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	m_pConstBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		elementBytes * nBuffers,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	hResult = m_pConstBuffer->Map(0, NULL, (void **)&m_pMappedObjects);
	assert(SUCCEEDED(hResult) && "m_pConstBuffer->Map Failed");
}

void CMinionHPGaugeShader::BuildObjects(CCreateMgr *pCreateMgr, void *pContext)
{
	m_pCamera = (CCamera*)pContext;

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateShaderVariables(pCreateMgr, MAX_MINION);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, MAX_MINION, 1);

	CreateConstantBufferViews(pCreateMgr, MAX_MINION, m_pConstBuffer, ncbElementBytes);

#if USE_BATCH_MATERIAL
	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];

	m_ppMaterials[0] = Materials::CreateGreyMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
#else
	CMaterial *pCubeMaterial = Materials::CreateGreyMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle[0], &m_srvGPUDescriptorStartHandle[0]);
#endif

}

int CMinionHPGaugeShader::GetPossibleIndex()
{
	for (int idx = 0; idx < MAX_MINION; ++idx)
	{
		if (!m_indexArr[idx])
		{
			m_indexArr[idx] = true;
			return idx;
		}
	}
	return NONE;
}

void CMinionHPGaugeShader::SpawnGauge()
{
	m_MinionObjectList = m_pGaugeManger->GetMinionObjectList();

	static UINT incrementSize{ m_pCreateMgr->GetCbvSrvDescriptorIncrementSize() };

	m_pCreateMgr->ResetCommandList();

	CHPGaugeObjects *pGaugeObject{ NULL };
	CBaseObject *pMinionObjects{ NULL };

	pGaugeObject = new CHPGaugeObjects(m_pCreateMgr);
	pMinionObjects = m_MinionObjectList->back();

	pGaugeObject->SetObject(pMinionObjects);
	pGaugeObject->SetMaterial(m_ppMaterials[0]);
	pGaugeObject->SetCamera(m_pCamera);

	int index = GetPossibleIndex();

	pGaugeObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * index));

	pGaugeObject->SaveIndex(index);

	XMFLOAT3 xmfGaugePosition;
	xmfGaugePosition = pMinionObjects->GetPosition();
	xmfGaugePosition.y += 70.f;
	pGaugeObject->SetPosition(xmfGaugePosition);
	m_GaugeObjectList.emplace_back(pGaugeObject);

	m_pCreateMgr->ExecuteCommandList();
}

void CMinionHPGaugeShader::ReleaseObjects()
{
	for (auto& iter = m_GaugeObjectList.begin(); iter != m_GaugeObjectList.end();)
	{
		iter = m_GaugeObjectList.erase(iter);
	}
	m_GaugeObjectList.clear();

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