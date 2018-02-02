#pragma once
#include "Mesh.h"

class CCreateMgr;
class CShader;
class CCamera;
class CMaterial;

class CBaseObject
{
public:	// 생성자, 소멸자
	CBaseObject(CCreateMgr *pCreateMgr);
	~CBaseObject();

public: // 공개 함수
	virtual void Initialize(CCreateMgr *pCreateMgr);
	virtual void Finalize();

	void ReleaseUploadBuffers();

	void SetMesh(CMesh *pMesh);
	void SetShader(CShader *pShader);
	void SetMaterial(CMaterial *pMaterial);

	virtual void Animate(float timeElapsed);

	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);

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

	XMFLOAT4X4* GetWorldMatrix() { return &m_xmf4x4World; }

protected: // 내부 함수
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables();

	virtual void OnPrepareRender();

protected: // 변수
	int m_nReferences = 0;

	XMFLOAT4X4 m_xmf4x4World;
	CMesh *m_pMesh{ NULL };

	CShader *m_pShader{ NULL };
	CMaterial	 *m_pMaterial{ NULL };

	ID3D12GraphicsCommandList *m_pCommandList{ NULL };
};

