#pragma once
#include "Shader.h"
#include "Player.h"
#include "Camera.h"

class CCreateMgr;

class CScene
{
public:	// ������, �Ҹ���
	CScene();
	~CScene();

public: // ���� �Լ�
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

protected: // ���� �Լ�
	void BuildObjects(CCreateMgr *pCreateMgr);
	void ReleaseObjects();

	virtual void OnProcessMouseMove();

	virtual void OnProcessKeyUp(WPARAM wParam);
	virtual void OnProcessKeyDown(WPARAM wParam);

protected: // ����
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

