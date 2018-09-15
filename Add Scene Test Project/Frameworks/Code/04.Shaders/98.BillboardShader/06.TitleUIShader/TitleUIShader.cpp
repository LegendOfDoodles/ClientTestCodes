#include "stdafx.h"
#include "TitleUIShader.h"
#include "05.Objects/96.Billboard/01.FrameObject/UIFrameObject.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/08.Player/Player.h"

/// <summary>
/// 목적: UI Frame 쉐이더 (틀만 생성)
/// 최종 수정자:  김나단
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-08-22
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CTitleUIShader::CTitleUIShader(shared_ptr<CCreateMgr> pCreateMgr)
	: CBillboardShader(pCreateMgr)
{

}

CTitleUIShader::~CTitleUIShader()
{
}

////////////////////////////////////////////////////////////////////////
//
void CTitleUIShader::UpdateShaderVariables(int opt)
{
	UNREFERENCED_PARAMETER(opt);
	static UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CTitleUIShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CTitleUIShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera, 0);

	for (int j = 0; j < m_nObjects; j++)
	{
		UIFrameType type = (UIFrameType)((CUIFrameObject*)m_ppObjects[j])->GetType();

		switch (type)
		{
		case GameLogo:
			m_ppMaterials[0]->UpdateShaderVariable(0);
			break;
		case Connect:
			m_ppMaterials[0]->UpdateShaderVariable(1);
			break;
		case Exit:
			m_ppMaterials[0]->UpdateShaderVariable(2);
			break;
		}

		m_ppObjects[j]->Render(pCamera);
	}
}

bool CTitleUIShader::OnProcessKeyInput(UCHAR * pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	return false;
}

bool CTitleUIShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	POINT cursorPos;

	GetCursorPos(&cursorPos);
	ScreenToClient(m_pCamera->GetHwnd(), &cursorPos);

	if (pKeyBuffer == MK_LBUTTON)
	{
		// 접속 버튼, 시작 버튼
		if ((cursorPos.x > GAMESTART_AND_CONNECT_MINIMUM_X  && cursorPos.x < GAMESTART_AND_CONNECT_MAXIMUM_X)
			&& (cursorPos.y > GAMESTART_AND_CONNECT_MINIMUM_Y && cursorPos.y < GAMESTART_AND_CONNECT_MAXIMUM_Y))
		{
			printf("%d, %d\n", cursorPos.x, cursorPos.y);

			return false;
		}

		// 종료 버튼, 타이틀 복귀 버튼
		if ((cursorPos.x > EXIT_AND_PREV_MINIMUM_X  && cursorPos.x < EXIT_AND_PREV_MAXIMUM_X)
			&& (cursorPos.y > EXIT_AND_PREV_MINIMUM_Y && cursorPos.y < EXIT_AND_PREV_MAXIMUM_Y))
		{
			printf("%d, %d\n", cursorPos.x, cursorPos.y);

			return false;
		}
	}
	return false;
}

////////////////////////////////////////////////////////////////////////
// 내부함수
D3D12_INPUT_LAYOUT_DESC CTitleUIShader::CreateInputLayout()
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

D3D12_BLEND_DESC CTitleUIShader::CreateBlendState()
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

D3D12_SHADER_BYTECODE CTitleUIShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	//./Code/04.Shaders/99.GraphicsShader/
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/DefaultShader.hlsl",
		"VSTextured",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CTitleUIShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/DefaultShader.hlsl",
		"PSTextured",
		"ps_5_1",
		pShaderBlob));
}

void CTitleUIShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CTitleUIShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void * pContext)
{
	m_pCamera = (CCamera*)pContext;
	
	m_nObjects = 3;
	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 3);
	CreateShaderVariables(pCreateMgr, ncbElementBytes, m_nObjects);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer.Get(), ncbElementBytes);

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CUIFrameObject *pUIObject{ NULL };

	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreateTitleSceneUseMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);

	for (int i = 0; i < m_nObjects; ++i)
	{
		pUIObject = new CUIFrameObject(pCreateMgr, UIFrameType(GameLogo + i));
		pUIObject->SetCamera(m_pCamera);
		
		if (i == 0) pUIObject->SetDistance(2.f);
		else pUIObject->SetDistance(1.5f);

		pUIObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));

		m_ppObjects[i] = pUIObject;
	}
}
