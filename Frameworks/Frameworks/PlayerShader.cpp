#include "stdafx.h"
#include "PlayerShader.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/06.Minion/Minion.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
#include "05.Objects/07.StaticObjects/01.Nexus/Nexus.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"


//////////////////////////////////////////////////////////////////////////////////////////////////
CPlayerShader::CPlayerShader(CCreateMgr *pCreateMgr) : CShader(pCreateMgr)
{
}

CPlayerShader::~CPlayerShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CPlayerShader::ReleaseUploadBuffers()
{
	if (!m_ppObjects) return;

	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->ReleaseUploadBuffers();
	}
#if USE_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
#endif
}

void CPlayerShader::UpdateShaderVariables()
{
#if USE_INSTANCING
	m_pCommandList->SetGraphicsRootShaderResourceView(2,
		m_pInstanceBuffer->GetGPUVirtualAddress());

	for (int i = 0; i < m_nObjects; i++)
	{
		XMStoreFloat4x4(&m_pMappedObjects[i].m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
#else
	static UINT elementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_ANIOBJECT_INFO *pMappedObject = (CB_ANIOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMFLOAT4X4 tmp[128];
		memcpy(tmp, m_ppObjects[i]->GetFrameMatrix(), sizeof(XMFLOAT4X4) * 128);
		memcpy(pMappedObject->m_xmf4x4Frame, tmp, sizeof(XMFLOAT4X4) * 128);

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World0,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
#endif
}

void CPlayerShader::UpdateBoundingBoxShaderVariables()
{
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedBoundingBoxes + (i * boundingBoxElementBytes));

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CPlayerShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CPlayerShader::Render(CCamera *pCamera)
{
	CShader::Render(pCamera);
#if USE_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables();
#endif

#if USE_INSTANCING
	m_ppObjects[0]->Render(pCamera, m_nObjects);
#else
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->Render(pCamera);
	}
#endif
}

void CPlayerShader::RenderBoundingBox(CCamera * pCamera)
{
	CShader::RenderBoundingBox(pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->RenderBoundingBox(pCamera);
	}
}

CBaseObject *CPlayerShader::PickObjectByRayIntersection(
	XMFLOAT3& pickPosition, XMFLOAT4X4& xmf4x4View, float &nearHitDistance)
{
	bool intersected = 0;

	nearHitDistance = FLT_MAX;
	float hitDistance = FLT_MAX;
	CBaseObject *pSelectedObject{ NULL };

	for (int j = 0; j < m_nObjects; j++)
	{
		intersected = m_ppObjects[j]->PickObjectByRayIntersection(pickPosition, xmf4x4View, hitDistance);
		if (intersected && (hitDistance < nearHitDistance))
		{
			nearHitDistance = hitDistance;
			pSelectedObject = m_ppObjects[j];
		}
	}

	return(pSelectedObject);
}

bool CPlayerShader::OnProcessKeyInput(UCHAR* pKeyBuffer)
{
	static float R = 0.0f;
	static float M = 0.0f;

	if (GetAsyncKeyState('M') & 0x0001)
	{
		m_nWeaponState++;
		if (m_nWeaponState >= 3)m_nWeaponState = 0;
		for (int i = 0; i < m_nObjects; ++i) {
			CMinion* obj = dynamic_cast<CMinion*>(m_ppObjects[i]);

			obj->SetMesh(1, m_pWeapons[m_nWeaponState]);
		}
	}
	if (GetAsyncKeyState('N') & 0x0001)
	{

	}
	return true;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CPlayerShader::CreateInputLayout()
{
	UINT nInputElementDescs = 6;
	D3D12_INPUT_ELEMENT_DESC *pd3dInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	UINT cnt = 0;
	pd3dInputElementDescs[0] = {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pd3dInputElementDescs[1] = {
		"NORMAL",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pd3dInputElementDescs[2] = {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	pd3dInputElementDescs[3] = {
		"WEIGHTS",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pd3dInputElementDescs[4] = {
		"BONEINDICES",
		0,
		DXGI_FORMAT_R8G8B8A8_UINT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	pd3dInputElementDescs[5] = {
		"TANGENT",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CPlayerShader::CreateVertexShader(ID3DBlob **ppShaderBlob)
{
	//./Code/04.Shaders/99.GraphicsShader/
#if USE_INSTANCING
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "VSTexturedLightingInstancing", "vs_5_1", ppShaderBlob));
#else
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "VSBone", "vs_5_1", ppShaderBlob));
#endif
}

D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "PSBone", "ps_5_1", ppShaderBlob));
}

void CPlayerShader::CreateShader(CCreateMgr *pCreateMgr)
{
	m_nPipelineStates = 2;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	for (int i = 0; i < m_nPipelineStates; ++i)
	{
		m_ppPipelineStates[i] = NULL;
	}

	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr);
	CShader::CreateBoundingBoxShader(pCreateMgr);
}

void CPlayerShader::CreateShaderVariables(CCreateMgr *pCreateMgr)
{
	HRESULT hResult;

#if USE_INSTANCING
	m_pInstanceBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		sizeof(CB_GAMEOBJECT_INFO) * m_nObjects,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL);

	hResult = m_pInstanceBuffer->Map(0, NULL, (void **)&m_pMappedObjects);
	assert(SUCCEEDED(hResult) && "m_pInstanceBuffer->Map Failed");
#else
	UINT elementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);

	m_pConstBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		elementBytes * m_nObjects,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	hResult = m_pConstBuffer->Map(0, NULL, (void **)&m_pMappedObjects);
	assert(SUCCEEDED(hResult) && "m_pConstBuffer->Map Failed");
#endif

	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	m_pBoundingBoxBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		boundingBoxElementBytes * m_nObjects,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	hResult = m_pBoundingBoxBuffer->Map(0, NULL, (void **)&m_pMappedBoundingBoxes);
	assert(SUCCEEDED(hResult) && "m_pBoundingBoxBuffer->Map Failed");
}

void CPlayerShader::BuildObjects(CCreateMgr *pCreateMgr, void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;

	m_nObjects =1;
	m_ppObjects = new CBaseObject*[m_nObjects];

#if USE_INSTANCING
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 0, 2);
	CreateShaderVariables(pCreateMgr);
#else
	UINT ncbElementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 4);
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 0, 1);
	CreateShaderVariables(pCreateMgr);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer, ncbElementBytes, 0);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pBoundingBoxBuffer, boundingBoxElementBytes, 1);
#endif

#if USE_BATCH_MATERIAL
	m_pMaterial = Materials::CreateTresureBoxMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif

	CSkinnedMesh *pPlayerMesh = new CSkinnedMesh(pCreateMgr, "Resource//3D//Minion//Mesh//Minion.meshinfo");
	m_pWeapons[0] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Minion//Weapon//Minion_Sword.meshinfo");
	m_pWeapons[1] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Minion//Weapon//Minion_Staff.meshinfo");
	m_pWeapons[2] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Minion//Weapon//Minion_Bow2.meshinfo");



	CSkeleton *pSIdle = new CSkeleton("Resource//3D//Minion//Animation//Sword//Minion_S_Idle.aniinfo");
	CSkeleton *pSAtk1 = new CSkeleton("Resource//3D//Minion//Animation//Sword//Minion_S_Attack1.aniinfo");
	CSkeleton *pSAtk2 = new CSkeleton("Resource//3D//Minion//Animation//Sword//Minion_S_Attack2.aniinfo");
	CSkeleton *pSWalkStart = new CSkeleton("Resource//3D//Minion//Animation//Sword//Minion_S_WalkStart.aniinfo");
	CSkeleton *pSWalk = new CSkeleton("Resource//3D//Minion//Animation//Sword//Minion_S_Walk.aniinfo");

	CSkeleton *pBIdle = new CSkeleton("Resource//3D//Minion//Animation//Bow//Minion_B_Idle.aniinfo");
	CSkeleton *pBAtk = new CSkeleton("Resource//3D//Minion//Animation//Bow//Minion_B_Attack.aniinfo");
	CSkeleton *pBWalkStart = new CSkeleton("Resource//3D//Minion//Animation//Bow//Minion_B_WalkStart.aniinfo");
	CSkeleton *pBWalk = new CSkeleton("Resource//3D//Minion//Animation//Bow//Minion_B_Walk.aniinfo");

	CSkeleton *pMIdle = new CSkeleton("Resource//3D//Minion//Animation//Magic//Minion_M_Idle.aniinfo");
	CSkeleton *pMAtk1 = new CSkeleton("Resource//3D//Minion//Animation//Magic//Minion_M_Attack1.aniinfo");
	CSkeleton *pMAtk2 = new CSkeleton("Resource//3D//Minion//Animation//Magic//Minion_M_Attack2.aniinfo");
	CSkeleton *pMWalkStart = new CSkeleton("Resource//3D//Minion//Animation//Magic//Minion_M_WalkStart.aniinfo");
	CSkeleton *pMWalk = new CSkeleton("Resource//3D//Minion//Animation//Magic//Minion_M_Walk.aniinfo");


	CSkeleton *pDie = new CSkeleton("Resource//3D//Minion//Animation//Minion_Die.aniinfo");


	pPlayerMesh->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(4)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(1.5), CONVERT_PaperUnit_to_InG(1.5), CONVERT_PaperUnit_to_InG(3.5)));


	float fxPitch = 12.0f * 5.f;
	float fyPitch = 12.0f * 5.f;
	float fzPitch = 12.0f * 5.f;

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CMinion *pMinionObject = NULL;

	for (int j = 0; j < 3; ++j) {
		m_pWeapons[j]->AddRef();
	}


}

void CPlayerShader::ReleaseObjects()
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
	Safe_Delete(m_pMaterial);
#endif
}