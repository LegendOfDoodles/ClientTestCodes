#include "stdafx.h"
#include "Scene.h"
#include "CreateMgr.h"

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CScene::CScene()
{
	m_pGraphicsRootSignature = NULL;
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

	//그래픽 루트 시그너쳐를 생성한다.
	m_pGraphicsRootSignature = pCreateMgr->GetGraphicsRootSignature();

	//씬을 그리기 위한 셰이더 객체를 생성한다.
	m_nShaders = 1;
	m_ppShaders = new CShader*[m_nShaders];

	CShader *pShader = new CShader(pCreateMgr);
	pShader->CreateShader(pCreateMgr);
	pShader->BuildObjects(pCreateMgr, NULL);

	m_ppShaders[0] = pShader;
}

void CScene::ReleaseObjects()
{
	if (m_ppShaders)
	{
		for (int i = 0; i < m_nShaders; i++)
		{
			m_ppShaders[i]->ReleaseShaderVariables();
			m_ppShaders[i]->ReleaseObjects();
			m_ppShaders[i]->Release();
		}
		delete[] m_ppShaders;
	}
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

void CScene::AnimateObjects()
{
	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->AnimateObjects(0.2f);
	}
}

void CScene::Render()
{
	m_pCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature);

	for (int i = 0; i < m_nShaders; i++)
	{
		m_ppShaders[i]->Render();
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