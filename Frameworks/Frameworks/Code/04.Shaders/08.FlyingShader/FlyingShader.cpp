#include "stdafx.h"
#include "FlyingShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
#include "05.Objects/07.StaticObjects/Obstacle.h"
#include "05.Objects/09.NexusTower/NexusTower.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"

/// <summary>
/// 목적: 날아다니는(화살 등) 오브젝트 그리기 용도의 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-07-03
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CFlyingShader::CFlyingShader(shared_ptr<CCreateMgr> pCreateMgr)
	: CShader(pCreateMgr)
{
}

CFlyingShader::~CFlyingShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CFlyingShader::UpdateShaderVariables(int opt)
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

void CFlyingShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CFlyingShader::Render(CCamera *pCamera)
{
	int cnt{ 0 };
	for (int i = 0; i < m_nMaterials; ++i)
	{
		for (int j = 0; j < m_meshCounts[i]; ++j, ++cnt)
		{
			if (j == 0)
			{
				CShader::Render(pCamera, i);
				m_ppMaterials[i]->UpdateShaderVariables();
			}
			if (m_ppObjects[cnt]) m_ppObjects[cnt]->Render(pCamera);
		}
	}
}

void CFlyingShader::SetColManagerToObject(shared_ptr<CCollisionManager> manager)
{
	for (int i = 0; i < m_nObjects; ++i) {

		dynamic_cast<CCollisionObject*>(m_ppObjects[i])->SetCollisionManager(manager);
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CFlyingShader::CreateInputLayout()
{
	UINT nInputElementDescs = 4;
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
		"NORMAL",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[2] = {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		24,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[3] = {
		"TANGENT",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		32,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CFlyingShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTexturedLighting",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CFlyingShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSTexturedLightingEmissive",
		"ps_5_1",
		pShaderBlob));
}

void CFlyingShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;

	m_nHeaps = 4;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CFlyingShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext)
{
	UNREFERENCED_PARAMETER(pContext);

	CTransformImporter monsterTransformImporter;
	monsterTransformImporter.LoadMeshData("Resource//Data//MonsterSetting.txt");

	// 오브젝트 순서 설정
	FlyingObjectType objectOrder[]{
		FlyingObjectType::Roider_Dumbel
	};

	// 각 오브젝트의 최대 개수 설정
	m_nObjects += m_objectsMaxCount[FlyingObjectType::Roider_Dumbel] = monsterTransformImporter.m_iKindMeshCnt[0];

	// 각 오브젝트 개수 만큼 Possible Index 생성
	m_objectsPossibleIndices = std::unique_ptr<bool[]>(new bool[m_nObjects]);
	
	// 설정된 Possible Indices를 0(false)로 초기화
	memset(m_objectsPossibleIndices.get(), false, m_nObjects * sizeof(bool));
	
	CTransformImporter transformInporter;
	transformInporter.LoadMeshData("Resource//Data//NexusTowerSetting.txt");

	m_ppObjects = new CCollisionObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	int accCnt{ 0 };

	CreateShaderVariables(pCreateMgr, ncbElementBytes, m_nObjects, true, ncbElementBytes, m_nObjects);
	for (int i = 0; i < m_nHeaps- 1; ++i)
	{
		m_objectsIndices[objectOrder[i]].m_begIndex = accCnt;
		CreateCbvAndSrvDescriptorHeaps(pCreateMgr, transformInporter.m_iKindMeshCnt[i], 1, i);
		CreateConstantBufferViews(pCreateMgr, transformInporter.m_iKindMeshCnt[i], m_pConstBuffer.Get(), ncbElementBytes, accCnt, i);
		accCnt += transformInporter.m_iKindMeshCnt[i];
		m_objectsIndices[objectOrder[i]].m_endIndex = accCnt;
	}
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 0, m_nHeaps - 1);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pBoundingBoxBuffer.Get(), ncbElementBytes, 0, m_nHeaps - 1);

#if USE_BATCH_MATERIAL
	m_nMaterials = m_nHeaps - 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreateTresureBoxMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[1] = Materials::CreateShellMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);
	m_ppMaterials[2] = Materials::CreateRoundSoapDispenserMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[2], &m_psrvGPUDescriptorStartHandle[2]);
	m_ppMaterials[3] = Materials::CreateSquareSoapDispenserMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[3], &m_psrvGPUDescriptorStartHandle[3]);
#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CStaticMesh *pMeshes[4];
	pMeshes[0] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//NexusTower//Treasure Box Nexus(UV).meshinfo");
	pMeshes[0]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(16)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(25), CONVERT_PaperUnit_to_InG(18), CONVERT_PaperUnit_to_InG(16)));

	pMeshes[1] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//NexusTower//Shell Nexus (UV).meshinfo");
	pMeshes[1]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(10)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(18), CONVERT_PaperUnit_to_InG(18), CONVERT_PaperUnit_to_InG(10)));

	pMeshes[2] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//NexusTower//Circle Soap Dispenser (UV).meshinfo");
	pMeshes[2]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(10)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(5), CONVERT_PaperUnit_to_InG(7), CONVERT_PaperUnit_to_InG(10)));

	pMeshes[3] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//NexusTower//Square Soap Dispenser ver.2 (UV).meshinfo");
	pMeshes[3]->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(10)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(5), CONVERT_PaperUnit_to_InG(7), CONVERT_PaperUnit_to_InG(10)));
	CNexusTower *pBuild = NULL;

	m_nNexus = transformInporter.m_iKindMeshCnt[0] + transformInporter.m_iKindMeshCnt[1];
	m_nTower = transformInporter.m_iKindMeshCnt[2] + transformInporter.m_iKindMeshCnt[3];

	int cnt = 0;
	for (int i = 0; i < m_nMaterials; ++i) {
		m_meshCounts[i] = transformInporter.m_iKindMeshCnt[i];
		for (int j = 0; j < transformInporter.m_iKindMeshCnt[i]; ++j) {
			XMFLOAT3 pos = transformInporter.m_Transform[cnt].pos;
			XMFLOAT3 rot = transformInporter.m_Transform[cnt].rotation;
			pBuild = new CNexusTower(pCreateMgr);
			pBuild->SetPosition(CONVERT_Unit_to_InG(pos.x), CONVERT_Unit_to_InG(pos.y), CONVERT_Unit_to_InG(pos.z));
			if (i == 0 || i == 2)
			{
				pBuild->SetTeam(TeamType::Blue);
			}
			else {
				pBuild->SetTeam(TeamType::Red);
			}
			if (i < 2) {
				pBuild->SetCollisionSize(CONVERT_PaperUnit_to_InG(40));
				pBuild->SetType(ObjectType::Nexus);
			}
			else {
				pBuild->SetCollisionSize(CONVERT_PaperUnit_to_InG(8));
				pBuild->SetType(ObjectType::FirstTower);

			}
			pBuild->Rotate(0, 180, 0);
			pBuild->Rotate(-rot.x, rot.y, -rot.z);
			pBuild->SetMesh(0, pMeshes[i]);

			pBuild->ResetCollisionLevel();
			pBuild->SetStatic(StaticType::Static);

			pBuild->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[i].ptr + (incrementSize * j));
			m_ppObjects[cnt++] = pBuild;
		}
	}
}

int CFlyingShader::GetPossibleIndex(FlyingObjectType type)
{
	for (int idx = m_objectsIndices[type].m_begIndex; idx < m_objectsIndices[type].m_endIndex; ++idx)
	{
		if (!m_objectsPossibleIndices[idx])
		{
			m_objectsPossibleIndices[idx] = true;
			return idx;
		}
	}
	return NONE;
}