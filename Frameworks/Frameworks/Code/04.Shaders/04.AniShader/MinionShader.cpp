#include "stdafx.h"
#include "MinionShader.h"
#include "05.Objects/06.Minion/Minion.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
#include "05.Objects/99.Material/Material.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"
#include "00.Global/01.Utility/06.HPGaugeManager/HPGaugeManager.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"
#include "00.Global/02.AI/00.FSMMgr/FSMMgr.h"

/// <summary>
/// 목적: 움직이는 오브젝트 관리 및 그리기 용도
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-07-31
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CMinionShader::CMinionShader(shared_ptr<CCreateMgr> pCreateMgr) : CShader(pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;
}

CMinionShader::~CMinionShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CMinionShader::Initialize(shared_ptr<CCreateMgr> pCreateMgr, void *pContext)
{
	CreateShader(pCreateMgr, RENDER_TARGET_BUFFER_CNT, true, true);
	BuildObjects(pCreateMgr, pContext);
}

void CMinionShader::ReleaseUploadBuffers()
{
#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
#endif
}

void CMinionShader::UpdateShaderVariables(int opt)
{
	UNREFERENCED_PARAMETER(opt);
	static UINT elementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);

	for (auto iter = m_blueObjects.begin(); iter != m_blueObjects.end(); ++iter)
	{
		CB_ANIOBJECT_INFO *pMappedObject = (CB_ANIOBJECT_INFO *)(m_pMappedObjects + ((*iter)->GetIndex() * elementBytes));
		XMFLOAT4X4 tmp[128];
		memcpy(tmp, (*iter)->GetFrameMatrix(), sizeof(XMFLOAT4X4) * 128);
		memcpy(pMappedObject->m_xmf4x4Frame, tmp, sizeof(XMFLOAT4X4) * 128);

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World0,
			XMMatrixTranspose(XMLoadFloat4x4((*iter)->GetWorldMatrix())));
	}
	for (auto iter = m_redObjects.begin(); iter != m_redObjects.end(); ++iter)
	{
		CB_ANIOBJECT_INFO *pMappedObject = (CB_ANIOBJECT_INFO *)(m_pMappedObjects + ((*iter)->GetIndex() * elementBytes));
		XMFLOAT4X4 tmp[128];
		memcpy(tmp, (*iter)->GetFrameMatrix(), sizeof(XMFLOAT4X4) * 128);
		memcpy(pMappedObject->m_xmf4x4Frame, tmp, sizeof(XMFLOAT4X4) * 128);

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World0,
			XMMatrixTranspose(XMLoadFloat4x4((*iter)->GetWorldMatrix())));
	}
}

void CMinionShader::UpdateBoundingBoxShaderVariables()
{
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (auto iter = m_blueObjects.begin(); iter != m_blueObjects.end(); ++iter)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedBoundingBoxes + ((*iter)->GetIndex() * boundingBoxElementBytes));

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4((*iter)->GetWorldMatrix())));
	}
	for (auto iter = m_redObjects.begin(); iter != m_redObjects.end(); ++iter)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedBoundingBoxes + ((*iter)->GetIndex() * boundingBoxElementBytes));

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4((*iter)->GetWorldMatrix())));
	}
}

void CMinionShader::AnimateObjects(float timeElapsed)
{
	m_spawnTime += timeElapsed;

	//if (m_spawnTime >= 0.0f && m_spawnTime <= 5.0f)
	//{
	//	bool spawned{ false };
	//	for (float time = m_spawnTime - m_preSpawnTime; time >= 0.25; time -= 0.25)
	//	{
	//		spawned = true;
	//		SpawnMinion();
	//	}
	//	if(spawned) m_preSpawnTime = m_spawnTime;
	//}

	//if (m_spawnTime >= 30.0f)
	//{
	//	m_spawnTime -= 30.0f;
	//	m_preSpawnTime = -0.25f;
	//}

	for (auto iter = m_blueObjects.begin(); iter != m_blueObjects.end();)
	{
		if ((*iter)->GetState() == States::Remove)
		{
			CCollisionObject* temp{ *iter };
			ResetPossibleIndex(temp->GetIndex());
			Safe_Delete(temp);

			iter = m_blueObjects.erase(iter);
		}
		else
		{
			m_pFSMMgr->Update(timeElapsed, (*iter));
			++iter;
		}
	}

	for (auto iter = m_redObjects.begin(); iter != m_redObjects.end();)
	{
		if ((*iter)->GetState() == States::Remove)
		{
			CCollisionObject* temp{ *iter };
			ResetPossibleIndex(temp->GetIndex());
			Safe_Delete(temp);

			iter = m_redObjects.erase(iter);
		}
		else
		{
			m_pFSMMgr->Update(timeElapsed, (*iter));
			++iter;
		}
	}
}

void CMinionShader::Render(CCamera *pCamera)
{
	CShader::Render(pCamera, 0);

	if (m_ppMaterials) m_ppMaterials[0]->UpdateShaderVariables();
	for (auto iter = m_blueObjects.begin(); iter != m_blueObjects.end(); ++iter)
	{
		(*iter)->Render(pCamera);
	}

	if (m_ppMaterials) m_ppMaterials[1]->UpdateShaderVariables();
	for (auto iter = m_redObjects.begin(); iter != m_redObjects.end(); ++iter)
	{
		(*iter)->Render(pCamera);
	}
}

void CMinionShader::RenderBoundingBox(CCamera * pCamera)
{
	CShader::RenderBoundingBox(pCamera);
	for (auto iter = m_blueObjects.begin(); iter != m_blueObjects.end(); ++iter)
	{
		(*iter)->RenderBoundingBox(pCamera);
	}
	for (auto iter = m_redObjects.begin(); iter != m_redObjects.end(); ++iter)
	{
		(*iter)->RenderBoundingBox(pCamera);
	}
}

void CMinionShader::RenderShadow(CCamera * pCamera)
{
	CShader::Render(pCamera, 0, 2);
	for (auto iter = m_blueObjects.begin(); iter != m_blueObjects.end(); ++iter)
	{
		(*iter)->Render(pCamera);
	}

	for (auto iter = m_redObjects.begin(); iter != m_redObjects.end(); ++iter)
	{
		(*iter)->Render(pCamera);
	}
}

CBaseObject *CMinionShader::PickObjectByRayIntersection(
	XMFLOAT3& pickPosition, XMFLOAT4X4& xmf4x4View, float &nearHitDistance)
{
	bool intersected = 0;

	nearHitDistance = FLT_MAX;
	float hitDistance = FLT_MAX;
	CCollisionObject *pSelectedObject{ NULL };

	for (auto iter = m_blueObjects.begin(); iter != m_blueObjects.end(); ++iter)
	{
		intersected = (*iter)->PickObjectByRayIntersection(pickPosition, xmf4x4View, hitDistance);
		if (intersected && (hitDistance < nearHitDistance))
		{
			nearHitDistance = hitDistance;
			pSelectedObject = (*iter);
		}
	}

	for (auto iter = m_redObjects.begin(); iter != m_redObjects.end(); ++iter)
	{
		intersected = (*iter)->PickObjectByRayIntersection(pickPosition, xmf4x4View, hitDistance);
		if (intersected && (hitDistance < nearHitDistance))
		{
			nearHitDistance = hitDistance;
			pSelectedObject = (*iter);
		}
	}

	return(pSelectedObject);
}

bool CMinionShader::OnProcessKeyInput(UCHAR* pKeyBuffer)
{
	UNREFERENCED_PARAMETER(pKeyBuffer);

	static float R = 0.0f;
	static float M = 0.0f;

	if (GetAsyncKeyState('M') & 0x0001)
	{
		m_nWeaponState++;
		if (m_nWeaponState >= 3)m_nWeaponState = 0;

	}
	if (GetAsyncKeyState('N') & 0x0001)
	{
		SpawnMinion();
	}

	return true;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CMinionShader::CreateInputLayout()
{
	UINT nInputElementDescs = 6;
	D3D12_INPUT_ELEMENT_DESC *pInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pInputElementDescs[0] = {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[1] = {
		"NORMAL",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[2] = {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	pInputElementDescs[3] = {
		"WEIGHTS",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[4] = {
		"BONEINDICES",
		0,
		DXGI_FORMAT_R8G8B8A8_UINT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	pInputElementDescs[5] = {
		"TANGENT",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CMinionShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", 
		"VSBone",
		"vs_5_1", 
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CMinionShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", 
		"PSBone",
		"ps_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CMinionShader::CreateShadowVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/ShadowShader.hlsl",
		"VSBone",
		"vs_5_1",
		pShaderBlob));
}

void CMinionShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 3;

	m_nHeaps = 2;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CMinionShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;

	UINT ncbElementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateShaderVariables(pCreateMgr, ncbElementBytes, MAX_MINION, true, boundingBoxElementBytes, MAX_MINION);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, MAX_MINION, 1, 0);
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, MAX_MINION, 0, 1);

	CreateConstantBufferViews(pCreateMgr, MAX_MINION, m_pConstBuffer.Get(), ncbElementBytes, 0, 0);
	CreateConstantBufferViews(pCreateMgr, MAX_MINION, m_pBoundingBoxBuffer.Get(), boundingBoxElementBytes, 0, 1);

	SaveBoundingBoxHeapNumber(1);

#if USE_BATCH_MATERIAL
	m_nMaterials = 2;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	// Blue
	m_ppMaterials[0] = Materials::CreateMinionMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[0]->SetAlbedo(XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f));
	// Red
	m_ppMaterials[1] = Materials::CreateMinionMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[1]->SetAlbedo(XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f));
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
	SpawnMinion();
}

void CMinionShader::ReleaseObjects()
{
	for (auto iter = m_blueObjects.begin(); iter != m_blueObjects.end();)
	{
		delete(*iter);
		iter = m_blueObjects.erase(iter);
	}
	m_blueObjects.clear();
	for (auto iter = m_redObjects.begin(); iter != m_redObjects.end(); )
	{
		delete(*iter);
		iter = m_redObjects.erase(iter);
	}
	m_redObjects.clear();

#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i < m_nMaterials; ++i)
		{
			Safe_Delete(m_ppMaterials[i]);
		}
		Safe_Delete_Array(m_ppMaterials);
	}
#endif
}

void CMinionShader::CreatePathes()
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

int CMinionShader::GetPossibleIndex()
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

void CMinionShader::SpawnMinion()
{
	static bool dataPrepared{ false };
	static CSkinnedMesh minionMesh(m_pCreateMgr, "Resource//3D//Minion//Mesh//Minion.meshinfo");
	static UINT incrementSize{ m_pCreateMgr->GetCbvSrvDescriptorIncrementSize() };

	static CCubeMesh boundingBoxMesh(m_pCreateMgr,
		CONVERT_PaperUnit_to_InG(3.0f), CONVERT_PaperUnit_to_InG(1.5f), CONVERT_PaperUnit_to_InG(7.0f),
		0, 0, -CONVERT_PaperUnit_to_InG(4.0f));
	boundingBoxMesh.AddRef();

	static CSkeleton SIdle("Resource//3D//Minion//Animation//Sword//Minion_S_Idle.aniinfo");
	static CSkeleton SAtk1("Resource//3D//Minion//Animation//Sword//Minion_S_Attack1.aniinfo");
	static CSkeleton SAtk2("Resource//3D//Minion//Animation//Sword//Minion_S_Attack2.aniinfo");
	static CSkeleton SWalkStart("Resource//3D//Minion//Animation//Sword//Minion_S_WalkStart.aniinfo");
	static CSkeleton SWalk("Resource//3D//Minion//Animation//Sword//Minion_S_Walk.aniinfo");

	static CSkeleton BIdle("Resource//3D//Minion//Animation//Bow//Minion_B_Idle.aniinfo");
	static CSkeleton BAtk("Resource//3D//Minion//Animation//Bow//Minion_B_Attack.aniinfo");
	static CSkeleton BWalkStart("Resource//3D//Minion//Animation//Bow//Minion_B_WalkStart.aniinfo");
	static CSkeleton BWalk("Resource//3D//Minion//Animation//Bow//Minion_B_Walk.aniinfo");

	static CSkeleton MIdle("Resource//3D//Minion//Animation//Magic//Minion_M_Idle.aniinfo");
	static CSkeleton MAtk1("Resource//3D//Minion//Animation//Magic//Minion_M_Attack1.aniinfo");
	static CSkeleton MAtk2("Resource//3D//Minion//Animation//Magic//Minion_M_Attack2.aniinfo");
	static CSkeleton MWalkStart("Resource//3D//Minion//Animation//Magic//Minion_M_WalkStart.aniinfo");
	static CSkeleton MWalk("Resource//3D//Minion//Animation//Magic//Minion_M_Walk.aniinfo");

	static CSkeleton Die("Resource//3D//Minion//Animation//Minion_Die.aniinfo");

	if (!dataPrepared)
	{
		minionMesh.SetBoundingBox(
			XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(4.0f)),
			XMFLOAT3(CONVERT_PaperUnit_to_InG(1.5f), CONVERT_PaperUnit_to_InG(1.5f), CONVERT_PaperUnit_to_InG(3.5f)));
		minionMesh.AddRef();
		dataPrepared = true;
		return;
	}

	m_pCreateMgr->ResetCommandList();

	int kind{ 0 };
	int makeCnt{ 0 };
	for (; kind < 4; ++kind)
	{
		int index{ GetPossibleIndex() };

		if (index == NONE) break;
		CMinion *pMinionObject{ NULL };

		switch (m_kind)
		{
		case ObjectType::SwordMinion:
			pMinionObject = new CSwordMinion(m_pCreateMgr, 2);
			break;
		case ObjectType::StaffMinion:
			pMinionObject = new CMagicMinion(m_pCreateMgr, 2);
			break;
		case ObjectType::BowMinion:
			pMinionObject = new CBowMinion(m_pCreateMgr, 2);
			break;
		}

		pMinionObject->SetMesh(0, &minionMesh);
		switch (m_kind)
		{
		case ObjectType::SwordMinion:
			pMinionObject->SetMesh(1, m_pWeapons[0]);
			break;
		case ObjectType::StaffMinion:
			pMinionObject->SetMesh(1, m_pWeapons[1]);
			break;
		case ObjectType::BowMinion:
			pMinionObject->SetMesh(1, m_pWeapons[2]);
			break;
		}

		pMinionObject->SetBoundingMesh(&boundingBoxMesh);
		pMinionObject->SetCollisionSize(CONVERT_PaperUnit_to_InG(2));

		switch (m_kind)
		{
		case ObjectType::SwordMinion:
			pMinionObject->SetSkeleton(&SIdle);
			pMinionObject->SetSkeleton(&SAtk1);
			pMinionObject->SetSkeleton(&SAtk2);
			pMinionObject->SetSkeleton(&SWalkStart);
			pMinionObject->SetSkeleton(&SWalk);
			break;
		case ObjectType::StaffMinion:
			pMinionObject->SetSkeleton(&MIdle);
			pMinionObject->SetSkeleton(&MAtk1);
			pMinionObject->SetSkeleton(&MAtk2);
			pMinionObject->SetSkeleton(&MWalkStart);
			pMinionObject->SetSkeleton(&MWalk);
			break;
		case ObjectType::BowMinion:
			pMinionObject->SetSkeleton(&BIdle);
			pMinionObject->SetSkeleton(&BAtk);
			pMinionObject->SetSkeleton(&BWalkStart);
			pMinionObject->SetSkeleton(&BWalk);
			break;
		}

		pMinionObject->SetSkeleton(&Die);
		pMinionObject->SetTerrain(m_pTerrain);

		pMinionObject->Rotate(90, 0, 0);

		pMinionObject->SaveIndex(index);

		pMinionObject->SetPathToGo(new Path(m_pathes[kind]));

		XMFLOAT2 firstPos{ m_pathes[kind].front().From() };
		pMinionObject->CBaseObject::SetPosition(XMFLOAT3(firstPos.x, 0, firstPos.y));

		pMinionObject->SetCollisionManager(m_pColManager);

		pMinionObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * index));
		pMinionObject->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[1].ptr + (incrementSize * index));

		if (kind == Minion_Species::Blue_Up || kind == Minion_Species::Blue_Down)
		{
			pMinionObject->SetTeam(TeamType::Blue);
			m_blueObjects.emplace_back(pMinionObject);
		}
		else if (kind == Minion_Species::Red_Up || kind == Minion_Species::Red_Down)
		{
			pMinionObject->SetTeam(TeamType::Red);
			m_redObjects.emplace_back(pMinionObject);
		}

		makeCnt++;
	}
	m_pCreateMgr->ExecuteCommandList();

	if (!makeCnt) return;
	
	CollisionObjectList::reverse_iterator blueBegin{ m_blueObjects.rbegin() };
	CollisionObjectList::reverse_iterator redBegin{ m_redObjects.rbegin() };

	if (makeCnt > 1) blueBegin++;
	if (makeCnt > 3) redBegin++;

	for (kind = 0; makeCnt > 0; --makeCnt, ++kind)
	{
		if (kind == Minion_Species::Blue_Up || kind == Minion_Species::Blue_Down) {
			(*blueBegin)->ReleaseUploadBuffers();

			m_pColManager->AddCollider((*blueBegin));
			m_pGaugeManger->AddMinionObject((*blueBegin));
			if(blueBegin != m_blueObjects.rbegin()) --blueBegin;
		}
		else if (kind == Minion_Species::Red_Up || kind == Minion_Species::Red_Down) {
			(*redBegin)->ReleaseUploadBuffers();

			m_pColManager->AddCollider((*redBegin));
			m_pGaugeManger->AddMinionObject((*redBegin));
			if (redBegin != m_redObjects.rbegin()) --redBegin;
		}
	}
}
