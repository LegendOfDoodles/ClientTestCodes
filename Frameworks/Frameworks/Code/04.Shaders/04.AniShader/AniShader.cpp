#include "stdafx.h"
#include "AniShader.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/06.Minion/Minion.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
#include "05.Objects/99.Material/Material.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"
#include "00.Global/01.Utility/06.HPGaugeManager/HPGaugeManager.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"

/// <summary>
/// 목적: 움직이는 오브젝트 관리 및 그리기 용도
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-05-09
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CAniShader::CAniShader(CCreateMgr *pCreateMgr, Network* pNetwork) : CShader(pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;
	m_pNetwork = pNetwork;
}

CAniShader::~CAniShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CAniShader::ReleaseUploadBuffers()
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

void CAniShader::UpdateShaderVariables()
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

	for (auto& iter = m_blueObjects.begin(); iter != m_blueObjects.end(); ++iter)
	{
		CB_ANIOBJECT_INFO *pMappedObject = (CB_ANIOBJECT_INFO *)(m_pMappedObjects + ((*iter)->GetIndex() * elementBytes));
		XMFLOAT4X4 tmp[128];
		memcpy(tmp, (*iter)->GetFrameMatrix(), sizeof(XMFLOAT4X4) * 128);
		memcpy(pMappedObject->m_xmf4x4Frame, tmp, sizeof(XMFLOAT4X4) * 128);

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World0,
			XMMatrixTranspose(XMLoadFloat4x4((*iter)->GetWorldMatrix())));
	}
	for (auto& iter = m_redObjects.begin(); iter != m_redObjects.end(); ++iter)
	{
		CB_ANIOBJECT_INFO *pMappedObject = (CB_ANIOBJECT_INFO *)(m_pMappedObjects + ((*iter)->GetIndex() * elementBytes));
		XMFLOAT4X4 tmp[128];
		memcpy(tmp, (*iter)->GetFrameMatrix(), sizeof(XMFLOAT4X4) * 128);
		memcpy(pMappedObject->m_xmf4x4Frame, tmp, sizeof(XMFLOAT4X4) * 128);

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World0,
			XMMatrixTranspose(XMLoadFloat4x4((*iter)->GetWorldMatrix())));
	}

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

void CAniShader::UpdateBoundingBoxShaderVariables()
{
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (auto& iter = m_blueObjects.begin(); iter != m_blueObjects.end(); ++iter)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedBoundingBoxes + ((*iter)->GetIndex() * boundingBoxElementBytes));

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4((*iter)->GetWorldMatrix())));
	}
	for (auto& iter = m_redObjects.begin(); iter != m_redObjects.end(); ++iter)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedBoundingBoxes + ((*iter)->GetIndex() * boundingBoxElementBytes));

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4((*iter)->GetWorldMatrix())));
	}

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedBoundingBoxes + (i * boundingBoxElementBytes));

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CAniShader::AnimateObjects(float timeElapsed)
{
	for (auto& iter = m_blueObjects.begin(); iter != m_blueObjects.end(); ++iter)
	{
		(*iter)->Animate(timeElapsed);
	}
	for (auto& iter = m_redObjects.begin(); iter != m_redObjects.end(); ++iter)
	{
		(*iter)->Animate(timeElapsed);
	}
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CAniShader::Render(CCamera *pCamera)
{
	CShader::Render(pCamera);

	if (m_ppMaterials) m_ppMaterials[0]->UpdateShaderVariables();
	for (auto& iter = m_blueObjects.begin(); iter != m_blueObjects.end(); ++iter)
	{
		(*iter)->Render(pCamera);
	}
	if (m_ppMaterials) m_ppMaterials[1]->UpdateShaderVariables();
	for (auto& iter = m_redObjects.begin(); iter != m_redObjects.end(); ++iter)
	{
		(*iter)->Render(pCamera);
	}
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->Render(pCamera);
	}
}

void CAniShader::RenderBoundingBox(CCamera * pCamera)
{
	CShader::RenderBoundingBox(pCamera);
	for (auto& iter = m_blueObjects.begin(); iter != m_blueObjects.end(); ++iter)
	{
		(*iter)->RenderBoundingBox(pCamera);
	}
	for (auto& iter = m_redObjects.begin(); iter != m_redObjects.end(); ++iter)
	{
		(*iter)->RenderBoundingBox(pCamera);
	}
	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->RenderBoundingBox(pCamera);
	}
}

CBaseObject *CAniShader::PickObjectByRayIntersection(
	XMFLOAT3& pickPosition, XMFLOAT4X4& xmf4x4View, float &nearHitDistance)
{
	bool intersected = 0;

	nearHitDistance = FLT_MAX;
	float hitDistance = FLT_MAX;
	CBaseObject *pSelectedObject{ NULL };

	for (auto& iter = m_blueObjects.begin(); iter != m_blueObjects.end(); ++iter)
	{
		intersected = (*iter)->PickObjectByRayIntersection(pickPosition, xmf4x4View, hitDistance);
		if (intersected && (hitDistance < nearHitDistance))
		{
			nearHitDistance = hitDistance;
			pSelectedObject = (*iter);
		}
	}

	for (auto& iter = m_redObjects.begin(); iter != m_redObjects.end(); ++iter)
	{
		intersected = (*iter)->PickObjectByRayIntersection(pickPosition, xmf4x4View, hitDistance);
		if (intersected && (hitDistance < nearHitDistance))
		{
			nearHitDistance = hitDistance;
			pSelectedObject = (*iter);
		}
	}

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

bool CAniShader::OnProcessKeyInput(UCHAR* pKeyBuffer)
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
		SpawnMinion(m_pCreateMgr, Minion_Species::Blue_Up);
		//미니언 생성 패킷 조립, 전송
		CS_MsgMoCreate p;
		p.type = CS_PUT_MINION;
		p.size = sizeof(p);
		m_pNetwork->SendPacket(m_pNetwork->m_myid, &p);
		m_pNetwork->ReadPacket(m_pNetwork->m_mysocket, NULL);

	}
	if (GetAsyncKeyState('B') & 0x0001)
	{
		SpawnMinion(m_pCreateMgr, Minion_Species::Blue_Down);
		CS_MsgMoCreate p;
		p.type = CS_PUT_MINION;
		p.size = sizeof(p);
		m_pNetwork->SendPacket(m_pNetwork->m_myid, &p);
		m_pNetwork->ReadPacket(m_pNetwork->m_mysocket, NULL);
	}
	if (GetAsyncKeyState('V') & 0x0001)
	{
		SpawnMinion(m_pCreateMgr, Minion_Species::Red_Up);
		CS_MsgMoCreate p;
		p.type = CS_PUT_MINION;
		p.size = sizeof(p);
		m_pNetwork->SendPacket(m_pNetwork->m_myid, &p);
		m_pNetwork->ReadPacket(m_pNetwork->m_mysocket, NULL);
	}
	if (GetAsyncKeyState('C') & 0x0001)
	{
		SpawnMinion(m_pCreateMgr, Minion_Species::Red_Down);
		CS_MsgMoCreate p;
		p.type = CS_PUT_MINION;
		p.size = sizeof(p);
		m_pNetwork->SendPacket(m_pNetwork->m_myid, &p);
		m_pNetwork->ReadPacket(m_pNetwork->m_mysocket, NULL);
	}
	return true;
}

void CAniShader::SetCollisionManager(CCollisionManager * manager)
{
	m_pColManager = manager;
}

void CAniShader::SetGaugeManger(CHPGaugeManager * pManger)
{
	m_pGaugeManger = pManger;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CAniShader::CreateInputLayout()
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

D3D12_SHADER_BYTECODE CAniShader::CreateVertexShader(ID3DBlob **ppShaderBlob)
{
	//./Code/04.Shaders/99.GraphicsShader/
#if USE_INSTANCING
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "VSTexturedLightingInstancing", "vs_5_1", ppShaderBlob));
#else
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "VSBone", "vs_5_1", ppShaderBlob));
#endif
}

D3D12_SHADER_BYTECODE CAniShader::CreatePixelShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "PSBone", "ps_5_1", ppShaderBlob));
}

void CAniShader::CreateShader(CCreateMgr *pCreateMgr)
{
	m_nPipelineStates = 2;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	for (int i = 0; i < m_nPipelineStates; ++i)
	{
		m_ppPipelineStates[i] = NULL;
	}

	m_nHeaps = 3;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr);
	CShader::CreateBoundingBoxShader(pCreateMgr);
}

void CAniShader::CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers)
{
	HRESULT hResult;

#if USE_INSTANCING
	m_pInstanceBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		sizeof(CB_GAMEOBJECT_INFO) * nBuffers,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_GENERIC_READ,
		NULL);

	hResult = m_pInstanceBuffer->Map(0, NULL, (void **)&m_pMappedObjects);
	assert(SUCCEEDED(hResult) && "m_pInstanceBuffer->Map Failed");
#else
	UINT elementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);

	m_pConstBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		elementBytes * nBuffers,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	hResult = m_pConstBuffer->Map(0, NULL, (void **)&m_pMappedObjects);
	assert(SUCCEEDED(hResult) && "m_pConstBuffer->Map Failed");
#endif

	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	m_pBoundingBoxBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		boundingBoxElementBytes * nBuffers,
		D3D12_HEAP_TYPE_UPLOAD,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		NULL);

	hResult = m_pBoundingBoxBuffer->Map(0, NULL, (void **)&m_pMappedBoundingBoxes);
	assert(SUCCEEDED(hResult) && "m_pBoundingBoxBuffer->Map Failed");
}

void CAniShader::BuildObjects(CCreateMgr *pCreateMgr, void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;
	//int xObjects = 10, yObjects = 0, zObjects = 10, i = 0;
	//int xObjects = 0, yObjects = 0, zObjects = 0, i = 0;

	//m_nObjects = (xObjects + 1) * (yObjects + 1) * (zObjects + 1);
	//m_ppObjects = new CBaseObject*[m_nObjects];

#if USE_INSTANCING
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 0, 2);
	CreateShaderVariables(pCreateMgr);
#else
	UINT ncbElementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateShaderVariables(pCreateMgr, MAX_MINION);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, MAX_MINION, 4);
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, MAX_MINION, 0, 1);
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, MAX_MINION, 4, 2);

	CreateConstantBufferViews(pCreateMgr, MAX_MINION, m_pConstBuffer, ncbElementBytes, 0);
	CreateConstantBufferViews(pCreateMgr, MAX_MINION, m_pBoundingBoxBuffer, boundingBoxElementBytes, 1);
	CreateConstantBufferViews(pCreateMgr, MAX_MINION, m_pConstBuffer, ncbElementBytes, 2);

#endif

#if USE_BATCH_MATERIAL
	m_nMaterials = 2;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	// Blue
	m_ppMaterials[0] = Materials::CreateMinionMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[0]->SetAlbedo(XMFLOAT4(0.6, 0.6, 1.0, 1.0));
	// Red
	m_ppMaterials[1] = Materials::CreateMinionMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[2], &m_psrvGPUDescriptorStartHandle[2]);
	m_ppMaterials[1]->SetAlbedo(XMFLOAT4(1.0, 0.6, 0.6, 1.0));
#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif

	m_pWeapons[0] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Minion//Weapon//Minion_Sword.meshinfo");
	m_pWeapons[1] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Minion//Weapon//Minion_Staff.meshinfo");
	m_pWeapons[2] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Minion//Weapon//Minion_Bow2.meshinfo");

	for (int j = 0; j < 3; ++j) {
		m_pWeapons[j]->AddRef();
	}

	CreatePathes();
	SpawnMinion(pCreateMgr, Minion_Species::Data_Prepare);
	//CSkinnedMesh *pMinionMesh = new CSkinnedMesh(pCreateMgr, "Resource//3D//Minion//Mesh//Minion.meshinfo");

	//CSkeleton *pSIdle = new CSkeleton("Resource//3D//Minion//Animation//Sword//Minion_S_Idle.aniinfo");
	//CSkeleton *pSAtk1 = new CSkeleton("Resource//3D//Minion//Animation//Sword//Minion_S_Attack1.aniinfo");
	//CSkeleton *pSAtk2 = new CSkeleton("Resource//3D//Minion//Animation//Sword//Minion_S_Attack2.aniinfo");
	//CSkeleton *pSWalkStart = new CSkeleton("Resource//3D//Minion//Animation//Sword//Minion_S_WalkStart.aniinfo");
	//CSkeleton *pSWalk = new CSkeleton("Resource//3D//Minion//Animation//Sword//Minion_S_Walk.aniinfo");

	//CSkeleton *pBIdle = new CSkeleton("Resource//3D//Minion//Animation//Bow//Minion_B_Idle.aniinfo");
	//CSkeleton *pBAtk = new CSkeleton("Resource//3D//Minion//Animation//Bow//Minion_B_Attack.aniinfo");
	//CSkeleton *pBWalkStart = new CSkeleton("Resource//3D//Minion//Animation//Bow//Minion_B_WalkStart.aniinfo");
	//CSkeleton *pBWalk = new CSkeleton("Resource//3D//Minion//Animation//Bow//Minion_B_Walk.aniinfo");

	//CSkeleton *pMIdle = new CSkeleton("Resource//3D//Minion//Animation//Magic//Minion_M_Idle.aniinfo");
	//CSkeleton *pMAtk1 = new CSkeleton("Resource//3D//Minion//Animation//Magic//Minion_M_Attack1.aniinfo");
	//CSkeleton *pMAtk2 = new CSkeleton("Resource//3D//Minion//Animation//Magic//Minion_M_Attack2.aniinfo");
	//CSkeleton *pMWalkStart = new CSkeleton("Resource//3D//Minion//Animation//Magic//Minion_M_WalkStart.aniinfo");
	//CSkeleton *pMWalk = new CSkeleton("Resource//3D//Minion//Animation//Magic//Minion_M_Walk.aniinfo");


	//CSkeleton *pDie = new CSkeleton("Resource//3D//Minion//Animation//Minion_Die.aniinfo");


	//pMinionMesh->SetBoundingBox(
	//	XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(4)),
	//	XMFLOAT3(CONVERT_PaperUnit_to_InG(1.5), CONVERT_PaperUnit_to_InG(1.5), CONVERT_PaperUnit_to_InG(3.5)));


	//float fxPitch = 12.0f * 5.f;
	//float fyPitch = 12.0f * 5.f;
	//float fzPitch = 12.0f * 5.f;

	//UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	//CMinion *pMinionObject = NULL;


//	for (int y = 0; y <= yObjects; y++)
//	{
//		for (int z = 0; z <= zObjects; z++)
//		{
//			for (int x = 0; x <= xObjects; x++)
//			{
//				switch (z)
//				{
//				case 0:
//					pMinionObject = new CSwordMinion(pCreateMgr, 2);
//					break;
//				case 1:
//					pMinionObject = new CMagicMinion(pCreateMgr, 2);
//					break;
//				case 2:
//					pMinionObject = new CBowMinion(pCreateMgr, 2);
//					break;
//				default:
//					pMinionObject = new CSwordMinion(pCreateMgr, 2);
//					break;
//				}
//#if !USE_INSTANCING
//				pMinionObject->SetMesh(0, pMinionMesh);
//				switch (z)
//				{
//				case 0:
//					pMinionObject->SetMesh(1, m_pWeapons[0]);
//					break;
//				case 1:
//					pMinionObject->SetMesh(1, m_pWeapons[1]);
//					break;
//				case 2:
//					pMinionObject->SetMesh(1, m_pWeapons[2]);
//					break;
//				default:
//					pMinionObject->SetMesh(1, m_pWeapons[0]);
//					break;
//				}
//#endif
//#if !USE_BATCH_MATERIAL
//				pRotatingObject->SetMaterial(pCubeMaterial);
//#endif
//				pMinionObject->SetBoundingMesh(pCreateMgr,
//					CONVERT_PaperUnit_to_InG(3), CONVERT_PaperUnit_to_InG(3), CONVERT_PaperUnit_to_InG(7),
//					0, 0, -CONVERT_PaperUnit_to_InG(4));
//				pMinionObject->SetCollisionSize(CONVERT_PaperUnit_to_InG(3));
//				pMinionObject->CBaseObject::SetPosition(x * 100, y * 100, z * 100 + 3000);
//
//				switch (z)
//				{
//				case 0:
//					pMinionObject->SetSkeleton(pSIdle);
//					pMinionObject->SetSkeleton(pSAtk1);
//					pMinionObject->SetSkeleton(pSAtk2);
//					pMinionObject->SetSkeleton(pSWalkStart);
//					pMinionObject->SetSkeleton(pSWalk);
//					break;
//				case 1:
//					pMinionObject->SetSkeleton(pMIdle);
//					pMinionObject->SetSkeleton(pMAtk1);
//					pMinionObject->SetSkeleton(pMAtk2);
//					pMinionObject->SetSkeleton(pMWalkStart);
//					pMinionObject->SetSkeleton(pMWalk);
//					break;
//				case 2:
//					pMinionObject->SetSkeleton(pBIdle);
//					pMinionObject->SetSkeleton(pBAtk);
//					pMinionObject->SetSkeleton(pBWalkStart);
//					pMinionObject->SetSkeleton(pBWalk);
//					break;
//				default:
//					pMinionObject->SetSkeleton(pSIdle);
//					pMinionObject->SetSkeleton(pSAtk1);
//					pMinionObject->SetSkeleton(pSAtk2);
//					pMinionObject->SetSkeleton(pSWalkStart);
//					pMinionObject->SetSkeleton(pSWalk);
//
//					break;
//				}
//				pMinionObject->SetSpeed(CONVERT_cm_to_InG(1.805));
//				pMinionObject->SetSkeleton(pDie);
//				pMinionObject->SetTerrain(m_pTerrain);
//
//				pMinionObject->Rotate(90, 0, 0);
//
//
//#if !USE_INSTANCING
//				pMinionObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));
//				pMinionObject->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[1].ptr + (incrementSize * i));
//#endif
//				m_ppObjects[i++] = pMinionObject;
//			}
//		}
//	}

//#if USE_INSTANCING
//	m_ppObjects[0]->SetMesh(0, pCubeMesh);
//#endif
//
	//Safe_Delete(pSIdle);
	//Safe_Delete(pSAtk1);
	//Safe_Delete(pSAtk2);
	//Safe_Delete(pSWalkStart);
	//Safe_Delete(pSWalk);
	//Safe_Delete(pDie);
}

void CAniShader::ReleaseObjects()
{
	for (auto& iter = m_blueObjects.begin(); iter != m_blueObjects.end();)
	{
		iter = m_blueObjects.erase(iter);
	}
	m_blueObjects.clear();
	for (auto& iter = m_redObjects.begin(); iter != m_redObjects.end(); )
	{
		iter = m_redObjects.erase(iter);
	}
	m_redObjects.clear();

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

void CAniShader::CreatePathes()
{
	CTransformImporter transformInporter;
	transformInporter.LoadMeshData("Resource/Data/Pathes.txt");
	for (int i = 0, cnt = 0; i < 4; ++i) 
	{
		for (int j = 0; j < transformInporter.m_iKindMeshCnt[i] - 1; ++j, ++cnt) 
		{
			XMFLOAT3 from = transformInporter.m_Transform[cnt].pos;
			XMFLOAT3 to = transformInporter.m_Transform[cnt + 1].pos;
			m_pathes[i].push_back(CPathEdge(XMFLOAT2(CONVERT_Unit_to_InG(from.x), CONVERT_Unit_to_InG(from.z)), XMFLOAT2(CONVERT_Unit_to_InG(to.x), CONVERT_Unit_to_InG(to.z))));
		}
		++cnt;
	}
}

int CAniShader::GetPossibleIndex()
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

void CAniShader::SpawnMinion(CCreateMgr *pCreateMgr, Minion_Species kind)
{
	static bool setFirst{ true };
	static CSkinnedMesh *pMinionMesh = new CSkinnedMesh(pCreateMgr, "Resource//3D//Minion//Mesh//Minion.meshinfo");
	static UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };

	static CSkeleton *pSIdle = new CSkeleton("Resource//3D//Minion//Animation//Sword//Minion_S_Idle.aniinfo");
	static CSkeleton *pSAtk1 = new CSkeleton("Resource//3D//Minion//Animation//Sword//Minion_S_Attack1.aniinfo");
	static CSkeleton *pSAtk2 = new CSkeleton("Resource//3D//Minion//Animation//Sword//Minion_S_Attack2.aniinfo");
	static CSkeleton *pSWalkStart = new CSkeleton("Resource//3D//Minion//Animation//Sword//Minion_S_WalkStart.aniinfo");
	static CSkeleton *pSWalk = new CSkeleton("Resource//3D//Minion//Animation//Sword//Minion_S_Walk.aniinfo");

	static CSkeleton *pBIdle = new CSkeleton("Resource//3D//Minion//Animation//Bow//Minion_B_Idle.aniinfo");
	static CSkeleton *pBAtk = new CSkeleton("Resource//3D//Minion//Animation//Bow//Minion_B_Attack.aniinfo");
	static CSkeleton *pBWalkStart = new CSkeleton("Resource//3D//Minion//Animation//Bow//Minion_B_WalkStart.aniinfo");
	static CSkeleton *pBWalk = new CSkeleton("Resource//3D//Minion//Animation//Bow//Minion_B_Walk.aniinfo");

	static CSkeleton *pMIdle = new CSkeleton("Resource//3D//Minion//Animation//Magic//Minion_M_Idle.aniinfo");
	static CSkeleton *pMAtk1 = new CSkeleton("Resource//3D//Minion//Animation//Magic//Minion_M_Attack1.aniinfo");
	static CSkeleton *pMAtk2 = new CSkeleton("Resource//3D//Minion//Animation//Magic//Minion_M_Attack2.aniinfo");
	static CSkeleton *pMWalkStart = new CSkeleton("Resource//3D//Minion//Animation//Magic//Minion_M_WalkStart.aniinfo");
	static CSkeleton *pMWalk = new CSkeleton("Resource//3D//Minion//Animation//Magic//Minion_M_Walk.aniinfo");

	static CSkeleton *pDie = new CSkeleton("Resource//3D//Minion//Animation//Minion_Die.aniinfo");

	if (setFirst)
	{
		pMinionMesh->SetBoundingBox(
			XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(4)),
			XMFLOAT3(CONVERT_PaperUnit_to_InG(1.5), CONVERT_PaperUnit_to_InG(1.5), CONVERT_PaperUnit_to_InG(3.5)));
		pMinionMesh->AddRef();
		setFirst = false;
		return;
	}

	m_pCreateMgr->ResetCommandList();
	CMinion *pMinionObject{ NULL };

	switch (m_kind)
	{
	case 0:
		pMinionObject = new CSwordMinion(pCreateMgr, 2);
		break;
	case 1:
		pMinionObject = new CMagicMinion(pCreateMgr, 2);
		break;
	case 2:
		pMinionObject = new CBowMinion(pCreateMgr, 2);
		break;
	}

	pMinionObject->SetMesh(0, pMinionMesh);
	switch (m_kind)
	{
	case 0:
		pMinionObject->SetMesh(1, m_pWeapons[0]);
		break;
	case 1:
		pMinionObject->SetMesh(1, m_pWeapons[1]);
		break;
	case 2:
		pMinionObject->SetMesh(1, m_pWeapons[2]);
		break;
	}

	pMinionObject->SetBoundingMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(3), CONVERT_PaperUnit_to_InG(3), CONVERT_PaperUnit_to_InG(7),
		0, 0, -CONVERT_PaperUnit_to_InG(4));
	pMinionObject->SetCollisionSize(CONVERT_PaperUnit_to_InG(3));

	switch (m_kind)
	{
	case 0:
		pMinionObject->SetSkeleton(pSIdle);
		pMinionObject->SetSkeleton(pSAtk1);
		pMinionObject->SetSkeleton(pSAtk2);
		pMinionObject->SetSkeleton(pSWalkStart);
		pMinionObject->SetSkeleton(pSWalk);
		break;
	case 1:
		pMinionObject->SetSkeleton(pMIdle);
		pMinionObject->SetSkeleton(pMAtk1);
		pMinionObject->SetSkeleton(pMAtk2);
		pMinionObject->SetSkeleton(pMWalkStart);
		pMinionObject->SetSkeleton(pMWalk);
		break;
	case 2:
		pMinionObject->SetSkeleton(pBIdle);
		pMinionObject->SetSkeleton(pBAtk);
		pMinionObject->SetSkeleton(pBWalkStart);
		pMinionObject->SetSkeleton(pBWalk);
		break;
	}
	pMinionObject->SetSpeed(CONVERT_cm_to_InG(1.805));
	pMinionObject->SetSkeleton(pDie);
	pMinionObject->SetTerrain(m_pTerrain);

	pMinionObject->Rotate(90, 0, 0);

	int index = GetPossibleIndex();

	pMinionObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * index));
	pMinionObject->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[1].ptr + (incrementSize * index));

	pMinionObject->SaveIndex(index);

	pMinionObject->SetPathToGo(new Path(m_pathes[kind]));

	if (kind == Minion_Species::Blue_Up)
	{
		pMinionObject->CBaseObject::SetPosition(335, 50, 2787);
		m_blueObjects.emplace_back(pMinionObject);

	}
	else if (kind == Minion_Species::Blue_Down)
	{
		pMinionObject->CBaseObject::SetPosition(335, 0, 2238);
		m_blueObjects.emplace_back(pMinionObject);

	}
	else if (kind == Minion_Species::Red_Up)
	{
		pMinionObject->CBaseObject::SetPosition(9669, 0, 2739);
		m_redObjects.emplace_back(pMinionObject);

	}
	else if (kind == Minion_Species::Red_Down)
	{
		pMinionObject->CBaseObject::SetPosition(9663, 0, 2254);
		m_redObjects.emplace_back(pMinionObject);

	}

	m_pCreateMgr->ExecuteCommandList();
	CCollisionObject* pointer;
	CBaseObject *pCurrentObject;

	if (kind == Minion_Species::Blue_Up || kind == Minion_Species::Blue_Down) {
		m_blueObjects.back()->ReleaseUploadBuffers();

		pointer = (CCollisionObject*)(m_blueObjects.back());
		pCurrentObject = (CBaseObject*)(m_blueObjects.back());

		m_pColManager->AddCollider(pointer);
		m_pGaugeManger->AddMinionObject(pCurrentObject);
	}
	else if (kind == Minion_Species::Red_Up || kind == Minion_Species::Red_Down) {
		m_redObjects.back()->ReleaseUploadBuffers();
		
		pointer = (CCollisionObject*)(m_redObjects.back());
		pCurrentObject = (CBaseObject*)(m_redObjects.back());

		m_pColManager->AddCollider(pointer);
		m_pGaugeManger->AddMinionObject(pCurrentObject);
	}
}
