#pragma once
#include "CreateMgr.h"
#include "Scene.h"
#include "Camera.h"
#include "Timer.h"

class CFramework
{
public:	// ������, �Ҹ���
	CFramework();
	~CFramework();

public: // ���� �Լ�
	// Initialize and Release
	bool OnCreate(HINSTANCE hInstance, HWND hWnd);
	void OnDestroy();

	void FrameAdvance();

	// Message Process
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, 
		WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, 
		WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, 
		WPARAM wParam, LPARAM lParam);

	// Register Timer
	void RegisterTimer(CTimer* timer) { m_pTimer = timer; }

private: // ���� �Լ�
	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects(float timeElapsed);
	void RenderObjects();

	void SwitchScreenMode();
	void ResizeScreen(WPARAM wParam, LPARAM lParam);

private: // ����
	CCreateMgr m_createMgr;
	CRenderMgr *m_pRenderMgr;

	CScene *m_pScene;

	CCamera *m_pCamera{ NULL };
	CTimer *m_pTimer{ NULL };
};

