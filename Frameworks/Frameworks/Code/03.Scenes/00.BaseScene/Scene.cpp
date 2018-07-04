#include "stdafx.h"
#include "Scene.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "04.Shaders/01.ObjectShader/StaticObjectShader.h"
#include "04.Shaders/02.TerrainShader/TerrainShader.h"
#include "04.Shaders/03.SkyBoxShader/SkyBoxShader.h"
#include "04.Shaders/04.AniShader/AniShader.h"
#include "04.Shaders/05.PlayerShader/PlayerShader.h"
#include "04.Shaders/06.NexusTowerShader/NexusTowerShader.h"
#include "04.Shaders/98.BillboardShader/00.UIShader/UIShader.h"
#include "04.Shaders/98.BillboardShader/01.GaugeShader/00.PlayerGaugeShader/PlayerGaugeShader.h"
#include "04.Shaders/98.BillboardShader/01.GaugeShader/01.MinionGaugeShader/MinionGaugeShader.h"
#include "04.Shaders/98.BillboardShader/01.GaugeShader/02.NexusAndTowerGaugeShader/NexusAndTowerGaugeShader.h"
#include "04.Shaders/98.BillboardShader/01.GaugeShader/03.CharacterGaugeShader/CharacterFrameGaugeShader.h"
#include "04.Shaders/98.BillboardShader/02.IconShader/00.MinimapIconShader/MinimapIconShader.h"
#include "04.Shaders/98.BillboardShader/02.IconShader/01.BuildingMinimapIconShader/BuildingMinimapIconShader.h"
#include "04.Shaders/98.BillboardShader/03.MinimapUIShader/MinimapShader.h"
#include "04.Shaders/98.BillboardShader/04.SkillShader/SkillShader.h"
#include "04.Shaders/98.BillboardShader/98.NumberShader/NumberShader.h"
#include "05.Objects/01.Camera/01.AOSCamera/AOSCamera.h"
#include "05.Objects/01.Camera/02.LightCamera/LightCamera.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"
#include "00.Global/01.Utility/06.HPGaugeManager/HPGaugeManager.h"
#include "00.Global/02.AI/00.FSMMgr/FSMMgr.h"

/// <summary>
/// 목적: 기본 씬, 인터페이스 용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-07-03
/// </summary>

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
	static UCHAR pKeyBuffer[256];

	GetKeyboardState(pKeyBuffer);

	bool continual = m_pCamera->OnProcessMouseInput(pKeyBuffer);
	if(continual) continual = m_pCamera->OnProcessKeyInput(pKeyBuffer);
	for (int i = 0; i < m_nShaders; ++i) {
		if(continual)
			continual = m_ppShaders[i]->OnProcessKeyInput(pKeyBuffer);
	}
}

void CScene::AnimateObjects(float timeElapsed)
{
	m_pCamera->Update(timeElapsed);

	UpdateShaderVariables();

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(timeElapsed);
	}

	if(m_pCollisionManager) m_pCollisionManager->Update(m_pWayFinder);
}

void CScene::Render()
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pcbLights->GetGPUVirtualAddress();
	m_pCommandList->SetGraphicsRootConstantBufferView(4, d3dcbLightsGpuVirtualAddress); //Lights

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
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pcbLights->GetGPUVirtualAddress();
	m_pCommandList->SetGraphicsRootConstantBufferView(4, d3dcbLightsGpuVirtualAddress); //Lights

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->RenderShadow(m_pLightCamera);
	}
}

void CScene::RenderWithLights()
{
	UpdateShaderVariables();
	m_pCamera->UpdateShaderVariables();

	if (m_pCubeMap)
	{
		m_pCommandList->SetDescriptorHeaps(1, &m_pCbvSrvDescriptorHeaps[0]);
		m_pCubeMap->UpdateShaderVariables();
	}

	if (m_pSketchEffect)
	{
		m_pCommandList->SetDescriptorHeaps(1, &m_pCbvSrvDescriptorHeaps[1]);
		m_pSketchEffect->UpdateShaderVariables();
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

		static_cast<CUIObjectShader*>(m_ppShaders[6])->SetCamera(m_pCamera);
		static_cast<CPlayerHPGaugeShader*>(m_ppShaders[7])->SetCamera(m_pCamera);
		static_cast<CMinionHPGaugeShader*>(m_ppShaders[8])->SetCamera(m_pCamera);

		m_bCamChanged = false;
	}
	m_pCamera->UpdateShaderVariables();
}

void CScene::UpdateShadowCamera()
{
	m_pLightCamera->UpdateShaderVariables();
}

void CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		::SetCapture(hWnd);
		m_pCamera->SavePickedPos();
		PickObjectPointedByCursor(wParam, lParam);
		for (int i = 0; i < m_nShaders; ++i)
			m_ppShaders[i]->OnProcessMouseInput(wParam);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
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

	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[0].m_color = XMFLOAT4(0.6f, 0.6f, 0.6f, 1.0f);
	m_pLights->m_pLights[0].m_direction = Vector3::Normalize(XMFLOAT3(0.3f, -0.5f, 0.7f));

	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[1].m_color = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[1].m_direction = Vector3::Normalize(XMFLOAT3(0.0, -0.6f, 1.0f));

	m_pLights->m_pLights[2].m_bEnable = true;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_color = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[2].m_direction = Vector3::Normalize(XMFLOAT3(0.8f, -0.4f, 0.7f));

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

	m_pCamera = new  CAOSCamera();

	m_pCamera->Initialize(pCreateMgr);

	m_nShaders = 15;
	m_ppShaders = new CShader*[m_nShaders];
	m_ppShaders[0] = new CSkyBoxShader(pCreateMgr);
	CTerrainShader* pTerrainShader = new CTerrainShader(pCreateMgr);
	m_ppShaders[1] = pTerrainShader;
	m_ppShaders[2] = new CAniShader(pCreateMgr);
	m_ppShaders[3] = new CPlayerShader(pCreateMgr);
	m_ppShaders[4] = new CStaticObjectShader(pCreateMgr);
	m_ppShaders[5] = new CNexusTowerShader(pCreateMgr);
	
	//UI Shader
	m_ppShaders[6] = new CUIObjectShader(pCreateMgr);
	m_ppShaders[7] = new CPlayerHPGaugeShader(pCreateMgr);
	m_ppShaders[8] = new CMinionHPGaugeShader(pCreateMgr);
	m_ppShaders[9] = new CMinimapIconShader(pCreateMgr);
	m_ppShaders[10] = new CBuildingMinimapIconShader(pCreateMgr);
	m_ppShaders[11] = new CharacterFrameGaugeShader(pCreateMgr);
	m_ppShaders[12] = new CMinimapShader(pCreateMgr);
	m_ppShaders[13] = new CSkillShader(pCreateMgr);
	m_ppShaders[14] = new CNumberShader(pCreateMgr);


	for (int i = 0; i < 2; ++i)
	{
		m_ppShaders[i]->Initialize(pCreateMgr);
	}

	for (int i = 2; i < m_nShaders - 9; ++i)
	{
		m_ppShaders[i]->Initialize(pCreateMgr, pTerrainShader->GetTerrain());
	}
	
	// UI Shaders Initialize
	((CPlayerHPGaugeShader*)m_ppShaders[7])->SetPlayerCnt(((CPlayerShader *)m_ppShaders[3])->GetObjectCount());
	((CPlayerHPGaugeShader*)m_ppShaders[7])->SetPlayer(((CPlayerShader *)m_ppShaders[3])->GetCollisionObjects());
	((CPlayerHPGaugeShader*)m_ppShaders[7])->SetNexusAndTowerCnt(((CNexusTowerShader *)m_ppShaders[5])->GetObjectCount());
	((CPlayerHPGaugeShader*)m_ppShaders[7])->SetNexusAndTower(((CNexusTowerShader *)m_ppShaders[5])->GetCollisionObjects());

	((CMinimapIconShader*)m_ppShaders[9])->SetPlayerCnt(((CPlayerShader *)m_ppShaders[3])->GetObjectCount());
	((CMinimapIconShader*)m_ppShaders[9])->SetPlayer(((CPlayerShader *)m_ppShaders[3])->GetCollisionObjects());

	((CBuildingMinimapIconShader*)m_ppShaders[10])->SetNexusAndTowerCnt(((CNexusTowerShader *)m_ppShaders[5])->GetObjectCount());
	((CBuildingMinimapIconShader*)m_ppShaders[10])->SetNexusAndTower(((CNexusTowerShader *)m_ppShaders[5])->GetCollisionObjects());

	m_ppObjects = ((CPlayerShader *)m_ppShaders[3])->GetCollisionObjects();

	((CharacterFrameGaugeShader*)m_ppShaders[11])->SetPlayer(m_ppObjects[0]);
	((CMinimapShader*)m_ppShaders[12])->SetPlayer(m_ppObjects[0]);

	((CPlayerHPGaugeShader*)m_ppShaders[14])->SetPlayerCnt(((CPlayerShader *)m_ppShaders[3])->GetObjectCount());
	((CPlayerHPGaugeShader*)m_ppShaders[14])->SetPlayer(((CPlayerShader *)m_ppShaders[3])->GetCollisionObjects());

	m_ppShaders[6]->Initialize(pCreateMgr, m_pCamera);
	m_ppShaders[7]->Initialize(pCreateMgr, m_pCamera);
	m_ppShaders[8]->Initialize(pCreateMgr, m_pCamera);
	m_ppShaders[9]->Initialize(pCreateMgr, m_pCamera);
	m_ppShaders[10]->Initialize(pCreateMgr, m_pCamera);
	m_ppShaders[11]->Initialize(pCreateMgr, m_pCamera);
	m_ppShaders[12]->Initialize(pCreateMgr, m_pCamera);
	m_ppShaders[13]->Initialize(pCreateMgr, m_pCamera);
	m_ppShaders[14]->Initialize(pCreateMgr, m_pCamera);

	//Managere Initialize
	m_pWayFinder = shared_ptr<CWayFinder>(new CWayFinder());
	m_pCollisionManager = shared_ptr<CCollisionManager>(new CCollisionManager());
	m_pUIObjectsManager = shared_ptr<CUIObjectManager>(new CUIObjectManager());
	m_pFSMMgr = shared_ptr<CFSMMgr>(new CFSMMgr(m_pWayFinder));
	((CMinimapShader*)m_ppShaders[12])->SetWayFinder(m_pWayFinder);

	 //Manager Shaders Setting
	CAniShader* pAniS = (CAniShader *)m_ppShaders[2];
	
	pAniS->SetCollisionManager(m_pCollisionManager);
	pAniS->SetGaugeManger(m_pUIObjectsManager);
	pAniS->SetFSMManager(m_pFSMMgr);

	static_cast<CMinionHPGaugeShader*>(m_ppShaders[8])->SetUIObjectsManager(m_pUIObjectsManager);
	static_cast<CMinimapIconShader*>(m_ppShaders[9])->SetUIObjectsManager(m_pUIObjectsManager);

	m_pCollisionManager->SetNodeMap(m_pWayFinder->GetNodeMap(), m_pWayFinder->GetNodeSize(),m_pWayFinder->GetNodeWH());


	CPlayerShader* pPlayerS = (CPlayerShader *)m_ppShaders[3];
	int nColliderObject = pPlayerS->GetObjectCount();
	for (int i = 0; i < nColliderObject; ++i)
	{
		m_pCollisionManager->AddCollider(((CCollisionObject * *)pPlayerS->GetCollisionObjects())[i]);
	}
	pPlayerS->SetColManagerToObject(m_pCollisionManager);
	
	CNexusTowerShader* pNTS = (CNexusTowerShader *)m_ppShaders[5];
	nColliderObject = pNTS->GetObjectCount();
	for (int i = 0; i < nColliderObject; ++i)
	{
		m_pCollisionManager->AddCollider(((CCollisionObject * *)pNTS->GetCollisionObjects())[i]);
	}
	pNTS->SetColManagerToObject(m_pCollisionManager);
	
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
}

void CScene::ReleaseShaderVariables()
{
	if (m_pcbLights)
	{
		m_pcbLights->Unmap(0, NULL);
		m_pcbLights.Reset();
	}

	for (int i = 0; i < m_nHeaps; ++i)
	{
		if (m_pCbvSrvDescriptorHeaps[i])
			Safe_Release(m_pCbvSrvDescriptorHeaps[i]);
	}
}

void CScene::UpdateShaderVariables()
{
	::memcpy(m_pcbMappedLights, m_pLights, sizeof(LIGHTS));
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

			// Status 창 띄우기 수도 코드
			// 현재 6번 쉐이더가 UI 이므로 상호작용하는 Object의 타입을 받아와서
			// 그 해당 오브젝트에 대한 정보를 출력
			m_ppShaders[6]->OnStatus(pIntersectedObject->GetType());
		}
	}

	if (wParam == MK_RBUTTON)
	{
		GenerateLayEndWorldPosition(pickPosition, xmf4x4View);
	}
	else if (wParam == MK_LBUTTON) {
		
	}
}

void CScene::GenerateLayEndWorldPosition(XMFLOAT3& pickPosition, XMFLOAT4X4&	 xmf4x4View)
{
	XMFLOAT4X4  inverseArr = Matrix4x4::Inverse(xmf4x4View);
	XMFLOAT3 camPosition = m_pCamera->GetPosition();
	XMFLOAT3 layWorldPosition = Vector3::TransformCoord(pickPosition, inverseArr);
	XMFLOAT3 layDirection = Vector3::Subtract(layWorldPosition, camPosition);
	float yDiff = abs(camPosition.y / layDirection.y);

	m_pickWorldPosition = Vector3::Add(camPosition, Vector3::ScalarProduct(layDirection, yDiff, false));

	if (m_pSelectedObject)
	{
		m_pSelectedObject->LookAt(m_pickWorldPosition);
		m_pSelectedObject->SetPathToGo(m_pWayFinder->GetPathToPosition(
			XMFLOAT2(m_pSelectedObject->GetPosition().x, m_pSelectedObject->GetPosition().z),
			XMFLOAT2(m_pickWorldPosition.x, m_pickWorldPosition.z),
			m_pSelectedObject->GetCollisionSize()));
	}
	else {
		// 미 선택시 Player 0번
		m_pSelectedObject = (CAnimatedObject*)m_ppObjects[0];
	}
}

// Process Keyboard Input
void CScene::OnProcessKeyUp(WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);

	if (wParam == VK_ESCAPE)
		::PostQuitMessage(0);
	else if (wParam == VK_F1)
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

