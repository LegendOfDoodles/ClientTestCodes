#include "stdafx.h"
#include "StaticObjectShader.h"
#include "05.Objects/02.AnimatedObject/AnimatedObject.h"
#include "05.Objects/05.Minion/Minion.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/03.Terrain/HeightMapTerrain.h"

/// <summary>
/// 목적: 스테틱 오브젝트 그리기 용도의 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-04-24
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

void CStaticObjectShader::Render(CCamera *pCamera)
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
	static float R = 0.0f;
	static float M = 0.0f;

	if (pKeyBuffer['U'] & 0xF0)
	{
		R -= 0.1f;
		if (R < 0.0f) R = 0.0f;
		m_pMaterial->SetRoughness(R);
		return true;
	}
	if (pKeyBuffer['I'] & 0xF0)
	{
		R += 0.1f;
		if (R > 1.0f) R = 1.0f;
		m_pMaterial->SetRoughness(R);
		return true;
	}
	if (pKeyBuffer['O'] & 0xF0)
	{
		M -= 0.1f;
		if (M < 0.0f) M = 0.0f;
		m_pMaterial->SetMetalic(M);
		return true;
	}
	if (pKeyBuffer['P'] & 0xF0)
	{
		M += 0.1f;
		if (M > 1.0f) M = 1.0f;
		m_pMaterial->SetMetalic(M);
		return true;
	}

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
	//./Code/04.Shaders/99.GraphicsShader/
#if USE_INSTANCING
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "VSTexturedLightingInstancing", "vs_5_1", ppShaderBlob));
#else
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "VSTexturedLighting", "vs_5_1", ppShaderBlob));
#endif
}

D3D12_SHADER_BYTECODE CStaticObjectShader::CreatePixelShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "PSTexturedLighting", "ps_5_1", ppShaderBlob));
}

void CStaticObjectShader::CreateShader(CCreateMgr *pCreateMgr)
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

void CStaticObjectShader::CreateShaderVariables(CCreateMgr *pCreateMgr)
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
	UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

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

void CStaticObjectShader::BuildObjects(CCreateMgr *pCreateMgr, void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;

	m_nObjects = 1;
	m_ppObjects = new CBaseObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 0);
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 0, 1);
	CreateShaderVariables(pCreateMgr);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer, ncbElementBytes, 0);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pBoundingBoxBuffer, boundingBoxElementBytes, 1);

#if USE_BATCH_MATERIAL
	m_pMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif

}

void CStaticObjectShader::ReleaseObjects()
{
	if (!m_ppObjects) return;

	for (int j = 0; j < m_nObjects; j++)
	{
		Safe_Delete(m_ppObjects[j]);
	}
	Safe_Delete_Array(m_ppObjects);

#if USE_BATCH_MATERIAL
	Safe_Delete(m_pMaterial);
#endif
}


//////////////////////////////////////////////////////////////////////////////////////////////////
CAniShader::CAniShader(CCreateMgr *pCreateMgr)
	: CShader(pCreateMgr)
{
}

CAniShader::~CAniShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CAniShader::ReleaseUploadBuffers()
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

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedBoundingBoxes + (i * boundingBoxElementBytes));

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CAniShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CAniShader::Render(CCamera *pCamera)
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

void CAniShader::RenderBoundingBox(CCamera * pCamera)
{
	CShader::RenderBoundingBox(pCamera);

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

	if (pKeyBuffer['U'] & 0xF0)
	{
		R -= 0.1f;
		if (R < 0.0f) R = 0.0f;
		m_pMaterial->SetRoughness(R);
	}
	if (pKeyBuffer['I'] & 0xF0)
	{
		R += 0.1f;
		if (R > 1.0f) R = 1.0f;
		m_pMaterial->SetRoughness(R);
	}
	if (pKeyBuffer['O'] & 0xF0)
	{
		M -= 0.1f;
		if (M < 0.0f) M = 0.0f;
		m_pMaterial->SetMetalic(M);
	}
	if (pKeyBuffer['P'] & 0xF0)
	{
		M += 0.1f;
		if (M > 1.0f) M = 1.0f;
		m_pMaterial->SetMetalic(M);
	}
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
		for (int i = 0; i < m_nObjects; ++i) {
			CMinion* obj = dynamic_cast<CMinion*>(m_ppObjects[i]);
			obj->AniStateSet();
		}
	}
	return true;
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

	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr);
	CShader::CreateBoundingBoxShader(pCreateMgr);
}

void CAniShader::CreateShaderVariables(CCreateMgr *pCreateMgr)
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

void CAniShader::BuildObjects(CCreateMgr *pCreateMgr, void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;
	int xObjects = 10, yObjects = 0, zObjects = 2, i = 0;

	m_nObjects = (xObjects + 1) * (yObjects + 1) * (zObjects + 1);
	m_ppObjects = new CBaseObject*[m_nObjects];

#if USE_INSTANCING
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 0, 2);
	CreateShaderVariables(pCreateMgr);
#else
	UINT ncbElementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 0);
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 0, 1);
	CreateShaderVariables(pCreateMgr);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer, ncbElementBytes, 0);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pBoundingBoxBuffer, boundingBoxElementBytes, 1);
#endif

#if USE_BATCH_MATERIAL
	m_pMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif

	CSkinnedMesh *pMinionMesh = new CSkinnedMesh(pCreateMgr, "Resource//3D//Minion//Mesh//Minion.meshinfo");
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


	pMinionMesh->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(4)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(1.5), CONVERT_PaperUnit_to_InG(1.5), CONVERT_PaperUnit_to_InG(3.5)));


	float fxPitch = 12.0f * 5.f;
	float fyPitch = 12.0f * 5.f;
	float fzPitch = 12.0f * 5.f;

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CMinion *pMinionObject = NULL;

	for (int i = 0; i < 3; ++i) {
		m_pWeapons[i]->AddRef();
	}


	for (int y = 0; y <= yObjects; y++)
	{
		for (int z = 0; z <= zObjects; z++)
		{
			for (int x = 0; x <= xObjects; x++)
			{
				switch (z)
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
				default:
					pMinionObject = new CSwordMinion(pCreateMgr, 2);
					break;
				}
#if !USE_INSTANCING
				pMinionObject->SetMesh(0, pMinionMesh);
				switch (z)
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
				default:
					pMinionObject->SetMesh(1, m_pWeapons[0]);
					break;
			}
#endif
#if !USE_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				pMinionObject->SetBoundingMesh(pCreateMgr,
					CONVERT_PaperUnit_to_InG(3), CONVERT_PaperUnit_to_InG(3), CONVERT_PaperUnit_to_InG(7),
					0, 0, -CONVERT_PaperUnit_to_InG(4));
				pMinionObject->CBaseObject::SetPosition(x * 30, y * 100, z * 100);

				switch (z)
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
				default:
					pMinionObject->SetSkeleton(pSIdle);
					pMinionObject->SetSkeleton(pSAtk1);
					pMinionObject->SetSkeleton(pSAtk2);
					pMinionObject->SetSkeleton(pSWalkStart);
					pMinionObject->SetSkeleton(pSWalk);

					break;
				}

				pMinionObject->SetSkeleton(pDie);
				pMinionObject->SetTerrain(m_pTerrain);

				pMinionObject->Rotate(90, 0, 0);


#if !USE_INSTANCING
				pMinionObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));
				pMinionObject->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[1].ptr + (incrementSize * i));
#endif
				m_ppObjects[i++] = pMinionObject;
		}
	}
}







#if USE_INSTANCING
	m_ppObjects[0]->SetMesh(0, pCubeMesh);
#endif



	Safe_Delete(pSIdle);
	Safe_Delete(pSAtk1);
	Safe_Delete(pSAtk2);
	Safe_Delete(pSWalkStart);
	Safe_Delete(pSWalk);
	Safe_Delete(pDie);
}

void CAniShader::ReleaseObjects()
{
	if (!m_ppObjects) return;

	for (int j = 0; j < m_nObjects; j++)
	{
		Safe_Delete(m_ppObjects[j]);
	}
	Safe_Delete_Array(m_ppObjects);


#if USE_BATCH_MATERIAL
	Safe_Delete(m_pMaterial);
#endif
}