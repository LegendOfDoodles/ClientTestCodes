#include "stdafx.h"
#include "Scene.h"
#include "CreateMgr.h"

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CScene::CScene()
{
	m_pGraphicsRootSignature = NULL;
}


CScene::~CScene()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CScene::BuildObjects(CCreateMgr *pCreateMgr)
{
	ID3D12Device *pDevice = pCreateMgr->GetDevice();
	m_pCommandList = pCreateMgr->GetCommandList();

	//�׷��� ��Ʈ �ñ׳��ĸ� �����Ѵ�.
	m_pGraphicsRootSignature = pCreateMgr->GetGraphicsRootSignature();

	//���� �׸��� ���� ���̴� ��ü�� �����Ѵ�.
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
// ���� �Լ�