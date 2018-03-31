#include "stdafx.h"
#include "Scene.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/02.RotatingObject/RotatingObject.h"
#include "04.Shaders/01.ObjectShader/ObjectShader.h"
#include "04.Shaders/02.TerrainShader/TerrainShader.h"
#include "04.Shaders/03.SkyBoxShader/SkyBoxShader.h"
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"

/// <summary>
/// ����: �⺻ ��, �������̽� ��
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-03-31
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CScene::CScene()
{
}

CScene::~CScene()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CScene::Initialize(CCreateMgr *pCreateMgr)
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
	if (!m_ppShaders) return;

	for (int j = 0; j < m_nShaders; j++)
	{
		if (m_ppShaders[j])
		{
			m_ppShaders[j]->ReleaseUploadBuffers();
		}
	}
}

void CScene::AnimateObjects(float timeElapsed)
{
	UpdateShaderVariables();

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(timeElapsed);
	}
	m_pCamera->Update(timeElapsed);
}

void CScene::Render()
{
	D3D12_GPU_VIRTUAL_ADDRESS d3dcbLightsGpuVirtualAddress = m_pd3dcbLights->GetGPUVirtualAddress();
	m_pCommandList->SetGraphicsRootConstantBufferView(5, d3dcbLightsGpuVirtualAddress); //Lights

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Render(m_pCamera);
	}
}

void CScene::SetViewportsAndScissorRects()
{
	m_pCamera->SetViewportsAndScissorRects();
}

void CScene::UpdateCamera()
{
	m_pCamera->UpdateShaderVariables();
}

void CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam, float timeElapsed)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		::SetCapture(hWnd);
		::GetCursorPos(&m_oldCursorPos);
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		::ReleaseCapture();
		break;
	case WM_MOUSEMOVE:
		OnProcessMouseMove(lParam, timeElapsed);
		break;
	default:
		break;
	}
}

void CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam, float timeElapsed)
{
	if (nMessageID == WM_KEYUP)
	{
		OnProcessKeyUp(wParam, timeElapsed);
	}
	else if (nMessageID == WM_KEYDOWN)
	{
		OnProcessKeyDown(wParam, timeElapsed);
	}
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
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
	m_hWnd = pCreateMgr->GetHwnd();
	m_pCommandList = pCreateMgr->GetCommandList();

	//m_oldCursorPos.x = static_cast<long>(pCreateMgr->GetWindowWidth() / 2.0f);
	//m_oldCursorPos.y = static_cast<long>(pCreateMgr->GetWindowHeight() / 2.0f);

	m_nShaders = 3;
	m_ppShaders = new CShader*[m_nShaders];
	m_ppShaders[0] = new CSkyBoxShader(pCreateMgr);
	m_ppShaders[1] = new CTerrainShader(pCreateMgr);
	m_ppShaders[2] = new CObjectShader(pCreateMgr);
	

	for (int i = 0; i < m_nShaders; ++i)
	{
		m_ppShaders[i]->Initialize(pCreateMgr);
	}

	m_pCamera = new  CCamera();

	m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
	m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
	m_pCamera->RegenerateViewMatrix();

	m_pCamera->Initialize(pCreateMgr);

	BuildLights();
}

void CScene::ReleaseObjects()
{
	if (!m_ppShaders) return;

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Finalize();
	}
	Safe_Delete_Array(m_ppShaders);
}

void CScene::CreateShaderVariables(CCreateMgr *pCreateMgr)
{
	UINT ncbElementBytes = ((sizeof(LIGHTS) + 255) & ~255); //256�� ���
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

// Process Mouse Input
void CScene::OnProcessMouseMove(LPARAM lParam, float timeElapsed)
{
	//int mx{ LOWORD(lParam) };
	//int my{ FRAME_BUFFER_HEIGHT - HIWORD(lParam) };
	//printf("%d %d\n", mx, my);
	static UCHAR pKeyBuffer[256];
	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT cursorPos;
	if (::GetCapture() == m_hWnd)
	{
		::GetCursorPos(&cursorPos);
		cxDelta = (float)(cursorPos.x - m_oldCursorPos.x) / 3.0f;
		cyDelta = (float)(cursorPos.y - m_oldCursorPos.y) / 3.0f;
		::SetCursorPos(m_oldCursorPos.x, m_oldCursorPos.y);
	}
	if ((cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			::GetKeyboardState(pKeyBuffer);
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				m_pCamera->SetRotation(cyDelta, 0.0f, -cxDelta);
			else
				m_pCamera->SetRotation(cyDelta, cxDelta, 0.0f);
		}
	}
	m_pCamera->Update(timeElapsed, false);
}

// Process Keyboard Input
void CScene::OnProcessKeyUp(WPARAM wParam, float timeElapsed)
{
	switch (wParam)
	{
	case VK_ESCAPE:
		::PostQuitMessage(0);
		break;

	case 'W':
		m_pCamera->ResetDirection(DIR_FORWARD);
		break;
	case 'S':
		m_pCamera->ResetDirection(DIR_BACKWARD);
		break;
	case 'A':
		m_pCamera->ResetDirection(DIR_LEFT);
		break;
	case 'D':
		m_pCamera->ResetDirection(DIR_RIGHT);
		break;
	case 'Q':
		m_pCamera->ResetDirection(DIR_UP);
		break;
	case 'E':
		m_pCamera->ResetDirection(DIR_DOWN);
		break;

	default:
		for (int i = 0; i < m_nShaders; ++i)
			m_ppShaders[i]->OnProcessKeyUp(wParam);
		break;
	}
}

void CScene::OnProcessKeyDown(WPARAM wParam, float timeElapsed)
{
	switch (wParam)
	{
	case 'W':
		m_pCamera->SetDirection(DIR_FORWARD);
		break;
	case 'S':
		m_pCamera->SetDirection(DIR_BACKWARD);
		break;
	case 'A':
		m_pCamera->SetDirection(DIR_LEFT);
		break;
	case 'D':
		m_pCamera->SetDirection(DIR_RIGHT);
		break;
	case 'Q':
		m_pCamera->SetDirection(DIR_UP);
		break;
	case 'E':
		m_pCamera->SetDirection(DIR_DOWN);
		break;

	default:
		for (int i = 0; i < m_nShaders; ++i)
			m_ppShaders[i]->OnProcessKeyDown(wParam);
		break;
	}
}