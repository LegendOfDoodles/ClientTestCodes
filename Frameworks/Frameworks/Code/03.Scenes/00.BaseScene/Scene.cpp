#include "stdafx.h"
#include "Scene.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/02.AnimatedObject/AnimatedObject.h"
#include "04.Shaders/01.ObjectShader/StaticObjectShader.h"
#include "04.Shaders/02.TerrainShader/TerrainShader.h"
#include "04.Shaders/03.SkyBoxShader/SkyBoxShader.h"
#include "04.Shaders/98.ArrowShader/ArrowShader.h"
#include "05.Objects/01.Camera/01.AOSCamera/AOSCamera.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

/// <summary>
/// 목적: 기본 씬, 인터페이스 용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-04-20
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
void CScene::Initialize(CCreateMgr *pCreateMgr, Network network)
{
	m_Network = network;
	BuildObjects(pCreateMgr);
	CreateShaderVariables(pCreateMgr);
	WSADATA	wsadata;
	WSAStartup(MAKEWORD(2, 2), &wsadata);

	m_Network.m_mysocket = WSASocket(AF_INET, SOCK_STREAM, IPPROTO_TCP, NULL, 0, 0);

	SOCKADDR_IN ServerAddr;
	ZeroMemory(&ServerAddr, sizeof(SOCKADDR_IN));
	ServerAddr.sin_family = AF_INET;
	ServerAddr.sin_port = htons(MY_SERVER_PORT);
	ServerAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

	int Result = WSAConnect(m_Network.m_mysocket, (sockaddr *)&ServerAddr, sizeof(ServerAddr), NULL, NULL, NULL, NULL);

	WSAAsyncSelect(m_Network.m_mysocket, m_hWnd, WM_SOCKET, FD_CLOSE | FD_READ);

	m_Network.m_send_wsabuf.buf = m_Network.m_send_buffer;
	m_Network.m_send_wsabuf.len = MAX_BUFF_SIZE;
	m_Network.m_recv_wsabuf.buf = m_Network.m_recv_buffer;
	m_Network.m_recv_wsabuf.len = MAX_BUFF_SIZE;
}

void CScene::Finalize()
{
	ReleaseObjects();
	ReleaseShaderVariables();
}

void CScene::ReleaseUploadBuffers()
{
	if (!m_ppShaders) return;

	for (int j = 0; j < m_nShaders; j++)
	{
		if (m_ppShaders[j])
		{
			m_ppShaders[j]->ReleaseUploadBuffers();
		}
	}
}

void CScene::ProcessInput()
{
	static UCHAR pKeyBuffer[256];

	GetKeyboardState(pKeyBuffer);

	for (int i = 0; i < m_nShaders; ++i)
		m_ppShaders[i]->OnProcessKeyInput(pKeyBuffer);

	m_pCamera->OnProcessMouseInput(pKeyBuffer);
	m_pCamera->OnProcessKeyInput(pKeyBuffer);

	if (m_pSelectedObject && pKeyBuffer['P'] & 0xF0)
	{
		m_pSelectedObject->Rotate(10, 0, 0);
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
}

void CScene::Render()
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	m_pCommandList->SetGraphicsRootConstantBufferView(5, d3dcbLightsGpuVirtualAddress); //Lights

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

void CScene::SetViewportsAndScissorRects()
{
	m_pCamera->SetViewportsAndScissorRects();
}

void CScene::UpdateCamera()
{
	if (m_bCamChanged)
	{
		m_pCamera->Finalize();
		Safe_Delete(m_pCamera);

		if (m_bCurCamIsAOS)
		{
			m_pCamera = new  CAOSCamera();
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
	if (nMessageID == WM_KEYUP)
	{
		OnProcessKeyUp(wParam, lParam);
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CScene::BuildLights()
{
	m_pLights = new LIGHTS;
	::ZeroMemory(m_pLights, sizeof(LIGHTS));

	m_pLights->m_xmf4GlobalAmbient = XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);

	m_pLights->m_pLights[0].m_bEnable = true;
	m_pLights->m_pLights[0].m_nType = POINT_LIGHT;
	m_pLights->m_pLights[0].m_fRange = 100.0f;
	m_pLights->m_pLights[0].m_color = XMFLOAT4(0.8f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[0].m_position = XMFLOAT3(130.0f, 30.0f, 30.0f);
	m_pLights->m_pLights[0].m_direction = XMFLOAT3(0.0f, 0.0f, 0.0f);
	m_pLights->m_pLights[0].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.001f, 0.0001f);

	m_pLights->m_pLights[1].m_bEnable = true;
	m_pLights->m_pLights[1].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[1].m_fRange = 100.0f;
	m_pLights->m_pLights[1].m_color = XMFLOAT4(0.0f, 1.0f, 0.4f, 1.0f);
	m_pLights->m_pLights[1].m_position = XMFLOAT3(-50.0f, 20.0f, -5.0f);
	m_pLights->m_pLights[1].m_direction = XMFLOAT3(0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[1].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[1].m_fFalloff = 8.0f;
	m_pLights->m_pLights[1].m_fPhi = (float)cos(XMConvertToRadians(40.0f));
	m_pLights->m_pLights[1].m_fTheta = (float)cos(XMConvertToRadians(20.0f));

	m_pLights->m_pLights[2].m_bEnable = true;
	m_pLights->m_pLights[2].m_nType = DIRECTIONAL_LIGHT;
	m_pLights->m_pLights[2].m_color = XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	m_pLights->m_pLights[2].m_direction = XMFLOAT3(0.0f, 0.0f,1.0f);

	m_pLights->m_pLights[3].m_bEnable = false;
	m_pLights->m_pLights[3].m_nType = SPOT_LIGHT;
	m_pLights->m_pLights[3].m_fRange = 60.0f;
	m_pLights->m_pLights[3].m_color = XMFLOAT4(0.5f, 0.0f, 0.0f, 1.0f);
	m_pLights->m_pLights[3].m_position = XMFLOAT3(-150.0f, 30.0f, 30.0f);
	m_pLights->m_pLights[3].m_direction = XMFLOAT3(0.0f, 1.0f, 1.0f);
	m_pLights->m_pLights[3].m_xmf3Attenuation = XMFLOAT3(1.0f, 0.01f, 0.0001f);
	m_pLights->m_pLights[3].m_fFalloff = 8.0f;
	m_pLights->m_pLights[3].m_fPhi = (float)cos(XMConvertToRadians(90.0f));
	m_pLights->m_pLights[3].m_fTheta = (float)cos(XMConvertToRadians(30.0f));
}

void CScene::BuildObjects(CCreateMgr *pCreateMgr)
{
	m_pCreateMgr = pCreateMgr;

	m_hWnd = pCreateMgr->GetHwnd();
	m_pCommandList = pCreateMgr->GetCommandList();

	m_nShaders = 4;
	m_ppShaders = new CShader*[m_nShaders];
	m_ppShaders[0] = new CSkyBoxShader(pCreateMgr);
	CTerrainShader* pTerrainShader = new CTerrainShader(pCreateMgr);
	m_ppShaders[1] = pTerrainShader;
	m_ppShaders[2] = new CAniShader(pCreateMgr);
	m_ppShaders[3] = new CArrowShader(pCreateMgr);

	for (int i = 0; i < m_nShaders; ++i)
	{
		m_ppShaders[i]->Initialize(pCreateMgr);
	}

	for (int i = 2; i < m_nShaders; ++i)
	{
		m_ppShaders[i]->Initialize(pCreateMgr, pTerrainShader->GetTerrain());
	}

	m_pCamera = new  CAOSCamera();

	m_pCamera->Initialize(pCreateMgr);

	m_pWayFinder = new CWayFinder(124.2, 124.2);

	BuildLights();
}

void CScene::ReleaseObjects()
{
	if (m_pCamera)
	{
		m_pCamera->Finalize();
		Safe_Delete(m_pCamera);
	}
	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			if(m_ppShaders[i]) m_ppShaders[i]->Finalize();
		}
		Safe_Delete_Array(m_ppShaders);
	}
	if (m_pWayFinder)
	{
		Safe_Delete(m_pWayFinder);
	}
}

void CScene::CreateShaderVariables(CCreateMgr *pCreateMgr)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256의 배수
	m_pd3dcbLights = pCreateMgr->CreateBufferResource(NULL, ncbElementBytes, D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, NULL);

	m_pd3dcbLights->Map(0, NULL, (void **)&m_pcbMappedLights);
}

void CScene::ReleaseShaderVariables()
{
	if (m_pd3dcbLights)
	{
		m_pd3dcbLights->Unmap(0, NULL);
		Safe_Release(m_pd3dcbLights);
	}
}

void CScene::UpdateShaderVariables()
{
	m_pLights->m_pLights[1].m_position = m_pCamera->GetPosition();
	m_pLights->m_pLights[1].m_direction = m_pCamera->GetLookVector();
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

	int nIntersected{ 0 };
	float hitDistance{ FLT_MAX }, nearestHitDistance{ FLT_MAX };
	CBaseObject *pIntersectedObject{ NULL };
	//m_pSelectedObject = NULL;

	for (int i = 0; i < m_nShaders; i++)
	{
		pIntersectedObject = m_ppShaders[i]->PickObjectByRayIntersection(pickPosition, xmf4x4View, hitDistance);
		if (pIntersectedObject && (hitDistance < nearestHitDistance))
		{
			nearestHitDistance = hitDistance;
			m_pSelectedObject = pIntersectedObject;
			printf("selected!\n");
		}
	}

	if (wParam == MK_RBUTTON)
	{
		GenerateLayEndWorldPosition(pickPosition, xmf4x4View);
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
			XMFLOAT2(m_pickWorldPosition.x, m_pickWorldPosition.z)));
	}
}

// Process Keyboard Input
void CScene::OnProcessKeyUp(WPARAM wParam, LPARAM lParam)
{
	

	if(wParam == VK_ESCAPE)
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
	if(m_Network.m_mysocket != 0)
		m_Network.SendMovePacket(wParam,m_pSelectedObject);

	
}