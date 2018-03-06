#pragma once

//ī�޶��� ����(���: Mode)�� ��Ÿ���� ����� ������ ���� �����Ѵ�.
#define	FIRST_PERSON_CAMERA				0x01
#define	SPACESHIP_CAMERA					0x02
#define	THIRD_PERSON_CAMERA			0x03

#define	ASPECT_RATIO							(float(FRAME_BUFFER_WIDTH) / float(FRAME_BUFFER_HEIGHT))

class CCreateMgr;
class CPlayer;

struct VS_CB_CAMERA_INFO
{
	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;
	XMFLOAT3	   m_xmf3Position;
};

class CCamera
{
public:	// ������, �Ҹ���
	CCamera(CCamera *pCamera = NULL);
	virtual ~CCamera();

public:	// ���� �Լ�
	void Initialize(CCreateMgr *pCreateMgr);
	void Finalize();

	virtual void UpdateShaderVariables();

	virtual void SetViewportsAndScissorRects();

	virtual void Move(XMFLOAT3& xmf3Shift);
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);
	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed);
	virtual void SetLookAt(XMFLOAT3& xmf3LookAt);

	void SetOffset(XMFLOAT3 xmf3Offset);

	void RegenerateViewMatrix();

	void GenerateProjectionMatrix(
		float fNearPlaneDistance, float fFarPlaneDistance,
		float fAspectRatio, float fFOVAngle);

	void SetPlayer(CPlayer *pPlayer) { m_pPlayer = pPlayer; }
	CPlayer *GetPlayer() { return(m_pPlayer); }

	void SetMode(DWORD nMode) { m_nMode = nMode; }
	DWORD GetMode() { return(m_nMode); }

	void SetPosition(XMFLOAT3 xmf3Position) { m_xmf3Position = xmf3Position; }
	XMFLOAT3& GetPosition() { return(m_xmf3Position); }

	void SetLookAtPosition(XMFLOAT3 xmf3LookAtWorld) { m_xmf3LookAtWorld = xmf3LookAtWorld; }

	XMFLOAT3& GetLookAtPosition() { return(m_xmf3LookAtWorld); }
	XMFLOAT3& GetRightVector() { return(m_xmf3Right); }
	XMFLOAT3& GetUpVector() { return(m_xmf3Up); }
	XMFLOAT3& GetLookVector() { return(m_xmf3Look); }

	float& GetPitch() { return(m_fPitch); }
	float& GetRoll() { return(m_fRoll); }
	float& GetYaw() { return(m_fYaw); }

	XMFLOAT3& GetOffset() { return(m_xmf3Offset); }

	void SetTimeLag(float fTimeLag) { m_fTimeLag = fTimeLag; }
	float GetTimeLag() { return(m_fTimeLag); }

	XMFLOAT4X4 GetViewMatrix() { return(m_xmf4x4View); }
	XMFLOAT4X4 GetProjectionMatrix() { return(m_xmf4x4Projection); }
	D3D12_VIEWPORT GetViewport() { return(m_viewport); }
	D3D12_RECT GetScissorRect() { return(m_scissorRect); }

protected:	// ���� �Լ�
	void GenerateViewMatrix();
	void GenerateViewMatrix(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3LookAt, XMFLOAT3 xmf3Up);

	void SetViewport(int xTopLeft, int yTopLeft, int nWidth, int nHeight, float fMinZ = 0.0f, float fMaxZ = 1.0f);
	void SetScissorRect(LONG xLeft, LONG yTop, LONG xRight, LONG yBottom);

	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr);
	virtual void ReleaseShaderVariables();

protected: // ����
	XMFLOAT3 m_xmf3Right{ 1.0f, 0.0f, 0.0f };
	XMFLOAT3 m_xmf3Up{ 0.0f,1.0f,0.0f };
	XMFLOAT3 m_xmf3Look{ 0.0f,0.0f,1.0f };
	XMFLOAT3 m_xmf3Position{ 0.0f,0.0f,0.0f };

	float m_fPitch{ 0.0f };
	float m_fRoll{ 0.0f };
	float m_fYaw{ 0.0f };

	DWORD m_nMode{ 0x00 };

	XMFLOAT3 m_xmf3LookAtWorld{ 0.0f,0.0f,0.0f };

	XMFLOAT3 m_xmf3Offset{ 0.0f,0.0f,0.0f };

	float m_fTimeLag{ 0.0f };

	XMFLOAT4X4 m_xmf4x4View;
	XMFLOAT4X4 m_xmf4x4Projection;

	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;

	CPlayer *m_pPlayer{ NULL };

	ID3D12Resource *m_pConstBuffer{ NULL };
	VS_CB_CAMERA_INFO	 *m_pMappedCamera{ NULL };

	ID3D12GraphicsCommandList *m_pCommandList{ NULL };
};
