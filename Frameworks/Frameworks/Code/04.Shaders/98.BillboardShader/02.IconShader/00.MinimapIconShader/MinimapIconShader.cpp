#include "stdafx.h"
#include "MinimapIconShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/08.Player/Player.h"
#include "05.Objects/99.Material/Material.h"
#include "00.Global/01.Utility/06.HPGaugeManager/HPGaugeManager.h"

/// <summary>
/// ����: �̴ϸ� Icon ���̴�
/// ���� ������:  �̿뼱
/// ������ ���:  �̿뼱
/// ���� ���� ��¥: 2018-05-13
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
	m_MinionIconObjectList.remove_if([this](CMinimapIconObjects* obj)
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
		CCollisionObject* master = ((CMinimapIconObjects*)m_ppObjects[j])->GetMasterObject();
		
		switch (master->GetType())
		{
			
			case ObjectType::StickPlayer:
				if (master->GetTeam() == TeamType::Blue) {
					CShader::Render(pCamera, 0);
					m_ppMaterials[0]->UpdateShaderVariables();
				}
				else if (master->GetTeam() == TeamType::Red) {
					CShader::Render(pCamera, 4);
					m_ppMaterials[4]->UpdateShaderVariables();
				}
				break;
			case ObjectType::SwordPlayer:
				if (master->GetTeam() == TeamType::Blue) {
					CShader::Render(pCamera, 1);
					m_ppMaterials[1]->UpdateShaderVariables();
				}
				else if (master->GetTeam() == TeamType::Red) {
					CShader::Render(pCamera, 5);
					m_ppMaterials[5]->UpdateShaderVariables();
				}
				break;
			case ObjectType::BowPlayer:
				if (master->GetTeam() == TeamType::Blue) {
					CShader::Render(pCamera, 2);
					m_ppMaterials[2]->UpdateShaderVariables();
				}
				else if (master->GetTeam() == TeamType::Red) {
					CShader::Render(pCamera, 6);
					m_ppMaterials[6]->UpdateShaderVariables();
				}
				break;
			case ObjectType::StaffPlayer:
				if (master->GetTeam() == TeamType::Blue) {
					CShader::Render(pCamera, 3);
					m_ppMaterials[3]->UpdateShaderVariables();
				}
				else if (master->GetTeam() == TeamType::Red) {
					CShader::Render(pCamera, 7);
					m_ppMaterials[7]->UpdateShaderVariables();
				}
				break;
		}
		if (m_ppObjects[j]) m_ppObjects[j]->Render(pCamera);
	}

	CShader::Render(pCamera, 9);
	m_ppMaterials[9]->UpdateShaderVariables();
	// �̴Ͼ�
	for (auto& iter = m_MinionIconObjectList.begin(); iter != m_MinionIconObjectList.end(); ++iter) {
		
		(*iter)->Render(pCamera);
	}
}

void CMinimapIconShader::GetCamera(CCamera * pCamera)
{
	m_pCamera = pCamera;

	for (auto& iter = m_MinionIconObjectList.begin(); iter != m_MinionIconObjectList.end(); ++iter) {
		static_cast<CMinimapIconObjects*>(*iter)->SetCamera(m_pCamera);
	}

	for (int i = 0; i < m_nObjects; ++i)
	{
		static_cast<CMinimapIconObjects*>(m_ppObjects[i])->SetCamera(m_pCamera);
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
// ���� �Լ�
D3D12_INPUT_LAYOUT_DESC CMinimapIconShader::CreateInputLayout()
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

void CMinimapIconShader::CreateShader(CCreateMgr * pCreateMgr, UINT nRenderTargets)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	m_nHeaps = 10;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets);
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
	assert(SUCCEEDED(hResult) && "m_pConstBuffer->Map Failed");
}

void CMinimapIconShader::BuildObjects(CCreateMgr * pCreateMgr, void * pContext)
{
	m_pCamera = (CCamera*)pContext;

	m_nObjects = m_nPlayer;
	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	
	CreateShaderVariables(pCreateMgr, m_nObjects + MAX_MINION);

	for (int i = 0; i < m_nHeaps; ++i) {
		CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects + MAX_MINION, 1, i);
		CreateConstantBufferViews(pCreateMgr, m_nObjects + MAX_MINION, m_pConstBuffer, ncbElementBytes, i);
	}
#if USE_BATCH_MATERIAL
	m_nMaterials = m_nHeaps;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	
	//Player
		// Blue Team
	m_ppMaterials[0] = Materials::CreateStickBlueIconMaterial(pCreateMgr,	&m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[1] = Materials::CreateSwordBlueIconMaterial(pCreateMgr,	&m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);
	m_ppMaterials[2] = Materials::CreateBowBlueIconMaterial(pCreateMgr,		&m_psrvCPUDescriptorStartHandle[2], &m_psrvGPUDescriptorStartHandle[2]);
	m_ppMaterials[3] = Materials::CreateStaffBlueIconMaterial(pCreateMgr,	&m_psrvCPUDescriptorStartHandle[3], &m_psrvGPUDescriptorStartHandle[3]);
		// Red Team
	m_ppMaterials[4] = Materials::CreateStickRedIconMaterial(pCreateMgr,	&m_psrvCPUDescriptorStartHandle[4], &m_psrvGPUDescriptorStartHandle[4]);
	m_ppMaterials[5] = Materials::CreateSwordRedIconMaterial(pCreateMgr,	&m_psrvCPUDescriptorStartHandle[5], &m_psrvGPUDescriptorStartHandle[5]);
	m_ppMaterials[6] = Materials::CreateBowRedIconMaterial(pCreateMgr,		&m_psrvCPUDescriptorStartHandle[6], &m_psrvGPUDescriptorStartHandle[6]);
	m_ppMaterials[7] = Materials::CreateStaffRedIconMaterial(pCreateMgr,	&m_psrvCPUDescriptorStartHandle[7], &m_psrvGPUDescriptorStartHandle[7]);
	//Minion
		// Blue
	m_ppMaterials[8] = Materials::CreateMinionBlueIconMaterial(pCreateMgr,	&m_psrvCPUDescriptorStartHandle[8], &m_psrvGPUDescriptorStartHandle[8]);
		// Red
	m_ppMaterials[9] = Materials::CreateMinionRedIconMaterial(pCreateMgr,	&m_psrvCPUDescriptorStartHandle[9], &m_psrvGPUDescriptorStartHandle[9]);

#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif

	// Player Icon ����
	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CMinimapIconObjects *pIconObject = NULL;
	
	for (int i = 0; i < m_nObjects; ++i) {
		if (i < m_nPlayer) {
			pIconObject = new CMinimapIconObjects(pCreateMgr, IconUIType::PlayerIcon);

			pIconObject->SetCamera(m_pCamera);
			pIconObject->SetDistance((FRAME_BUFFER_WIDTH / 128.f) - 0.04f);	// distance 9
			pIconObject->SetObject(m_pPlayer[i]);
			pIconObject->GetmasterObjectType((ObjectType)m_pPlayer[i]->GetType());
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

		CMinimapIconObjects *pMinionIcon{ NULL };
		CCollisionObject *pMinionObjects{ NULL };

		pMinionIcon = new CMinimapIconObjects(m_pCreateMgr, IconUIType::MinionIcon);
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