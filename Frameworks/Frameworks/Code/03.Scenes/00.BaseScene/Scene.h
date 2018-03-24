#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/03.Player/00.BasePlayer/Player.h"
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"

class CCreateMgr;

struct LIGHT
{
	XMFLOAT4				m_color;
	XMFLOAT3				m_position;
	float 					m_fFalloff;
	XMFLOAT3				m_direction;
	float 					m_fTheta; //cos(m_fTheta)
	XMFLOAT3				m_xmf3Attenuation;
	float					m_fPhi; //cos(m_fPhi)
	bool					m_bEnable;
	int						m_nType;
	float					m_fRange;
	float					padding;
};

struct LIGHTS
{
	LIGHT					m_pLights[MAX_LIGHTS];
	XMFLOAT4				m_xmf4GlobalAmbient;
};

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
	void BuildLights();

	void BuildObjects(CCreateMgr *pCreateMgr);
	void ReleaseObjects();

	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables();

	virtual void OnProcessMouseMove(LPARAM lParam);

	virtual void OnProcessKeyUp(WPARAM wParam);
	virtual void OnProcessKeyDown(WPARAM wParam);

protected: // ����
	HWND m_hWnd{ NULL };
	ID3D12GraphicsCommandList *m_pCommandList{ NULL };

	POINT m_oldCursorPos;

	CCamera *m_pCamera{ NULL };

	CShader **m_ppShaders{ NULL };
	int m_nShaders{ 0 };

	LIGHTS	 *m_pLights = NULL;

	ID3D12Resource	*m_pd3dcbLights = NULL;
	LIGHTS *m_pcbMappedLights = NULL;

	CBaseObject ** m_ppObjects{ NULL };
	int m_nObjects{ 0 };

	CPlayer *m_pPlayer{ NULL };
};

