#include "stdafx.h"
#include "FlyingShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/11.FlyingObject/FlyingObject.h"
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
void CFlyingShader::ReleaseUploadBuffers()
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

	for (int j = 0; j < m_nMesh; j++)
	{
		m_pMeshes[j]->ReleaseUploadBuffers();
	}
}

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
	UNREFERENCED_PARAMETER(pCamera);
	//int cnt{ 0 };
	//for (int i = 0; i < m_nMaterials; ++i)
	//{
	//	for (int j = 0; j < m_meshCounts[i]; ++j, ++cnt)
	//	{
	//		if (j == 0)
	//		{
	//			CShader::Render(pCamera, i);
	//			m_ppMaterials[i]->UpdateShaderVariables();
	//		}
	//		if (m_ppObjects[cnt]) m_ppObjects[cnt]->Render(pCamera);
	//	}
	//}
}

void CFlyingShader::SpawnFlyingObject(XMFLOAT3 position, XMFLOAT3 direction, TeamType teamType, FlyingObjectType objectType)
{
	int idx{ GetPossibleIndex(objectType) };
	if (idx != NONE)
	{
		// Warning! 오브젝트 생성 부분 추가 필요
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

	m_nHeaps = m_nMesh;
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

	m_ppObjects = new CCollisionObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	int accCnt{ 0 };

	CreateShaderVariables(pCreateMgr, ncbElementBytes, m_nObjects);
	for (int i = 0; i < m_nHeaps; ++i)
	{
		m_objectsIndices[objectOrder[i]].m_begIndex = accCnt;
		CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_objectsMaxCount[objectOrder[i]], 1, i);
		CreateConstantBufferViews(pCreateMgr, m_objectsMaxCount[objectOrder[i]], m_pConstBuffer.Get(), ncbElementBytes, accCnt, i);
		accCnt += m_objectsMaxCount[objectOrder[i]];
		m_objectsIndices[objectOrder[i]].m_endIndex = accCnt;
	}
	
#if USE_BATCH_MATERIAL
	m_nMaterials = m_nMesh;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreateTresureBoxMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif

	m_srvIncrementSize = pCreateMgr->GetCbvSrvDescriptorIncrementSize();

	// 필요한 메쉬 저장
	m_pMeshes[0] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//NexusTower//Treasure Box Nexus(UV).meshinfo");

	m_pMeshes[0]->AddRef();

	// 오브젝트 생성
	CFlyingObject *pObject{ NULL };
	for (int j = 0; j < m_nObjects; ++j) {
		pObject = new CFlyingObject(pCreateMgr);

		pObject->SetTeam(TeamType::None);
		pObject->Rotate(0, 180, 0);

		pObject->SetStatic(StaticType::Move);

		m_ppObjects[j] = pObject;
	}
}

void CFlyingShader::ReleaseObjects()
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
			Safe_Delete(m_ppMaterials[i]);
		}
		Safe_Delete_Array(m_ppMaterials);
	}
#endif

	for (int j = 0; j < m_nMesh; j++)
	{
		m_pMeshes[j]->Release();
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