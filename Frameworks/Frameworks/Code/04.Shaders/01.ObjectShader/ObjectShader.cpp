#include "stdafx.h"
#include "ObjectShader.h"
#include "05.Objects/02.RotatingObject/RotatingObject.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"

/// <summary>
/// 목적: 오브젝트 테스트 쉐이더
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-04-14
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CObjectShader::CObjectShader(CCreateMgr *pCreateMgr)
	: CShader(pCreateMgr)
{
}

CObjectShader::~CObjectShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CObjectShader::ReleaseUploadBuffers()
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

void CObjectShader::UpdateShaderVariables()
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

void CObjectShader::UpdateBoundingBoxShaderVariables()
{
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedBoundingBoxes + (i * boundingBoxElementBytes));

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CObjectShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CObjectShader::Render(CCamera *pCamera)
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

void CObjectShader::RenderBoundingBox(CCamera * pCamera)
{
	CShader::RenderBoundingBox(pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->RenderBoundingBox(pCamera);
	}
}

CBaseObject *CObjectShader::PickObjectByRayIntersection(
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

bool CObjectShader::OnProcessKeyInput(UCHAR* pKeyBuffer)
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
D3D12_INPUT_LAYOUT_DESC CObjectShader::CreateInputLayout()
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

D3D12_SHADER_BYTECODE CObjectShader::CreateVertexShader(ID3DBlob **ppShaderBlob)
{
	//./Code/04.Shaders/99.GraphicsShader/
#if USE_INSTANCING
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "VSTexturedLightingInstancing", "vs_5_1", ppShaderBlob));
#else
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "VSTexturedLighting", "vs_5_1", ppShaderBlob));
#endif
}

D3D12_SHADER_BYTECODE CObjectShader::CreatePixelShader(ID3DBlob **ppShaderBlob)
{
	return(CShader::CompileShaderFromFile(L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl", "PSTexturedLighting", "ps_5_1", ppShaderBlob));
}

void CObjectShader::CreateShader(CCreateMgr *pCreateMgr)
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

void CObjectShader::CreateShaderVariables(CCreateMgr *pCreateMgr)
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

void CObjectShader::BuildObjects(CCreateMgr *pCreateMgr, void *pContext)
{
	int xObjects = 10, yObjects = 0, zObjects =10, i = 0;

	m_nObjects = (xObjects * 2 + 1) * (yObjects * 2 + 1) * (zObjects * 2 + 1);
	m_ppObjects = new CBaseObject*[m_nObjects];

#if USE_INSTANCING
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, 0, 2);
	CreateShaderVariables(pCreateMgr);
#else
	UINT ncbElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);
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

	//CSkinnedMesh *pCubeMesh = new CSkinnedMesh(pCreateMgr, "FBXBinary//minion.meshinfo");
	CCubeMeshIlluminatedTextured* pCubeMesh = new CCubeMeshIlluminatedTextured(pCreateMgr,20,20,20);

	//CSkeleton *pSkeleton = new CSkeleton("FBXBinary//minion.aniinfo");
	//CSkeleton *pSkeleton1 = new CSkeleton("FBXBinary//minion1.aniinfo");
	//CSkeleton *pSkeleton2 = new CSkeleton("FBXBinary//minion2.aniinfo");
	float fxPitch = 12.0f * 5.f;
	float fyPitch = 12.0f * 5.f;
	float fzPitch = 12.0f * 5.f;

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CRotatingObject *pRotatingObject = NULL;
	for (int y = -yObjects; y <= yObjects; y++)
	{
		for (int z = -zObjects; z <= zObjects; z++)
		{
			for (int x = -xObjects; x <= xObjects; x++)
			{

				pRotatingObject = new CRotatingObject(pCreateMgr);
#if !USE_INSTANCING
				pRotatingObject->SetMesh(0, pCubeMesh);
#endif
#if !USE_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				pRotatingObject->SetPosition(x * 30+200, y * 100 + 100, z * 100+1000);
				//pRotatingObject->SetSkeleton(pSkeleton);
				//pRotatingObject->SetSkeleton1(pSkeleton1);
				//pRotatingObject->SetSkeleton2(pSkeleton2);
				pRotatingObject->Rotate(90, 0, 0);
#if !USE_INSTANCING
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));
				pRotatingObject->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[1].ptr + (incrementSize * i));
#endif
				m_ppObjects[i++] = pRotatingObject;
			}
		}
	}

#if USE_INSTANCING
	m_ppObjects[0]->SetMesh(0, pCubeMesh);
#endif
}

void CObjectShader::ReleaseObjects()
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
		memcpy( tmp, m_ppObjects[i]->GetFrameMatrix(), sizeof(XMFLOAT4X4) * 128);
		memcpy( pMappedObject->m_xmf4x4Frame, tmp,sizeof(XMFLOAT4X4)*128);
		
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
		for (int i = 0; i < m_nObjects; ++i) {
			CAnimatedObject* obj = dynamic_cast<CAnimatedObject*>(m_ppObjects[i]);
			obj->AniStateSet();
		}
	}
	return true;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CAniShader::CreateInputLayout()
{
	UINT nInputElementDescs =6;
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
	int xObjects = 10, yObjects = 0, zObjects = 10, i = 0;

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

	CSkinnedMesh *pCubeMesh = new CSkinnedMesh(pCreateMgr, "FBXBinary//minion.meshinfo");
	pCubeMesh->SetBoundingBox(XMFLOAT3(0.0f, 0.0f, -16.56), XMFLOAT3(12.42f, 12.42f, 28.98f));

	CSkeleton *pSkeleton = new CSkeleton("FBXBinary//minion.aniinfo");
	CSkeleton *pSkeleton1 = new CSkeleton("FBXBinary//minion1.aniinfo");
	CSkeleton *pSkeleton2 = new CSkeleton("FBXBinary//minion2.aniinfo");
	float fxPitch = 12.0f * 5.f;
	float fyPitch = 12.0f * 5.f;
	float fzPitch = 12.0f * 5.f;

	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CAnimatedObject *pRotatingObject = NULL;
	for (int y = 0; y <= yObjects; y++)
	{
		for (int z =0; z <= zObjects; z++)
		{
			for (int x = 0; x <= xObjects; x++)
			{

				pRotatingObject = new CAnimatedObject(pCreateMgr);
#if !USE_INSTANCING
				pRotatingObject->SetMesh(0, pCubeMesh);
#endif
#if !USE_BATCH_MATERIAL
				pRotatingObject->SetMaterial(pCubeMaterial);
#endif
				pRotatingObject->SetBoundingMesh(pCreateMgr, 24.84f, 12.42f, 57.96f, 0, 0, -16.56);
				pRotatingObject->SetPosition(x * 30 , y * 100, z * 100 );
				pRotatingObject->SetSkeleton(pSkeleton);
				pRotatingObject->SetSkeleton1(pSkeleton1);
				pRotatingObject->SetSkeleton2(pSkeleton2);
				pRotatingObject->Rotate(90, 0, 0);
#if !USE_INSTANCING
				pRotatingObject->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));
				pRotatingObject->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[1].ptr + (incrementSize * i));
#endif
				m_ppObjects[i++] = pRotatingObject;
			}
		}
	}

#if USE_INSTANCING
	m_ppObjects[0]->SetMesh(0, pCubeMesh);
#endif
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