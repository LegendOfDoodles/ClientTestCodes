#pragma once
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "03.Scenes/00.BaseScene/Scene.h"
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"

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
		WPARAM wParam, LPARAM lParam, float timeElapsed);

private: // 내부 함수
	void BuildObjects();
	void ReleaseObjects();

	void AnimateObjects(float timeElapsed);
	void RenderObjects();

private: // 변수
	HWND m_hWnd{ NULL };

	CCreateMgr m_createMgr;
	CRenderMgr *m_pRenderMgr{ NULL };

	CScene *m_pScene{ NULL };
};

