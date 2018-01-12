#pragma once
#include "Mesh.h"

class CCreateMgr;
class CShader;
class CCamera;

class CBaseObject
{
public:	// 생성자, 소멸자
	CBaseObject();
	~CBaseObject();

public: // 공개 함수
	virtual void Initialize(CCreateMgr *pCreateMgr);
	virtual void Finalize();

	void ReleaseUploadBuffers();

	virtual void SetMesh(CMesh *pMesh);
	virtual void SetShader(CShader *pShader);

	virtual void Animate(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected: // 내부 함수
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables();

	virtual void OnPrepareRender();

protected: // 변수
	int m_nReferences = 0;

	XMFLOAT4X4 m_xmf4x4World;
	CMesh *m_pMesh = NULL;

	CShader *m_pShader = NULL;

	ID3D12GraphicsCommandList *m_pCommandList;
};

