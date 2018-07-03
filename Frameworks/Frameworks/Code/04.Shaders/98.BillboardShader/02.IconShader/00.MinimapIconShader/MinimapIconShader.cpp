#include "stdafx.h"
#include "MinimapIconShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/08.Player/Player.h"
#include "05.Objects/99.Material/Material.h"
#include "00.Global/01.Utility/06.HPGaugeManager/HPGaugeManager.h"

/// <summary>
/// 목적: 미니맵 Icon 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  이용선, 김나단
/// 최종 수정 날짜: 2018-07-03
/// </summary>

CMinimapIconShader::CMinimapIconShader(CCreateMgr *pCreateMgr)
	: CShader(pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;
}

CMinimapIconShader::~CMinimapIconShader()
{
}

void CMinimapIconShader::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			m_ppObjects[j]->ReleaseUploadBuffers();
		}
	}

#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
#endif
}

void CMinimapIconShader::UpdateShaderVariables()
{
	static UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}

	for (auto& iter = m_MinionIconObjectList.begin(); iter != m_MinionIconObjectList.end(); ++iter) {
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects + ((*iter)->GetIndex() * elementBytes));
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4((*iter)->GetWorldMatrix())));
	}
}

void CMinimapIconShader::AnimateObjects(float timeElapsed)
{
	m_MinionIconObjectList.remove_if([this](CIconObject* obj)
	{
		if (obj->GetState() == States::Die)
		{
			ResetPossibleIndex(obj->GetIndex());
			return true;
		}
		return false;
	});

	for (auto& iter = m_MinionIconObjectList.begin(); iter != m_MinionIconObjectList.end(); ++iter) {
		(*iter)->Animate(timeElapsed);
	}

	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}

	if (m_pIconManger->GetCount() > 0) SpawnMinionIcon();
}

void CMinimapIconShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		CCollisionObject* master = ((CIconObject*)m_ppObjects[j])->GetMasterObject();
		
		switch (master->GetType())
		{
			case ObjectType::StickPlayer:
				if (master->GetTeam() == TeamType::Blue) {
					m_ppMaterials[0]->UpdateShaderVariable(0);
				}
				else if (master->GetTeam() == TeamType::Red) {
					m_ppMaterials[1]->UpdateShaderVariable(0);
				}
				break;
			case ObjectType::SwordPlayer:
				if (master->GetTeam() == TeamType::Blue) {
					m_ppMaterials[0]->UpdateShaderVariable(1);
				}
				else if (master->GetTeam() == TeamType::Red) {
					m_ppMaterials[1]->UpdateShaderVariable(1);
				}
				break;
			case ObjectType::BowPlayer:
				if (master->GetTeam() == TeamType::Blue) {
					m_ppMaterials[0]->UpdateShaderVariable(3);
				}
				else if (master->GetTeam() == TeamType::Red) {
					m_ppMaterials[1]->UpdateShaderVariable(3);
				}
				break;
			case ObjectType::StaffPlayer:
				if (master->GetTeam() == TeamType::Blue) {
					m_ppMaterials[0]->UpdateShaderVariable(2);
				}
				else if (master->GetTeam() == TeamType::Red) {
					m_ppMaterials[1]->UpdateShaderVariable(2);
				}
				break;
		}
		if (m_ppObjects[j]) m_ppObjects[j]->Render(pCamera);
	}

	m_ppMaterials[2]->UpdateShaderVariable(0);
	// 미니언
	for (auto& iter = m_MinionIconObjectList.begin(); iter != m_MinionIconObjectList.end(); ++iter) {
		
		(*iter)->Render(pCamera);
	}
}

void CMinimapIconShader::GetCamera(CCamera * pCamera)
{
	m_pCamera = pCamera;

	for (auto& iter = m_MinionIconObjectList.begin(); iter != m_MinionIconObjectList.end(); ++iter) {
		static_cast<CIconObject*>(*iter)->SetCamera(m_pCamera);
	}

	for (int i = 0; i < m_nObjects; ++i)
	{
		static_cast<CIconObject*>(m_ppObjects[i])->SetCamera(m_pCamera);
	}
}

void CMinimapIconShader::SetUIObjectsManager(CUIObjectManager * pManger)
{
	m_pIconManger = pManger;
	m_MinionObjectList = m_pIconManger->GetMinionObjectList();
}

bool CMinimapIconShader::OnProcessKeyInput(UCHAR * pKeyBuffer)
{
	return true;
}

bool CMinimapIconShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	return false;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CMinimapIconShader::CreateInputLayout()
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

D3D12_BLEND_DESC CMinimapIconShader::CreateBlendState()
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

D3D12_SHADER_BYTECODE CMinimapIconShader::CreateVertexShader(ID3DBlob ** ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTextured",
		"vs_5_1",
		ppShaderBlob));
}

D3D12_SHADER_BYTECODE CMinimapIconShader::CreatePixelShader(ID3DBlob ** ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSTextured",
		"ps_5_1",
		ppShaderBlob));
}

void CMinimapIconShader::CreateShader(CCreateMgr * pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	m_nHeaps = 3;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CMinimapIconShader::CreateShaderVariables(CCreateMgr * pCreateMgr, int nBuffers)
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
	ThrowIfFailed(hResult);
}

void CMinimapIconShader::BuildObjects(CCreateMgr * pCreateMgr, void * pContext)
{
	m_pCamera = (CCamera*)pContext;

	m_nObjects = m_nPlayer;
	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	
	CreateShaderVariables(pCreateMgr, m_nObjects + MAX_MINION);

	for (int i = 0; i < m_nHeaps; ++i) {
		CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects + MAX_MINION, 4, i);
		CreateConstantBufferViews(pCreateMgr, m_nObjects + MAX_MINION, m_pConstBuffer, ncbElementBytes, i);
	}
#if USE_BATCH_MATERIAL
	m_nMaterials = m_nHeaps;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	
	//Player
	/* 0. Stick, 1.Sword, 2.Magic 3.Bow */
		// Blue Team
	m_ppMaterials[0] = Materials::CreatePlayerBlueIconMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	// Red Team
	m_ppMaterials[1] = Materials::CreatePlayerRedIconMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);

	//Minion
	/* 0. Blue 1. Red */
	m_ppMaterials[2] = Materials::CreateMinionIconMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[2], &m_psrvGPUDescriptorStartHandle[2]);

#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif

	// Player Icon 생성
	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CIconObject *pIconObject = NULL;
	
	for (int i = 0; i < m_nObjects; ++i) {
		if (i < m_nPlayer) {
			pIconObject = new CIconObject(pCreateMgr, IconUIType::PlayerIcon);

			pIconObject->SetCamera(m_pCamera);
			pIconObject->SetDistance((FRAME_BUFFER_WIDTH / 128.f) - 0.04f);	// distance 9
			pIconObject->SetObject(m_pPlayer[i]);
			pIconObject->WorldToMinimap();

			pIconObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));
		}

		m_ppObjects[i] = pIconObject;
	}
}


int CMinimapIconShader::GetPossibleIndex()
{
	for (int idx = m_nObjects; idx < MAX_MINION + m_nObjects; ++idx)
	{
		if (!m_indexArr[idx])
		{
			m_indexArr[idx] = true;
			return idx;
		}
	}
	return NONE;
}

void CMinimapIconShader::SpawnMinionIcon()
{
	static UINT incrementSize{ m_pCreateMgr->GetCbvSrvDescriptorIncrementSize() };

	m_pCreateMgr->ResetCommandList();

	int cnt{ 0 };
	CollisionObjectList::reverse_iterator minion{ m_MinionObjectList->rbegin() };

	for (; cnt < m_pIconManger->GetCount(); ++cnt)
	{
		int index = GetPossibleIndex();
		if (index == NONE) break;

		CIconObject *pMinionIcon{ NULL };
		CCollisionObject *pMinionObjects{ NULL };

		pMinionIcon = new CIconObject(m_pCreateMgr, IconUIType::MinionIcon);
		pMinionObjects = (*minion);

		pMinionIcon->SetObject(pMinionObjects);
		pMinionIcon->SetDistance((FRAME_BUFFER_WIDTH / 128.f) - 0.01f);
		pMinionIcon->SetCamera(m_pCamera);
		pMinionIcon->WorldToMinimap();

		pMinionIcon->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * index));

		pMinionIcon->SaveIndex(index);

		m_MinionIconObjectList.emplace_back(pMinionIcon);

		if (minion != m_MinionObjectList->rend()) ++minion;
	}
	m_pCreateMgr->ExecuteCommandList();

	if (!cnt) return;

	MinionIconObjectList::reverse_iterator &IconBegin{ m_MinionIconObjectList.rbegin() };
	MinionIconObjectList::reverse_iterator &IconEnd{ m_MinionIconObjectList.rbegin() };

	for (int i = 0; i < cnt - 1; ++i) ++IconBegin;

	for (int i = 0; i < cnt; ++i)
	{
		(*IconBegin)->ReleaseUploadBuffers();
		if (IconBegin != IconEnd) --IconBegin;
	}

	m_pIconManger->ResetCount();
}


void CMinimapIconShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			delete m_ppObjects[j];
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
