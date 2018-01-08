#pragma once
#include "CreateMgr.h"
#include "Scene.h"

class CFramework
{
public:	// ������, �Ҹ���
	CFramework();
	~CFramework();

public: // ���� �Լ�
	// Initialize and Release
	bool OnCreate(HINSTANCE hInstance, HWND hwnd);
	void OnDestroy();

	void FrameAdvance();

	// Message Process
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, 
		WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, 
		WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, 
		WPARAM wParam, LPARAM lParam);

private: // ���� �Լ�
	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void RenderObjects();

	void SwitchScreenMode();
	void ResizeScreen(WPARAM wParam, LPARAM lParam);

private: // ����
	CCreateMgr m_createMgr;
	CRenderMgr *m_pRenderMgr;

	CScene *m_pScene;
};

