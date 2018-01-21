#include "stdafx.h"
#include "Framework.h"
#include "AirplanePlayer.h"

/// <summary>
/// 목적: 테
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-01-21
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CFramework::CFramework()
{
}

CFramework::~CFramework()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
bool CFramework::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	m_hWnd = hWnd;

	m_createMgr.Initialize(hInstance, hWnd);
	m_pRenderMgr = m_createMgr.GetRenderMgr();

	BuildObjects();

	return(true);
}

void CFramework::Finalize()
{
	ReleaseObjects();
	m_createMgr.Release();
}

void CFramework::FrameAdvance(float timeElapsed)
{
	ProcessInput(timeElapsed);
	AnimateObjects(timeElapsed);
	RenderObjects();
}

void CFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, 
	WPARAM wParam, LPARAM lParam)
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
		break;
	default:
		break;
	}
}

void CFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, 
	WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
		case VK_F1:
		case VK_F2:
		case VK_F3:
			m_pCamera = m_pPlayer->ChangeCamera(&m_createMgr, (DWORD)(wParam - VK_F1 + 1), m_pTimer->GetTimeElapsed());
			break;
		case VK_F8:
			break;
		case VK_F9:
		{
			SwitchScreenMode();
			break;
		}
		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CALLBACK CFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
		ResizeScreen(wParam, lParam);
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CFramework::BuildObjects()
{
	m_pRenderMgr->ResetCommandList();

	m_pScene = new CScene();
	m_pScene->Initialize(&m_createMgr);

	CAirplanePlayer *pAirplanePlayer = new CAirplanePlayer(&m_createMgr);
	m_pPlayer = pAirplanePlayer;
	m_pPlayer->Initialize(&m_createMgr);
	m_pPlayer->SetCursorPos(&m_oldCursorPos);
	m_pCamera = m_pPlayer->GetCamera();

	m_pScene->SetPlayer(m_pPlayer);

	m_pRenderMgr->ExecuteCommandList();

	m_pScene->ReleaseUploadBuffers();
}

void CFramework::ReleaseObjects()
{
	if (m_pScene)
	{
		m_pScene->Finalize();
		Safe_Delete(m_pScene);
	}

	if (m_pPlayer)
	{
		m_pPlayer->Finalize();
		Safe_Delete(m_pPlayer);
	}
}

void CFramework::ProcessInput(float timeElapsed)
{
	if (!m_pScene) return;

	m_pScene->ProcessInput(timeElapsed);
}

void CFramework::AnimateObjects(float timeElapsed)
{
	if (m_pScene) m_pScene->AnimateObjects(timeElapsed);
}

void CFramework::RenderObjects()
{
	m_pRenderMgr->Render(m_pScene, m_pCamera);
}

void CFramework::SwitchScreenMode()
{
	m_createMgr.ChangeScreenMode();
	m_pRenderMgr->WaitForGpuComplete();
	m_createMgr.OnResizeBackBuffers();
	m_pRenderMgr->WaitForGpuComplete();
}

void CFramework::ResizeScreen(WPARAM wParam, LPARAM lParam)
{
	m_createMgr.Resize(LOWORD(lParam), HIWORD(lParam));
	m_pRenderMgr->WaitForGpuComplete();
	m_createMgr.OnResizeBackBuffers();
	m_pRenderMgr->WaitForGpuComplete();
}