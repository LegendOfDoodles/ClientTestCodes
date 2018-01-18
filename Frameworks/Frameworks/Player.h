#pragma once
#include "BaseObject.h"
#include "Camera.h"

#define DIR_FORWARD 0x01
#define DIR_BACKWARD 0x02
#define DIR_LEFT 0x04
#define DIR_RIGHT 0x08
#define DIR_UP 0x10
#define DIR_DOWN 0x20

class CCreateMgr;

struct CB_PLAYER_INFO
{
	XMFLOAT4X4					m_xmf4x4World;
};

class CPlayer : public CBaseObject
{
public:	// 생성자, 소멸자
	CPlayer(CCreateMgr *pCreateMgr);
	virtual ~CPlayer();

public: // 공개 함수
	virtual void Render(CCamera *pCamera = NULL);

	void Move(DWORD direction, float distance, bool bVelocity = false);
	void Move(XMFLOAT3& xmf3Shift, bool bVelocity = false);
	void Rotate(float x, float y, float z);
	void Update(float timeElapsed);

	virtual void OnPlayerUpdateCallback(float timeElapsed);
	virtual void OnCameraUpdateCallback(float timeElapsed);

	void SetPosition(XMFLOAT3& xmf3Position);

	CCamera *OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode);

	XMFLOAT3 GetLookVector() { return(m_xmf3Look); }
	XMFLOAT3 GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3 GetRightVector() { return(m_xmf3Right); }
	XMFLOAT3 GetPosition() { return(m_xmf3Position); }

	void SetFriction(float fFriction) { m_fFriction = fFriction; }
	void SetGravity(XMFLOAT3& xmf3Gravity) { m_xmf3Gravity = xmf3Gravity; }
	void SetMaxVelocityXZ(float fMaxVelocity) { m_fMaxVelocityXZ = fMaxVelocity; }
	void SetMaxVelocityY(float fMaxVelocity) { m_fMaxVelocityY = fMaxVelocity; }
	void SetVelocity(XMFLOAT3& xmf3Velocity) { m_xmf3Velocity = xmf3Velocity; }

	XMFLOAT3& GetVelocity() { return(m_xmf3Velocity); }

	float GetYaw() { return(m_fYaw); }
	float GetPitch() { return(m_fPitch); }
	float GetRoll() { return(m_fRoll); }

	CCamera *GetCamera() { return(m_pCamera); }
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; }

	void SetPlayerUpdatedContext(LPVOID pContext) { m_pPlayerUpdatedContext = pContext; }
	void SetCameraUpdatedContext(LPVOID pContext) { m_pCameraUpdatedContext = pContext; }

	virtual CCamera *ChangeCamera(CCreateMgr *pCreateMgr, DWORD nNewCameraMode, float fTimeElapsed) { return(NULL); }

protected: // 내부 함수
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables();

	virtual void OnPrepareRender();

protected: // 변수
	XMFLOAT3 m_xmf3Right{ 1.0f, 0.0f, 0.0f };
	XMFLOAT3 m_xmf3Up{ 0.0f,1.0f,0.0f };
	XMFLOAT3 m_xmf3Look{ 0.0f,0.0f,1.0f };
	XMFLOAT3 m_xmf3Position{ 0.0f,0.0f,0.0f };

	float m_fPitch{ 0.0f };
	float m_fRoll{ 0.0f };
	float m_fYaw{ 0.0f };

	XMFLOAT3 m_xmf3Velocity{ 0.0f,0.0f,0.0f };
	XMFLOAT3 m_xmf3Gravity{ 0.0f,0.0f,0.0f };

	float m_fMaxVelocityXZ{ 0.0f };
	float m_fMaxVelocityY{ 0.0f };

	float m_fFriction{ 0.0f };
	
	LPVOID m_pPlayerUpdatedContext{ NULL };
	LPVOID m_pCameraUpdatedContext{ NULL };

	CCamera *m_pCamera{ NULL };

	ID3D12Resource *m_pConstBuffer{ NULL };
	CB_PLAYER_INFO	 *m_pMappedPlayer{ NULL };
};

