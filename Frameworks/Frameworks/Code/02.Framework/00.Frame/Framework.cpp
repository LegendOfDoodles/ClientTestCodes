#include "stdafx.h"
#include "Framework.h"

/// <summary>
/// 목적: 테
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-04-07
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
	AnimateObjects(timeElapsed);
	RenderObjects();
}

LRESULT CALLBACK CFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam, float timeElapsed)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
		m_createMgr.Resize(LOWORD(lParam), HIWORD(lParam));
		// m_createMgr.ChangeScreenMode();
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
	case WM_MOUSEWHEEL:
		m_pScene->OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam, timeElapsed);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		m_pScene->OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam, timeElapsed);
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

	m_pRenderMgr->ExecuteCommandList();

	m_pScene->ReleaseUploadBuffers();
}

void CFramework::ReleaseObjects()
{
	if (!m_pScene) return;

	m_pScene->Finalize();
	Safe_Delete(m_pScene);
}

void CFramework::AnimateObjects(float timeElapsed)
{
	m_pScene->AnimateObjects(timeElapsed);
}

void CFramework::RenderObjects()
{
	m_pRenderMgr->Render(m_pScene);
}