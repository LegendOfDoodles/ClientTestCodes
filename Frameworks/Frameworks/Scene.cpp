#include "stdafx.h"
#include "Scene.h"
#include "CreateMgr.h"
#include "RotatingObject.h"
#include "ObjectShader.h"
#include "AirplanePlayer.h"

/// <summary>
/// 목적: 기본 씬, 인터페이스 용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-01-24
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
void CScene::Initialize(CCreateMgr *pCreateMgr)
{
	BuildObjects(pCreateMgr);
}

void CScene::Finalize()
{
	ReleaseObjects();
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
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(timeElapsed);
	}
	m_pPlayer->Update(timeElapsed);
}

void CScene::Render()
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Render(m_pCamera);
	}
	m_pPlayer->Render(m_pCamera);
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
		OnProcessMouseMove();
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
		OnProcessKeyUp(wParam);
	}
	else if (nMessageID == WM_KEYDOWN)
	{
		OnProcessKeyDown(wParam);
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CScene::BuildObjects(CCreateMgr *pCreateMgr)
{
	m_hWnd = pCreateMgr->GetHwnd();
	m_pCommandList = pCreateMgr->GetCommandList();

	m_oldCursorPos.x = static_cast<long>(pCreateMgr->GetWindowWidth() / 2.0f);
	m_oldCursorPos.y = static_cast<long>(pCreateMgr->GetWindowHeight() / 2.0f);

	m_nShaders = 1;
	m_ppShaders = new CShader*[m_nShaders];

	CObjectShader *pShader = new CObjectShader(pCreateMgr);
	pShader->Initialize(pCreateMgr);

	m_ppShaders[0] = pShader;

	CAirplanePlayer *pAirplanePlayer = new CAirplanePlayer(pCreateMgr);
	m_pPlayer = pAirplanePlayer;
	m_pPlayer->Initialize(pCreateMgr);

	m_pCamera = m_pPlayer->GetCamera();
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

// Process Mouse Input
void CScene::OnProcessMouseMove()
{
	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT cursorPos;

	::GetCursorPos(&cursorPos);
	cxDelta = (float)(cursorPos.x - m_oldCursorPos.x) / 3.0f;
	cyDelta = (float)(cursorPos.y - m_oldCursorPos.y) / 3.0f;
	::SetCursorPos(m_oldCursorPos.x, m_oldCursorPos.y);

	if ((cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			m_pPlayer->SetRotation(cyDelta, cxDelta, 0.0f);
		}
	}
}

// Process Keyboard Input
void CScene::OnProcessKeyUp(WPARAM wParam)
{
	switch (wParam)
	{
	case VK_ESCAPE:
		::PostQuitMessage(0);
		break;

	case 'W':
		m_pPlayer->ResetDirection(DIR_FORWARD);
		break;
	case 'S':
		m_pPlayer->ResetDirection(DIR_BACKWARD);
		break;
	case 'A':
		m_pPlayer->ResetDirection(DIR_LEFT);
		break;
	case 'D':
		m_pPlayer->ResetDirection(DIR_RIGHT);
		break;
	case 'Q':
		m_pPlayer->ResetDirection(DIR_UP);
		break;
	case 'E':
		m_pPlayer->ResetDirection(DIR_DOWN);
		break;

	default:
		break;
	}
}

void CScene::OnProcessKeyDown(WPARAM wParam)
{
	switch (wParam)
	{
	case 'W':
		m_pPlayer->SetDirection(DIR_FORWARD);
		break;
	case 'S':
		m_pPlayer->SetDirection(DIR_BACKWARD);
		break;
	case 'A':
		m_pPlayer->SetDirection(DIR_LEFT);
		break;
	case 'D':
		m_pPlayer->SetDirection(DIR_RIGHT);
		break;
	case 'Q':
		m_pPlayer->SetDirection(DIR_UP);
		break;
	case 'E':
		m_pPlayer->SetDirection(DIR_DOWN);
		break;

	default:
		break;
	}
}