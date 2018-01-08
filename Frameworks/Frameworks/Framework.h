#pragma once
#include "CreateMgr.h"
#include "Scene.h"

class CFramework
{
public:	// 생성자, 소멸자
	CFramework();
	~CFramework();

public: // 공개 함수
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

private: // 내부 함수
	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();
	void RenderObjects();

	void SwitchScreenMode();
	void ResizeScreen(WPARAM wParam, LPARAM lParam);

private: // 변수
	CCreateMgr m_createMgr;
	CRenderMgr *m_pRenderMgr;

	CScene *m_pScene;
};

