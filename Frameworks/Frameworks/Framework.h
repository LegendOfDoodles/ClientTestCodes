#pragma once
#include "CreateMgr.h"
#include "Scene.h"
#include "Camera.h"
#include "Timer.h"
#include "Player.h"

class CFramework
{
public:	// 생성자, 소멸자
	CFramework();
	~CFramework();

public: // 공개 함수
	// Initialize and Release
	bool Initialize(HINSTANCE hInstance, HWND hWnd);
	void Finalize();

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

private: // 내부 함수
	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects(float timeElapsed);
	void RenderObjects();

	void SwitchScreenMode();
	void ResizeScreen(WPARAM wParam, LPARAM lParam);

private: // 변수
	HWND m_hWnd{ NULL };

	CCreateMgr m_createMgr;
	CRenderMgr *m_pRenderMgr{ NULL };

	CScene *m_pScene{ NULL };

	CCamera *m_pCamera{ NULL };
	CTimer *m_pTimer{ NULL };

	CPlayer *m_pPlayer{ NULL };

	POINT m_ptOldCursorPos;
};

