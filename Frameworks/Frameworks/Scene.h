#pragma once
#include "Shader.h"
#include "Player.h"

class CCreateMgr;
class CCamera;

class CScene
{
public:	// 생성자, 소멸자
	CScene();
	~CScene();

public: // 공개 함수
	virtual void Initialize(CCreateMgr *pCreateMgr);
	virtual void Finalize();

	void ReleaseUploadBuffers();

	virtual void ProcessInput(float timeElapsed);
	void AnimateObjects(float timeElapsed);
	void Render(CCamera *pCamera);

	// Message Process
	bool OnProcessingMouseMessage(HWND hWnd, UINT messageID,
		WPARAM wParam, LPARAM lParam);
	bool OnProcessingKeyboardMessage(HWND hWnd, UINT messageID,
		WPARAM wParam, LPARAM lParam);

	void SetPlayer(CPlayer* pPlayer) { m_pPlayer = pPlayer; }

private: // 내부 함수
	void BuildObjects(CCreateMgr *pCreateMgr);
	void ReleaseObjects();

private: // 변수
	ID3D12GraphicsCommandList *m_pCommandList{ NULL };

	CShader **m_ppShaders{ NULL };
	int m_nShaders{ 0 };

	CBaseObject ** m_ppObjects{ NULL };
	int m_nObjects{ 0 };

	CPlayer *m_pPlayer{ NULL };
};

