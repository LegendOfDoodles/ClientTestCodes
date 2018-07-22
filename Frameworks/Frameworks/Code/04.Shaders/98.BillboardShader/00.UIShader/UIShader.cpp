#include "stdafx.h"
#include "UIShader.h"
#include "05.Objects/96.Billboard/01.FrameObject/UIFrameObject.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/08.Player/Player.h"

/// <summary>
/// 목적: UI Frame 쉐이더 (틀만 생성)
/// 최종 수정자:  김나단
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-07-03
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CUIObjectShader::CUIObjectShader(shared_ptr<CCreateMgr> pCreateMgr)
	: CBillboardShader(pCreateMgr)
{

}

CUIObjectShader::~CUIObjectShader()
{
}

////////////////////////////////////////////////////////////////////////
//
void CUIObjectShader::ReleaseUploadBuffers()
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

void CUIObjectShader::UpdateShaderVariables(int opt)
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

void CUIObjectShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}

}

void CUIObjectShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		UIFrameType type = (UIFrameType)((CUIFrameObject*)m_ppObjects[j])->GetType();

		switch (type)
		{
		case MinimapFrame:
		case SkillFrameQ:
		case SkillFrameW:
		case SkillFrameE:
		case SkillFrameR:
			isRendering = true;
			m_ppMaterials[0]->UpdateShaderVariable(0);
			break;
		case CharacterFrame:
			isRendering = true;
			m_ppMaterials[0]->UpdateShaderVariable(2);
			break;
		case SelectSpecial_7:
		case SelectSpecial_12:
		case SelectSpecial_17:
		case SelectSpecial_21:
			isRendering = true;
			m_ppMaterials[0]->UpdateShaderVariable(6);
			break;
		case StatusFrame:
			isRendering = true;
			m_ppMaterials[0]->UpdateShaderVariable(3);
			break;
		case KDAFrame:
			isRendering = true;
			m_ppMaterials[0]->UpdateShaderVariable(4);
			break;
		case TimerFrame:
			isRendering = true;
			m_ppMaterials[0]->UpdateShaderVariable(5);
			break;
		case SpecialFrame:
			if (LButton == true) isRendering = true;
			else isRendering = false;
			m_ppMaterials[0]->UpdateShaderVariable(1);
			break;
		default:
			break;
		}

		if(isRendering) m_ppObjects[j]->Render(pCamera);
	}
}

void CUIObjectShader::SetCamera(CCamera * pCamera)
{
	m_pCamera = pCamera;

	for (int i = 0; i < m_nObjects; ++i) {
		static_cast<CUIFrameObject*>(m_ppObjects[i])->SetCamera(m_pCamera);
	}
}

bool CUIObjectShader::OnProcessKeyInput(UCHAR * pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	return true;
}

bool CUIObjectShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	POINT cursorPos;

	GetCursorPos(&cursorPos);
	ScreenToClient(m_pCamera->GetHwnd(), &cursorPos);


	if (pKeyBuffer == MK_LBUTTON)
	{
		// 캐릭터창 클릭
		if ((cursorPos.x > SPECIAL_MINIMUM_X  && cursorPos.x < SPECIAL_MAXIMUM_X)
			&& (cursorPos.y > SPECIAL_MINIMUM_Y && cursorPos.y < SPECIAL_MAXIMUM_Y))
		{
			if (LButton == true) LButton = false;
			else LButton = true;
		}

		// 특성창이 켜져있고 (LButton == true) 플레이어(현재 조종중인)의 특성 포인트가 있을 경우
		// 각 특성을 선택한다. (윈도우 좌표로)
		if (LButton == true && m_pPlayer->GetPlayerStatus()->SpecialPoint >= 1) {
			
			// Attack
			if ((cursorPos.x > SPECIAL_MINIMUM_X  && cursorPos.x < SPECIAL_MAXIMUM_X)
				&& (cursorPos.y > SPECIAL_MINIMUM_Y && cursorPos.y < SPECIAL_MAXIMUM_Y))
			{
				//// 빈곳 찾아서 넣기
				//for (int i = 0; i < 4; ++i) {
				//	if (m_pPlayer->GetPlayerStatus()->Special[i] == (SpecialType::NoSelected)) {
				//		m_pPlayer->GetPlayerStatus()->Special[i] = (SpecialType::AttackSpecial);
				//	};
				//}

				//// 특성 포인트 사용
				//m_pPlayer->GetPlayerStatus()->SpecialPoint - 1;

				//// 특성창 닫기
				//LButton = false;
			}

			//// Defence
			//if ((cursorPos.x > SPECIAL_MINIMUM_X  && cursorPos.x < SPECIAL_MAXIMUM_X)
			//	&& (cursorPos.y > SPECIAL_MINIMUM_Y && cursorPos.y < SPECIAL_MAXIMUM_Y))
			//{
			//	// 빈곳 찾아서 넣기
			//	for (int i = 0; i < 4; ++i) {
			//		if (m_pPlayer->GetPlayerStatus()->Special[i] == (SpecialType::NoSelected)) {
			//			m_pPlayer->GetPlayerStatus()->Special[i] = (SpecialType::DefenceSpecial);
			//		};
			//	}

			//	// 특성 포인트 사용
			//	m_pPlayer->GetPlayerStatus()->SpecialPoint - 1;

			//	// 특성창 닫기
			//	if (LButton == true) LButton = false;
			//}
			//
			//// Tech
			//if ((cursorPos.x > SPECIAL_MINIMUM_X  && cursorPos.x < SPECIAL_MAXIMUM_X)
			//	&& (cursorPos.y > SPECIAL_MINIMUM_Y && cursorPos.y < SPECIAL_MAXIMUM_Y))
			//{
			//	// 빈곳 찾아서 넣기
			//	for (int i = 0; i < 4; ++i) {
			//		if (m_pPlayer->GetPlayerStatus()->Special[i] == (SpecialType::NoSelected)) {
			//			m_pPlayer->GetPlayerStatus()->Special[i] = (SpecialType::TechnicSpecial);
			//		};
			//	}

			//	// 특성 포인트 사용
			//	m_pPlayer->GetPlayerStatus()->SpecialPoint - 1;

			//	// 특성창 닫기
			//	if (LButton == true) LButton = false;
			//}

		}

	}

	return true;
}

////////////////////////////////////////////////////////////////////////
// 내부함수
D3D12_INPUT_LAYOUT_DESC CUIObjectShader::CreateInputLayout()
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

D3D12_BLEND_DESC CUIObjectShader::CreateBlendState()
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

D3D12_SHADER_BYTECODE CUIObjectShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	//./Code/04.Shaders/99.GraphicsShader/
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTextured",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CUIObjectShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSTextured",
		"ps_5_1",
		pShaderBlob));
}

void CUIObjectShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CUIObjectShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void * pContext)
{
	m_pCamera = (CCamera*)pContext;
	
	m_nObjects = 14;
	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 7);
	CreateShaderVariables(pCreateMgr, ncbElementBytes, m_nObjects);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer.Get(), ncbElementBytes);

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CUIFrameObject *pUIObject{ NULL };

	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreateUIMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);

	for (int i = 0; i < m_nObjects; ++i)
	{
		pUIObject = new CUIFrameObject(pCreateMgr, (UIFrameType)i);
		pUIObject->SetCamera(m_pCamera);
		pUIObject->SetDistance(FRAME_BUFFER_WIDTH / 128);	 // distance 10
		pUIObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));

		m_ppObjects[i] = pUIObject;
	}
}

void CUIObjectShader::ReleaseObjects()
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
