#pragma once
#include "CreateMgr.h"
#include "Scene.h"
#include "Camera.h"

class CFramework
{
public:	// ������, �Ҹ���
	CFramework();
	~CFramework();

public: // ���� �Լ�
	// Initialize and Release
	bool Initialize(HINSTANCE hInstance, HWND hWnd);
	void Finalize();

	void FrameAdvance(float timeElapsed);

	// Message Process
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID,
		WPARAM wParam, LPARAM lParam);

private: // ���� �Լ�
	void BuildObjects();
	void ReleaseObjects();

	void AnimateObjects(float timeElapsed);
	void RenderObjects();

	void SwitchScreenMode();
	void ResizeScreen(WPARAM wParam, LPARAM lParam);

private: // ����
	HWND m_hWnd{ NULL };

	CCreateMgr m_createMgr;
	CRenderMgr *m_pRenderMgr{ NULL };

	CScene *m_pScene{ NULL };
};

