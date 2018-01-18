#include "stdafx.h"
#include "Scene.h"
#include "CreateMgr.h"
#include "RotatingObject.h"
#include "ObjectShader.h"

/// <summary>
/// 목적: 기본 씬, 인터페이스 용
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-01-12
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

void CScene::ProcessInput()
{
}

void CScene::AnimateObjects(float timeElapsed)
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(timeElapsed);
	}
}

void CScene::Render(CCamera *pCamera)
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Render(pCamera);
	}
	m_pPlayer->Render(pCamera);
}

bool CScene::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	return(false);
}

bool CScene::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	return(false);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CScene::BuildObjects(CCreateMgr *pCreateMgr)
{
	m_pCommandList = pCreateMgr->GetCommandList();

	m_nShaders = 1;
	m_ppShaders = new CShader*[m_nShaders];

	CObjectShader *pShader = new CObjectShader(pCreateMgr);
	pShader->Initialize(pCreateMgr);

	m_ppShaders[0] = pShader;
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