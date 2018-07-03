#include "stdafx.h"
#include "SkillShader.h"
#include "05.Objects/96.Billboard/04.SkillObject/SkillObject.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/08.Player/Player.h"

/// <summary>
/// 목적: Skill UI Icon Shader (틀만 생성)
/// 최종 수정자:  김나단
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-07-03
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CSkillShader::CSkillShader(CCreateMgr * pCreateMgr)
	: CShader(pCreateMgr)
{
}

CSkillShader::~CSkillShader()
{
}

////////////////////////////////////////////////////////////////////////
//
void CSkillShader::ReleaseUploadBuffers()
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

void CSkillShader::UpdateShaderVariables()
{
	static UINT elementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAUGE_INFO *pMappedObject = (CB_GAUGE_INFO *)(m_pMappedObjects + (i * elementBytes));
		if (i >= 4 ) pMappedObject->m_fCurrentHP = 1.0f;	// Color
		else		 pMappedObject->m_fCurrentHP = 1.0f;	// Grey
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CSkillShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CSkillShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		SkillUIType type = (SkillUIType)((CSkillObject*)m_ppObjects[j])->GetType();

		switch (type)
		{
		case GreyQSkill:
			CShader::Render(pCamera, 0);
			m_ppMaterials[0]->UpdateShaderVariable(0);
			break;
		case GreyWSkill:
			m_ppMaterials[0]->UpdateShaderVariable(1);
			break;
		case GreyESkill:
			m_ppMaterials[0]->UpdateShaderVariable(2);
			break;
		case GreyRSkill:
			m_ppMaterials[0]->UpdateShaderVariable(3);
			break;
		case QSkill:
			CShader::Render(pCamera, 1);
			m_ppMaterials[1]->UpdateShaderVariable(0);
			break;
		case WSkill:
			m_ppMaterials[1]->UpdateShaderVariable(1);
			break;
		case ESkill:
			m_ppMaterials[1]->UpdateShaderVariable(2);
			break;
		case RSkill:
			m_ppMaterials[1]->UpdateShaderVariable(3);
			break;
		default:
			break;
		}

		m_ppObjects[j]->Render(pCamera);
	}
}

void CSkillShader::GetCamera(CCamera * pCamera)
{
	m_pCamera = pCamera;

	for (int i = 0; i < m_nObjects; ++i) {
		static_cast<CSkillObject*>(m_ppObjects[i])->SetCamera(m_pCamera);
	}
}

bool CSkillShader::OnProcessKeyInput(UCHAR * pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	return false;
}

bool CSkillShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	POINT cursorPos;

	GetCursorPos(&cursorPos);
	ScreenToClient(m_pCamera->GetHwnd(), &cursorPos);

	if (pKeyBuffer == MK_LBUTTON) {

		if (cursorPos.y > SKILL_MINIMUM_Y && cursorPos.y < SKILL_MAXIMUM_Y) {
		
			if ((cursorPos.x > QSKILL_MINIMUM_X  && cursorPos.x < QSKILL_MAXIMUM_X)) {
				printf("Q\n");
			}
			if ((cursorPos.x > WSKILL_MINIMUM_X  && cursorPos.x < WSKILL_MAXIMUM_X)) {
				printf("W\n");
			}
			if ((cursorPos.x > ESKILL_MINIMUM_X  && cursorPos.x < ESKILL_MAXIMUM_X)) {
				printf("E\n");
			}
			if ((cursorPos.x > RSKILL_MINIMUM_X  && cursorPos.x < RSKILL_MAXIMUM_X)) {
				printf("R\n");
			}
		}

		
	}

	return true;
}

////////////////////////////////////////////////////////////////////////
// 내부함수
D3D12_INPUT_LAYOUT_DESC CSkillShader::CreateInputLayout()
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

D3D12_BLEND_DESC CSkillShader::CreateBlendState()
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

D3D12_SHADER_BYTECODE CSkillShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTexturedGauge",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CSkillShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSTexturedIconGauge",
		"ps_5_1",
		pShaderBlob));
}

void CSkillShader::CreateShader(CCreateMgr * pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	m_nHeaps = 2;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CSkillShader::CreateShaderVariables(CCreateMgr * pCreateMgr, int nBuffers)
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

void CSkillShader::BuildObjects(CCreateMgr * pCreateMgr, void * pContext)
{
	m_pCamera = (CCamera*)pContext;

	m_nObjects = 8;
	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAUGE_INFO) + 255) & ~255);

	CreateShaderVariables(pCreateMgr, m_nObjects);
	
	for (int i = 0; i < m_nHeaps; ++i) {
		CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 4, i);
		CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer, ncbElementBytes, i);
	}
	
	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CSkillObject *pUIObject{ NULL };

	m_nMaterials = m_nHeaps;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreateStickGreySkillMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[1] = Materials::CreateStickColorSkillMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);

	for (int i = 0; i < m_nObjects; ++i)
	{
		pUIObject = new CSkillObject(pCreateMgr, (SkillUIType)i);
		pUIObject->SetCamera(m_pCamera);
		if (i >= 4) pUIObject->SetDistance(FRAME_BUFFER_WIDTH / 128.0128f);	 // distance 10
		else	   pUIObject->SetDistance(FRAME_BUFFER_WIDTH / 128.00128f);
		pUIObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));

		m_ppObjects[i] = pUIObject;
	}
}

void CSkillShader::ReleaseObjects()
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

