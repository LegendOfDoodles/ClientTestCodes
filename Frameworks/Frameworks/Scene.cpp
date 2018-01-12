#include "stdafx.h"
#include "Scene.h"
#include "CreateMgr.h"
#include "RotatingObject.h"
#include "DiffusedShader.h"

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
void CScene::BuildObjects(CCreateMgr *pCreateMgr)
{
	ID3D12Device *pDevice = pCreateMgr->GetDevice();
	m_pCommandList = pCreateMgr->GetCommandList();

	CTriangleMesh *pMesh = new CTriangleMesh(pCreateMgr);

	//m_nShaders = 1;
	//m_ppShaders = new CShader*[m_nShaders];

	//CShader *pShader = new CShader(pCreateMgr);
	//pShader->CreateShader(pCreateMgr);
	//pShader->BuildObjects(pCreateMgr, NULL);

	//m_ppShaders[0] = pShader;

	m_nObjects = 1;
	m_ppObjects = new CBaseObject*[m_nObjects];

	CRotatingObject *pRotatingObject = new CRotatingObject();
	pRotatingObject->SetMesh(pMesh);

	CDiffusedShader *pShader = new CDiffusedShader(pCreateMgr);
	pShader->Initialize(pCreateMgr);

	pRotatingObject->SetShader(pShader);

	m_ppObjects[0] = pRotatingObject;
}

void CScene::ReleaseObjects()
{
	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->Finalize();
		}
		delete[] m_ppShaders;
	}

	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			if (m_ppObjects[j]) delete m_ppObjects[j];
		}
		delete[] m_ppObjects;
	}
}

void CScene::ReleaseUploadBuffers()
{
	//if (!m_ppShaders) return;

	//for (int j = 0; j < m_nShaders; j++)
	//{
	//	if (m_ppShaders[j])
	//	{
	//		m_ppShaders[j]->ReleaseUploadBuffers();
	//	}
	//}

	if (!m_ppObjects) return;

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->ReleaseUploadBuffers();
		}
	}
}

void CScene::ProcessInput()
{
}

void CScene::AnimateObjects(float timeElapsed)
{
	//for (int i = 0; i < m_nShaders; i++)
	//{
	//	m_ppShaders[i]->AnimateObjects(timeElapsed);
	//}

	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CScene::Render(CCamera *pCamera)
{
	//for (int i = 0; i < m_nShaders; i++)
	//{
	//	m_ppShaders[i]->Render();
	//}

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j])
		{
			m_ppObjects[j]->Render(pCamera);
		}
	}
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