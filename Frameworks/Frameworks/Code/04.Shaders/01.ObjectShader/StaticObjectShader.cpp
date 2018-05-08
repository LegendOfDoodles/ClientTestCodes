#include "stdafx.h"
#include "StaticObjectShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
#include "05.Objects/07.StaticObjects/01.Nexus/Nexus.h"
#include "05.Objects/07.StaticObjects/02.Tower/Tower.h"
#include "05.Objects/07.StaticObjects/03.Obstacle/Obstacle.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"

/// <summary>
/// 목적: 스테틱 오브젝트 그리기 용도의 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-08
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CStaticObjectShader::CStaticObjectShader(CCreateMgr *pCreateMgr)
	: CShader(pCreateMgr)
{
}

CStaticObjectShader::~CStaticObjectShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CStaticObjectShader::ReleaseUploadBuffers()
{
	if (!m_ppObjects) return;

	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->ReleaseUploadBuffers();
	}
#if USE_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->ReleaseUploadBuffers();
	if (m_ppMaterials)
	{
		for(int i=0;i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
#endif
}

void CStaticObjectShader::UpdateShaderVariables()
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
	static UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
#endif
}

void CStaticObjectShader::UpdateBoundingBoxShaderVariables()
{
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedBoundingBoxes + (i * boundingBoxElementBytes));

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CStaticObjectShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CStaticObjectShader::Render(CCamera *pCamera)
{
	CShader::Render(pCamera);
#if USE_BATCH_MATERIAL
	if (m_pMaterial) m_pMaterial->UpdateShaderVariables();
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->UpdateShaderVariables();
	}
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

void CStaticObjectShader::RenderBoundingBox(CCamera * pCamera)
{
	CShader::RenderBoundingBox(pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->RenderBoundingBox(pCamera);
	}
}

CBaseObject *CStaticObjectShader::PickObjectByRayIntersection(
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

bool CStaticObjectShader::OnProcessKeyInput(UCHAR* pKeyBuffer)
{
	//if (pKeyBuffer['U'] & 0xF0)
	//{
	//	R -= 0.1f;
	//	if (R < 0.0f) R = 0.0f;
	//	m_pMaterial->SetRoughness(R);
	//	return true;
	//}

	return true;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CStaticObjectShader::CreateInputLayout()
{
	UINT nInputElementDescs = 4;
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
		"NORMAL",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pd3dInputElementDescs[2] = {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		24,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pd3dInputElementDescs[3] = {
		"TANGENT",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		32,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pd3dInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CStaticObjectShader::CreateVertexShader(ID3DBlob **ppShaderBlob)
{
#if USE_INSTANCING
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "VSTexturedLightingInstancing", "vs_5_1", ppShaderBlob));
#else
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "VSTexturedLighting", "vs_5_1", ppShaderBlob));
#endif
}

D3D12_SHADER_BYTECODE CStaticObjectShader::CreatePixelShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "PSTexturedLightingDetail", "ps_5_1", ppShaderBlob));
}

void CStaticObjectShader::CreateShader(CCreateMgr *pCreateMgr)
{
	m_nPipelineStates = 1;
	m_ppPipelineStates = new ID3D12PipelineState*[m_nPipelineStates];

	for (int i = 0; i < m_nPipelineStates; ++i)
	{
		m_ppPipelineStates[i] = NULL;
	}

	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr);
	//CShader::CreateBoundingBoxShader(pCreateMgr);
}

void CStaticObjectShader::CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers)
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
	UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

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

void CStaticObjectShader::BuildObjects(CCreateMgr *pCreateMgr, void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;

	CTransformImporter transformInporter;

	transformInporter.LoadMeshData("Resource//Data/Setting.txt");

	m_nObjects = transformInporter.m_iTotalCnt;
	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 4);
	//CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 0, 1);
	CreateShaderVariables(pCreateMgr, m_nObjects);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer, ncbElementBytes, 0);
	//CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pBoundingBoxBuffer, boundingBoxElementBytes, 1);

#if USE_BATCH_MATERIAL
	m_nMaterials = 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreateTresureBoxMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CStaticMesh *pMeshes[17];
	//CStaticMesh *pNexus = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Nexus//Treasure Box Nexus(UV).meshinfo");
	pMeshes[0]= new CStaticMesh(pCreateMgr, "Resource//3D//Building//Eraser.meshinfo");
	pMeshes[1]= new CStaticMesh(pCreateMgr, "Resource//3D//Building//Duck.meshinfo");
	pMeshes[2]= new CStaticMesh(pCreateMgr, "Resource//3D//Building//KeumWonBo.meshinfo");
	pMeshes[3]= new CStaticMesh(pCreateMgr, "Resource//3D//Building//PencilCase.meshinfo");//***
	pMeshes[4]= new CStaticMesh(pCreateMgr, "Resource//3D//Building//Nail.meshinfo");//***
	pMeshes[5]= new CStaticMesh(pCreateMgr, "Resource//3D//Building//BlueHead.meshinfo");//***
	pMeshes[6] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//ShortPencil.meshinfo");
	pMeshes[7]= new CStaticMesh(pCreateMgr, "Resource//3D//Building//LongPencil.meshinfo");
	pMeshes[8]= new CStaticMesh(pCreateMgr, "Resource//3D//Building//Cup.meshinfo");
	pMeshes[9]= new CStaticMesh(pCreateMgr, "Resource//3D//Building//RedHead.meshinfo");
	pMeshes[10]= new CStaticMesh(pCreateMgr, "Resource//3D//Building//PenCover.meshinfo");
	pMeshes[11]= new CStaticMesh(pCreateMgr, "Resource//3D//Building//Pen.meshinfo");
	pMeshes[12]= new CStaticMesh(pCreateMgr, "Resource//3D//Building//Dice.meshinfo");
	pMeshes[13] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Book.meshinfo", transformInporter.BookScale[0]);
	pMeshes[14] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Book.meshinfo", transformInporter.BookScale[1]);
	pMeshes[15] = new CStaticMesh(pCreateMgr, "Resource//3D//Building//Book.meshinfo", transformInporter.BookScale[2]);
	pMeshes[16]=  new CStaticMesh(pCreateMgr, "Resource//3D//Building//Book.meshinfo", transformInporter.BookScale[3]);
	int cnt = 0;
	for (int i = 0; i < 17; ++i) {
		for (int j = 0; j < transformInporter.m_iKindMeshCnt[i]; ++j) {
			XMFLOAT3 pos = transformInporter.m_Transform[cnt].pos;
			XMFLOAT3 rot = transformInporter.m_Transform[cnt].rotation;
			m_ppObjects[cnt] = new CObstacle(pCreateMgr);
			m_ppObjects[cnt]->SetPosition(CONVERT_Unit_to_InG(pos.x), CONVERT_Unit_to_InG(pos.y), CONVERT_Unit_to_InG(pos.z));
			//m_ppObjects[cnt]->SetPosition(CONVERT_Unit_to_InG(i), CONVERT_Unit_to_InG(0), CONVERT_Unit_to_InG(0));
			
			m_ppObjects[cnt]->Rotate(0, 180, 0);
			m_ppObjects[cnt]->Rotate(-rot.x, rot.y,-rot.z);


			m_ppObjects[cnt]->SetMesh(0, pMeshes[i]);

			m_ppObjects[cnt]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * cnt));
			++cnt;
		}
	}	
}

void CStaticObjectShader::ReleaseObjects()
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
	for (int i = 0; i < m_nMaterials; ++i)
	{
		delete m_ppMaterials[i];
	}
	Safe_Delete(m_ppMaterials);
#endif
}
