#pragma once
#include "CreateMgr.h"
#include "Scene.h"
#include "Camera.h"

class CFramework
{
public:	// 생성자, 소멸자
	CFramework();
	~CFramework();

public: // 공개 함수
	// Initialize and Release
	bool Initialize(HINSTANCE hInstance, HWND hWnd);
	void Finalize();

	void FrameAdvance(float timeElapsed);

	// Message Process
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID,
		WPARAM wParam, LPARAM lParam);

private: // 내부 함수
	void BuildObjects();
	void ReleaseObjects();

	void AnimateObjects(float timeElapsed);
	void RenderObjects();

	void SwitchScreenMode();
	void ResizeScreen(WPARAM wParam, LPARAM lParam);

private: // 변수
	HWND m_hWnd{ NULL };

	CCreateMgr m_createMgr;
	CRenderMgr *m_pRenderMgr{ NULL };

	CScene *m_pScene{ NULL };
};

