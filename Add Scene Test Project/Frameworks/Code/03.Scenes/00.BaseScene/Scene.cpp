#include "stdafx.h"
#include "Scene.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "04.Shaders/98.BillboardShader/00.UIShader/UIShader.h"
#include "04.Shaders/98.BillboardShader/06.TitleUIShader/TitleUIShader.h"
#include "05.Objects/01.Camera/01.AOSCamera/AOSCamera.h"
#include "05.Objects/01.Camera/02.LightCamera/LightCamera.h"
#include "00.Global/01.Utility/09.SoundManager/SoundManager.h"

/// <summary>
/// 목적: 기본 씬, 인터페이스 용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-08-05
/// </summary>

#define UI_Shader m_ppShaders[0]

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CScene::CScene()
{
}

CScene::~CScene()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CScene::Initialize(shared_ptr<CCreateMgr> pCreateMgr)
{
	BuildObjects(pCreateMgr);
	CreateShaderVariables(pCreateMgr);
}

void CScene::Finalize()
{
	ReleaseObjects();
	ReleaseShaderVariables();
}

void CScene::ReleaseUploadBuffers()
{
	if (m_ppShaders)
	{
		for (int j = 0; j < m_nShaders; j++)
		{
			if (m_ppShaders[j])
			{
				m_ppShaders[j]->ReleaseUploadBuffers();
			}
		}
	}
	if (m_pCubeMap) m_pCubeMap->ReleaseUploadBuffers();
	if (m_pSketchEffect) m_pSketchEffect->ReleaseUploadBuffers();
}

void CScene::ProcessInput()
{
	UCHAR pKeyBuffer[256];

	GetKeyboardState(pKeyBuffer);

	bool continual = m_pCamera->OnProcessMouseInput(pKeyBuffer);
	if (continual) continual = m_pCamera->OnProcessKeyInput(pKeyBuffer);
	for (int i = 0; i < m_nShaders; ++i) {
		if (continual)
			continual = m_ppShaders[i]->OnProcessKeyInput(pKeyBuffer);
	}
}

void CScene::AnimateObjects(float timeElapsed)
{
	m_pCamera->Update(timeElapsed);
	m_pSoundManager->Update(timeElapsed);

	UpdateShaderVariables();

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(timeElapsed);
	}

	if (m_pCollisionManager) {
		m_pCollisionManager->Update(m_pWayFinder);

		int(*fow)[NODE_HEIGHT] = m_pCollisionManager->GetFoW();
		for (int i = 0; i < NODE_WIDTH; ++i)
			for (int j = 0; j < NODE_HEIGHT; ++j)
			{
				m_pFoW[i].m_bFoW[j] = fow[i][j];
				//m_pFoW[i].m_bFoW[j] = 1;
			}
	}
}

void CScene::Render()
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pcbLights->GetGPUVirtualAddress();
	m_pCommandList->SetGraphicsRootConstantBufferView(4, d3dcbLightsGpuVirtualAddress); //Lights

	d3dcbLightsGpuVirtualAddress = m_pcbbFoW->GetGPUVirtualAddress();
	m_pCommandList->SetGraphicsRootShaderResourceView(9, d3dcbLightsGpuVirtualAddress);

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Render(m_pCamera);
	}
	if (m_bRenderBoundingBox)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			if (m_ppShaders[i]->HasBoundingBox())
				m_ppShaders[i]->RenderBoundingBox(m_pCamera);
		}
	}
}

void CScene::RenderShadow()
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->RenderShadow(m_pLightCamera);
	}
}

void CScene::RenderWithLights()
{
	UpdateShaderVariables();
	m_pCamera->UpdateShaderVariables(1);

	if (m_pCubeMap)
	{
		m_pCommandList->SetDescriptorHeaps(1, m_pCbvSrvDescriptorHeaps[0].GetAddressOf());
		m_pCubeMap->UpdateShaderVariable(0);
	}

	if (m_pSketchEffect)
	{
		m_pCommandList->SetDescriptorHeaps(1, m_pCbvSrvDescriptorHeaps[1].GetAddressOf());
		m_pSketchEffect->UpdateShaderVariable(0);
	}

	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pcbLights->GetGPUVirtualAddress();
	m_pCommandList->SetGraphicsRootConstantBufferView(2, d3dcbLightsGpuVirtualAddress); //Lights
}

void CScene::SetViewportsAndScissorRects()
{
	m_pCamera->SetViewportsAndScissorRects();
}

void CScene::SetShadowViewportsAndScissorRects()
{
	m_pLightCamera->SetViewportsAndScissorRects();
}

void CScene::UpdateCamera()
{
	if (m_bCamChanged)
	{
		m_pCamera->Finalize();
		Safe_Delete(m_pCamera);

		if (m_bCurCamIsAOS)
		{
			m_pCamera = new CAOSCamera();
		}
		else
		{
			m_pCamera = new CCamera();
		}

		m_pCamera->Initialize(m_pCreateMgr);

		m_bCamChanged = false;
	}
	m_pCamera->UpdateShaderVariables();
}

void CScene::UpdateShadowCamera(int renderStage)
{
	if (renderStage == 0)
	{
		m_pLightCamera->UpdateShaderVariables(8);
	}
	else
	{
		m_pLightCamera->UpdateShaderVariables(6);
	}
}

void CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		//::SetCapture(hWnd);
		//m_pCamera->SavePickedPos();
		//PickObjectPointedByCursor(wParam, lParam);
		for (int i = 0; i < m_nShaders; ++i)
			m_ppShaders[i]->OnProcessMouseInput(wParam);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		//::ReleaseCapture();
		break;
	case WM_MOUSEWHEEL:
		m_pCamera->OnProcessMouseWheel(wParam, lParam);
		break;
	default:
		break;
	}
}

void CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(hWnd);
	if (nMessageID == WM_KEYUP)
	{
		OnProcessKeyUp(wParam, lParam);
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CScene::CreateCbvAndSrvDescriptorHeap(shared_ptr<CCreateMgr> pCreateMgr, int nConstantBufferViews, int nShaderResourceViews, int index)
{
	m_pCbvSrvDescriptorHeaps.resize(m_nHeaps);

	UINT incrementSize = pCreateMgr->GetCbvSrvDescriptorIncrementSize();
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc;
	descriptorHeapDesc.NumDescriptors = nConstantBufferViews + nShaderResourceViews; //CBVs + SRVs 
	descriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	descriptorHeapDesc.NodeMask = 0;
	HRESULT hResult = pCreateMgr->GetDevice()->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&m_pCbvSrvDescriptorHeaps[index]));
	ThrowIfFailed(hResult);

	m_cbvCPUDescriptorStartHandles[index] = m_pCbvSrvDescriptorHeaps[index]->GetCPUDescriptorHandleForHeapStart();
	m_cbvGPUDescriptorStartHandles[index] = m_pCbvSrvDescriptorHeaps[index]->GetGPUDescriptorHandleForHeapStart();
	m_srvCPUDescriptorStartHandles[index].ptr = m_cbvCPUDescriptorStartHandles[index].ptr + (incrementSize * nConstantBufferViews);
	m_srvGPUDescriptorStartHandles[index].ptr = m_cbvGPUDescriptorStartHandles[index].ptr + (incrementSize * nConstantBufferViews);
}

void CScene::BuildLights()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));

	m_pFoW = new FOW[NODE_WIDTH];
	::ZeroMemory(m_pFoW, sizeof(FOW)*NODE_WIDTH);

	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[0].m_color = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	m_pLights->m_pLights[0].m_direction = Vector3::Normalize(XMFLOAT3(0.5f, -0.7f, 0.5f));

	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[1].m_color = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[1].m_direction = Vector3::Normalize(XMFLOAT3(0.0f, -0.5f, -1.0f));

	m_pLights->m_pLights[2].m_bEnable = true;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_color = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[2].m_direction = Vector3::Normalize(XMFLOAT3(0.5f, -0.3f, 0.5f));

	CreateCbvAndSrvDescriptorHeap(m_pCreateMgr, 0, 1, 0);
	m_pCubeMap = Materials::CreateCubeMapMaterial(m_pCreateMgr, &m_srvCPUDescriptorStartHandles[0], &m_srvGPUDescriptorStartHandles[0]);

	CreateCbvAndSrvDescriptorHeap(m_pCreateMgr, 0, 1, 1);
	m_pSketchEffect = Materials::CreateSketchMaterial(m_pCreateMgr, &m_srvCPUDescriptorStartHandles[1], &m_srvGPUDescriptorStartHandles[1]);

	m_pLightCamera = new CLightCamera(m_pLights->m_pLights[0].m_direction);
	m_pLightCamera->Initialize(m_pCreateMgr);
}

void CScene::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;

	m_hWnd = pCreateMgr->GetHwnd();
	m_pCommandList = pCreateMgr->GetCommandList();

	m_pCamera = new CCamera();

	m_pCamera->Initialize(pCreateMgr);

	m_nShaders = 1;
	m_ppShaders = new CShader*[m_nShaders];

	//UI Shader
	UI_Shader = new CTitleUIShader(pCreateMgr);

	for (int i = 0; i < m_nShaders; ++i)
	{
		m_ppShaders[i]->Initialize(pCreateMgr, m_pCamera);
	}
	
	m_pSoundManager = shared_ptr<CSoundManager>(new CSoundManager(m_pCamera));	// Fmod System Init
	m_pSoundManager->loading();		// Sound File Load
	
	BuildLights();

}

void CScene::ReleaseObjects()
{
	if (m_pCamera)
	{
		m_pCamera->Finalize();
		Safe_Delete(m_pCamera);
	}
	if (m_pLightCamera)
	{
		m_pLightCamera->Finalize();
		Safe_Delete(m_pLightCamera);
	}
	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			if (m_ppShaders[i]) m_ppShaders[i]->Finalize();
		}
		Safe_Delete_Array(m_ppShaders);
	}
	if (m_pCubeMap) Safe_Delete(m_pCubeMap);
}

void CScene::CreateShaderVariables(shared_ptr<CCreateMgr> pCreateMgr)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pcbLights = pCreateMgr->CreateBufferResource(NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);


	ncbElementBytes = ((sizeof(FOW) + 255) & ~255); //256의 배수
	m_pcbbFoW = pCreateMgr->CreateBufferResource(NULL, ncbElementBytes*NODE_WIDTH, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_GENERIC_READ, NULL);
	m_pcbbFoW->Map(0, NULL, (void **)&m_pcbMappedFoW);
}

void CScene::ReleaseShaderVariables()
{
	if (m_pcbLights.Get())
	{
		m_pcbLights->Unmap(0, NULL);
		m_pcbLights.Reset();
	}

	if (m_pcbbFoW.Get())
	{
		m_pcbbFoW->Unmap(0, NULL);
		m_pcbbFoW.Reset();
	}

	for (int i = 0; i < m_nHeaps; ++i)
	{
		m_pCbvSrvDescriptorHeaps[i].Reset();
	}
	m_pCbvSrvDescriptorHeaps.clear();
}

void CScene::UpdateShaderVariables()
{
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));

	::memcpy(m_pcbMappedFoW, m_pFoW, sizeof(FOW)*NODE_WIDTH);
	m_pcbMappedFoW[0].m_bFoW[0] = 0;
}

void CScene::PickObjectPointedByCursor(WPARAM wParam, LPARAM lParam)
{
	int xClient{ LOWORD(lParam) }, yClient{ HIWORD(lParam) };

	XMFLOAT4X4 xmf4x4View = m_pCamera->GetViewMatrix();
	XMFLOAT4X4 xmf4x4Projection = m_pCamera->GetProjectionMatrix();
	D3D12_VIEWPORT viewport = m_pCamera->GetViewport();

	XMFLOAT3 pickPosition;
	pickPosition.x = (((2.0f * xClient) / viewport.Width) - 1) / xmf4x4Projection._11;
	pickPosition.y = -(((2.0f * yClient) / viewport.Height) - 1) / xmf4x4Projection._22;
	pickPosition.z = 1.0f;

	float hitDistance{ FLT_MAX }, nearestHitDistance{ FLT_MAX };
	CBaseObject *pIntersectedObject{ NULL };
	//m_pSelectedObject = NULL;

	for (int i = 0; i < m_nShaders; i++)
	{
		pIntersectedObject = m_ppShaders[i]->PickObjectByRayIntersection(pickPosition, xmf4x4View, hitDistance);
		if (pIntersectedObject && (hitDistance < nearestHitDistance))
		{
			nearestHitDistance = hitDistance;
			m_pSelectedObject = reinterpret_cast<CAnimatedObject*>(pIntersectedObject);
		}
	}

	if (wParam == MK_RBUTTON)
	{
		GenerateLayEndWorldPosition(pickPosition, xmf4x4View);
	}
	else if (wParam == MK_LBUTTON) {

	}
}

void CScene::GenerateLayEndWorldPosition(XMFLOAT3& pickPosition, XMFLOAT4X4& xmf4x4View)
{
	UNREFERENCED_PARAMETER(pickPosition);
	UNREFERENCED_PARAMETER(xmf4x4View);

	//XMFLOAT4X4  inverseArr = Matrix4x4::Inverse(xmf4x4View);
	//XMFLOAT3 camPosition = m_pCamera->GetPosition();
	//XMFLOAT3 layWorldPosition = Vector3::TransformCoord(pickPosition, inverseArr);
	//XMFLOAT3 layDirection = Vector3::Subtract(layWorldPosition, camPosition);
	//float yDiff = abs(camPosition.y / layDirection.y);
	//
	//m_pickWorldPosition = Vector3::Add(camPosition, Vector3::ScalarProduct(layDirection, yDiff, false));
	//
	//if (m_pSelectedObject && (m_pSelectedObject->GetState() != States::Die && m_pSelectedObject->GetState() != States::Remove))
	//{
	//	m_pSelectedObject->LookAt(m_pickWorldPosition);
	//	m_pSelectedObject->SetPathToGo(m_pWayFinder->GetPathToPosition(
	//		XMFLOAT2(m_pSelectedObject->GetPosition().x, m_pSelectedObject->GetPosition().z),
	//		XMFLOAT2(m_pickWorldPosition.x, m_pickWorldPosition.z)));
	//}
	//else {
	//	// 미 선택시 Player 0번
	//	m_pSelectedObject = (CAnimatedObject*)m_ppObjects[0];
	//}
}

// Process Keyboard Input
void CScene::OnProcessKeyUp(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	if (wParam == VK_F1)
	{
		if (!m_bCurCamIsAOS)
		{
			m_bCurCamIsAOS = true;
			m_bCamChanged = true;
		}
	}
	else if (wParam == VK_F2)
	{
		if (m_bCurCamIsAOS)
		{
			m_bCurCamIsAOS = false;
			m_bCamChanged = true;
		}
	}
	else if (wParam == VK_F3)
	{
		m_bRenderBoundingBox = !m_bRenderBoundingBox;
	}
}

