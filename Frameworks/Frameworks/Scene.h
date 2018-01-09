#pragma once
#include "Shader.h"

class CCreateMgr;

class CScene
{
public:	// 생성자, 소멸자
	CScene();
	~CScene();

public: // 공개 함수
	void BuildObjects(CCreateMgr *pCreateMgr);
	void ReleaseObjects();

	void ReleaseUploadBuffers();

	void ProcessInput();
	void AnimateObjects(float timeElapsed);
	void Render();

	// Message Process
	bool OnProcessingMouseMessage(HWND hWnd, UINT messageID,
		WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT messageID,
		WPARAM wParam, LPARAM lParam);

private: // 내부 함수

private: // 변수
	ID3D12RootSignature *m_pGraphicsRootSignature = NULL;

	CShader **m_ppShaders = NULL;
	int m_nShaders = 0;

	ID3D12GraphicsCommandList *m_pCommandList;
};

