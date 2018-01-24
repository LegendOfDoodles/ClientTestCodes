#pragma once
#include "Shader.h"
#include "Player.h"
#include "Camera.h"

class CCreateMgr;

class CScene
{
public:	// 생성자, 소멸자
	CScene();
	~CScene();

public: // 공개 함수
	virtual void Initialize(CCreateMgr *pCreateMgr);
	virtual void Finalize();

	void ReleaseUploadBuffers();

	void AnimateObjects(float timeElapsed);
	void Render();

	void SetViewportsAndScissorRects();
	void UpdateCamera();

	// Message Process
	void OnProcessingMouseMessage(HWND hWnd, UINT messageID,
		WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT messageID,
		WPARAM wParam, LPARAM lParam);


	CCamera * GetCamera() { return m_pCamera; }

protected: // 내부 함수
	void BuildObjects(CCreateMgr *pCreateMgr);
	void ReleaseObjects();

	virtual void OnProcessMouseMove();

	virtual void OnProcessKeyUp(WPARAM wParam);
	virtual void OnProcessKeyDown(WPARAM wParam);

protected: // 변수
	HWND m_hWnd{ NULL };
	ID3D12GraphicsCommandList *m_pCommandList{ NULL };

	POINT m_oldCursorPos;

	CCamera *m_pCamera{ NULL };

	CShader **m_ppShaders{ NULL };
	int m_nShaders{ 0 };

	CBaseObject ** m_ppObjects{ NULL };
	int m_nObjects{ 0 };

	CPlayer *m_pPlayer{ NULL };
};

