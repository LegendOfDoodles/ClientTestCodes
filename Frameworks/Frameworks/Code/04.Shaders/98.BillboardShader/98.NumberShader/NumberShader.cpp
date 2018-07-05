#include "stdafx.h"
#include "NumberShader.h"
#include "05.Objects/96.Billboard/01.FrameObject/UIFrameObject.h"
#include "05.Objects/96.Billboard/98.NumberObject/NumberObject.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/08.Player/Player.h"


/// <summary>
/// 목적: Number Billboard 쉐이더 (시간, Kill Dead, Assist )
/// 최종 수정자:  김나단
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-07-03
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자

CNumberShader::CNumberShader(CCreateMgr * pCreateMgr) : CShader(pCreateMgr)
{
}

CNumberShader::~CNumberShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CNumberShader::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			m_ppObjects[j]->ReleaseUploadBuffers();
		}
	}

	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
}

void CNumberShader::UpdateShaderVariables()
{
	static UINT elementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAUGE_INFO *pMappedObject = (CB_GAUGE_INFO *)(m_pMappedObjects + (i * elementBytes));
		pMappedObject->m_fCurrentHP = static_cast<float>(((CNumberOjbect*)m_ppObjects[i])->GetNum());
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CNumberShader::AnimateObjects(float timeElapsed)
{
	if (m_iTimer[0] <= 60) {
	
		m_iTimer[0] += timeElapsed;
	}
	else {
		m_iTimer[1] += 1;
		m_iTimer[0] = 0;
	}

	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CNumberShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera);

	m_ppMaterials[0]->UpdateShaderVariables();
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Render(pCamera);
	}
}

void CNumberShader::GetCamera(CCamera * pCamera)
{
	m_pCamera = pCamera;

}

void CNumberShader::PositionalNumber(int inputNum, int PositionalNumber)
{
	UNREFERENCED_PARAMETER(inputNum);
	UNREFERENCED_PARAMETER(PositionalNumber);
}

void CNumberShader::SignificantDigit(int InputNum, int PositionalNumber, int *Num)
{
	UNREFERENCED_PARAMETER(InputNum);
	UNREFERENCED_PARAMETER(PositionalNumber);
	UNREFERENCED_PARAMETER(Num);
}

////////////////////////////////////////////////////////////////////////
// 내부함수
D3D12_INPUT_LAYOUT_DESC CNumberShader::CreateInputLayout()
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

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_BLEND_DESC CNumberShader::CreateBlendState()
{
	D3D12_BLEND_DESC blendDesc;
	::ZeroMemory(&blendDesc, sizeof(D3D12_BLEND_DESC));

	blendDesc.AlphaToCoverageEnable = TRUE;
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

D3D12_SHADER_BYTECODE CNumberShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	//./Code/04.Shaders/99.GraphicsShader/
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTexturedGauge",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CNumberShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSTexturedNumber",
		"ps_5_1",
		pShaderBlob));
}

void CNumberShader::CreateShader(CCreateMgr *pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CNumberShader::CreateShaderVariables(CCreateMgr * pCreateMgr, int nBuffers)
{
	HRESULT hResult;

	UINT elementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	m_pConstBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		elementBytes * nBuffers,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	hResult = m_pConstBuffer->Map(0, NULL, (void **)&m_pMappedObjects);
	ThrowIfFailed(hResult);
}

void CNumberShader::BuildObjects(CCreateMgr * pCreateMgr, void * pContext)
{
	m_pCamera = (CCamera*)pContext;

	/* Team K */
	// 플레이어 4명을 받아와서
	for (int i = 0; i < m_nPlayer; ++i) {
		// 플레이어의 스테이터스의 Kill을 팀별로 더한다.
		if (m_ppPlayers[i]->GetTeam() == TeamType::Blue)
			m_iTeamKill[TeamKILL::BlueTeam] += m_ppPlayers[i]->GetPlayerStatus()->Kill;
		else if (m_ppPlayers[i]->GetTeam() == TeamType::Red)
			m_iTeamKill[TeamKILL::RedTeam] += m_ppPlayers[i]->GetPlayerStatus()->Kill;
	}

	for (int i = 0; i < TeamKILL::EnumCnt; ++i) {
		int checkNum = m_iTeamKill[i];		// 자리 수 확인에서 사용할 변수

		if (checkNum == 0)
			m_iTeamKillPositionalNum[i] = 1;		// 0 이면 자리수는 1개
		else
			for (m_iTeamKillPositionalNum[i] = 0; checkNum > 0; checkNum /= 10, m_iTeamKillPositionalNum[i]++);

		m_iTeamKillSignificantNum[i] = new int[m_iTeamKillPositionalNum[i]];

		// Num[0] 부터 1의 자리 10의 자리 순차적 증가 저장
		// 30이면 0, 3 저장 (출력은 반대로 해야 함)
		for (int j = 0; j < m_iTeamKillPositionalNum[i]; ++j) {
			m_iTeamKillSignificantNum[i][j] = m_iTeamKill[i] % 10;

			m_iTeamKill[i] /= 10;
		}
	}
	
	/* KDA */
	m_iPlayerKDA[0] = m_ppPlayers[0]->GetPlayerStatus()->Kill;
	m_iPlayerKDA[1] = m_ppPlayers[0]->GetPlayerStatus()->Death;
	m_iPlayerKDA[2] = m_ppPlayers[0]->GetPlayerStatus()->Assist;
	
	for (int i = 0; i < 3; ++i) {
		int checkNum = m_iPlayerKDA[i];		// 자리 수 확인에서 사용할 변수

		if (checkNum == 0)
			m_iKDAPositionalNum[i] = 1;		// 0 이면 자리수는 1개
		else
			for (m_iKDAPositionalNum[i] = 0; checkNum > 0; checkNum /= 10, m_iKDAPositionalNum[i]++);

		m_iKDASignificantNum[i] = new int[m_iKDAPositionalNum[i]];

		// Num[0] 부터 1의 자리 10의 자리 순차적 증가 저장
		// 30이면 0, 3 저장 (출력은 반대로 해야 함)
		for (int j = 0; j < m_iKDAPositionalNum[i]; ++j) {
			m_iKDASignificantNum[i][j] = m_iPlayerKDA[i] % 10;

			m_iPlayerKDA[i] /= 10;
		}
	}

	/* Timer */
	for (int i = 0; i < 2; ++i) {
		int checkNum = m_iPlayerKDA[i];		// 자리 수 확인에서 사용할 변수

		if (checkNum == 0)
			m_iKDAPositionalNum[i] = 1;		// 0 이면 자리수는 1개
		else
			for (m_iKDAPositionalNum[i] = 0; checkNum > 0; checkNum /= 10, m_iKDAPositionalNum[i]++);

		m_iKDASignificantNum[i] = new int[m_iKDAPositionalNum[i]];

		// Num[0] 부터 1의 자리 10의 자리 순차적 증가 저장
		// 30이면 0, 3 저장 (출력은 반대로 해야 함)
		for (int j = 0; j < m_iKDAPositionalNum[i]; ++j) {
			m_iKDASignificantNum[i][j] = m_iPlayerKDA[i] % 10;

			m_iPlayerKDA[i] /= 10;
		}
	}

	for (int i = 0; i < 2; ++i) m_nObjects += m_iTeamKillPositionalNum[i];
	for (int i = 0; i < 3; ++i) m_nObjects += m_iKDAPositionalNum[i];

	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 1);
	CreateShaderVariables(pCreateMgr, m_nObjects);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer, ncbElementBytes);

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CNumberOjbect *pNumber{ NULL };

	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreateNumberMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);

	int objectCnt = 0;

	for (int j = 0; j < TeamKILL::EnumCnt;++j) {
		for (int i = 0; i < m_iTeamKillPositionalNum[j]; ++i)
		{
			pNumber = new CNumberOjbect(pCreateMgr, NumberType(j));
			pNumber->SetCamera(m_pCamera);
			pNumber->SetDistance(FRAME_BUFFER_WIDTH / 128);	 // distance 10
			pNumber->SetTexCoord(m_iTeamKillSignificantNum[j][(m_iTeamKillPositionalNum[j] - 1) - i]);
			pNumber->SetOffset(i);

			pNumber->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * (objectCnt)));
			m_ppObjects[objectCnt] = pNumber;

			++objectCnt;
		}
	}

	for (int j = 0; j < 3; ++j) {
		for (int i = 0; i < m_iKDAPositionalNum[j]; ++i)
		{
			pNumber = new CNumberOjbect(pCreateMgr, NumberType(j+4));
			pNumber->SetCamera(m_pCamera);
			pNumber->SetDistance(FRAME_BUFFER_WIDTH / 128);	 // distance 10
			pNumber->SetTexCoord(m_iKDASignificantNum[j][(m_iKDAPositionalNum[j] - 1) - i]);
			pNumber->SetOffset(i);
			
			pNumber->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * (objectCnt)));
			m_ppObjects[objectCnt] = pNumber;

			++objectCnt;
		}
	}

	

}

void CNumberShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			Safe_Delete(m_ppObjects[j]);
		}
		Safe_Delete_Array(m_ppObjects);
	}

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
