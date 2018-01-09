#include "stdafx.h"
#include "Framework.h"

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
bool CFramework::OnCreate(HINSTANCE hInstance, HWND hwnd)
{
	m_createMgr.Initialize(hInstance, hwnd);
	m_pRenderMgr = m_createMgr.GetRenderMgr();

	BuildObjects();

	return(true);
}

void CFramework::OnDestroy()
{
	ReleaseObjects();
	m_createMgr.Release();
}

void CFramework::FrameAdvance()
{
	ProcessInput();
	AnimateObjects();
	RenderObjects();
}

void CFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, 
	WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
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

	m_pCamera = new CCamera();
	m_pCamera->Initialize(&m_createMgr);

	m_pScene = new CScene();
	if (m_pScene) m_pScene->BuildObjects(&m_createMgr);

	m_pRenderMgr->ExecuteCommandList();

	if (m_pScene) m_pScene->ReleaseUploadBuffers();
}

void CFramework::ReleaseObjects()
{
	if (m_pScene)
	{
		m_pScene->ReleaseObjects();
		delete m_pScene;
	}

	if (m_pCamera)
	{
		delete m_pCamera;
	}
}

void CFramework::ProcessInput()
{
}

void CFramework::AnimateObjects()
{
	if (m_pScene) m_pScene->AnimateObjects();
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