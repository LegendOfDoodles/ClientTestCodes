#include "stdafx.h"
#include "MinimapShader.h"
#include "05.Objects/96.Billboard/01.FrameObject/UIFrameObject.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/08.Player/Player.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

/// <summary>
/// 목적: Minimap Ui Shader (FrameShader와의 분리)
/// 최종 수정자:  김나단
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-07-02
/// </summary>

CMinimapShader::CMinimapShader(CCreateMgr * pCreateMgr)
	:CShader(pCreateMgr)
{
}

CMinimapShader::~CMinimapShader()
{
}

////////////////////////////////////////////////////////////////////////
//
void CMinimapShader::ReleaseUploadBuffers()
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

void CMinimapShader::UpdateShaderVariables()
{
	static UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CMinimapShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CMinimapShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera);

	m_ppMaterials[0]->UpdateShaderVariable(0);

	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Render(pCamera);
	}
}

void CMinimapShader::GetCamera(CCamera * pCamera)
{
	m_pCamera = pCamera;

	for (int i = 0; i < m_nObjects; ++i) {
		static_cast<CUIFrameObject*>(m_ppObjects[i])->SetCamera(m_pCamera);
	}
}

bool CMinimapShader::OnProcessKeyInput(UCHAR * pKeyBuffer)
{
	return false;
}

bool CMinimapShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	POINT cursorPos;

	GetCursorPos(&cursorPos);
	ScreenToClient(m_pCamera->GetHwnd(), &cursorPos);

	if (pKeyBuffer == MK_LBUTTON)
	{
		//printf("%d, %d\n", cursorPos.x, cursorPos.y);

		// 미니맵 좌 클릭
		if ((cursorPos.x > MINIMAP_MINIMUM_X  && cursorPos.x < MINIMAP_MAXIMUM_X)
			&& (cursorPos.y > MINIMAP_MINIMUM_Y && cursorPos.y < MINIMAP_MAXIMUM_Y))
		{
			XMFLOAT3 newCameraPos;
			newCameraPos.x = (MINIMAP_MINIMUM_X - cursorPos.x) * -1.736 * 20;
			newCameraPos.y = m_pCamera->GetPosition().y;
			newCameraPos.z = (MINIMAP_MAXIMUM_Y - cursorPos.y) * 1.736 * 20;

			m_pCamera->SetPosition(newCameraPos);
		}
	}

	else if (pKeyBuffer == MK_RBUTTON)
	{
		// 미니맵 우 클릭
		if ((cursorPos.x > MINIMAP_MINIMUM_X  && cursorPos.x < MINIMAP_MAXIMUM_X)
			&& (cursorPos.y > MINIMAP_MINIMUM_Y && cursorPos.y < MINIMAP_MAXIMUM_Y))
		{
			XMFLOAT3 PlayerDestination;
			PlayerDestination.x = (MINIMAP_MINIMUM_X - cursorPos.x) * -1.736 * 20;
			PlayerDestination.y = m_pCamera->GetPosition().y;
			PlayerDestination.z = (MINIMAP_MAXIMUM_Y - cursorPos.y) * 1.736 * 20;

			m_pPlayer->LookAt(PlayerDestination);
			m_pPlayer->SetPathToGo(m_pWayFinder->GetPathToPosition(
				XMFLOAT2(m_pPlayer->GetPosition().x, m_pPlayer->GetPosition().z),
				XMFLOAT2(PlayerDestination.x, PlayerDestination.z),
				m_pPlayer->GetCollisionSize()));
		}
	}

	return true;
}


////////////////////////////////////////////////////////////////////////
//
D3D12_INPUT_LAYOUT_DESC CMinimapShader::CreateInputLayout()
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

D3D12_BLEND_DESC CMinimapShader::CreateBlendState()
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

D3D12_SHADER_BYTECODE CMinimapShader::CreateVertexShader(ID3DBlob ** ppShaderBlob)
{
	//./Code/04.Shaders/99.GraphicsShader/
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTextured",
		"vs_5_1",
		ppShaderBlob));
}

D3D12_SHADER_BYTECODE CMinimapShader::CreatePixelShader(ID3DBlob ** ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSTextured",
		"ps_5_1",
		ppShaderBlob));
}

void CMinimapShader::CreateShader(CCreateMgr * pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	m_nHeaps = 1;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CMinimapShader::CreateShaderVariables(CCreateMgr * pCreateMgr, int nBuffers)
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

void CMinimapShader::BuildObjects(CCreateMgr * pCreateMgr, void * pContext)
{
	m_pCamera = (CCamera*)pContext;

	m_nObjects = 1;
	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 1);
	CreateShaderVariables(pCreateMgr, m_nObjects);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer, ncbElementBytes);

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CUIFrameObject *pUIObject{ NULL };

	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreateMinimapUIMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);

	for (int i = 0; i < m_nObjects; ++i)
	{
		pUIObject = new CUIFrameObject(pCreateMgr, UIFrameType::Minimap);
		pUIObject->SetCamera(m_pCamera);
		pUIObject->SetDistance(FRAME_BUFFER_WIDTH / 128.0128f);	 // distance 10
		pUIObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));

		m_ppObjects[i] = pUIObject;
	}
}

void CMinimapShader::ReleaseObjects()
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
