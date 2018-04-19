#pragma once
#include "06.Meshes/01.Mesh/Mesh.h"

class CCreateMgr;
class CShader;
class CCamera;
class CMaterial;

struct CB_GAMEOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World;
};

struct CB_ANIOBJECT_INFO
{
	XMFLOAT4X4 m_xmf4x4World0;
	XMFLOAT4X4 m_xmf4x4Frame[128];
};

class CBaseObject
{
public:	// ������, �Ҹ���
	CBaseObject(CCreateMgr *pCreateMgr, int nMeshes = 1);
	~CBaseObject();

public: // ���� �Լ�
	virtual void Initialize(CCreateMgr *pCreateMgr);
	virtual void Finalize();

	void ReleaseUploadBuffers();

	void SetMesh(int nIndex, CMesh *pMesh);
	void SetBoundingMesh(CCreateMgr *pCreateMgr, float width, float height, float depth, float xOffset = 0.0f, float yOffSet = 0.0f, float zOffSet = 0.0f);
	void SetShader(CShader *pShader);
	void SetMaterial(CMaterial *pMaterial);

	virtual void Animate(float timeElapsed);

	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);
	virtual void RenderBoundingBox(CCamera *pCamera, UINT istanceCnt = 1);

	void GenerateRayForPicking(XMFLOAT3& pickPosition, XMFLOAT4X4& xmf4x4View,
		XMFLOAT3 &pickRayOrigin, XMFLOAT3 &pickRayDirection);
	bool PickObjectByRayIntersection(XMFLOAT3& pickPosition, XMFLOAT4X4& xmf4x4View, float &hitDistance);

	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

	void LookAt(XMFLOAT3 objPosition);

	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	XMFLOAT3 GetLookModel();
	XMFLOAT3 GetUpModel();

	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	XMFLOAT4X4* GetWorldMatrix() { return &m_xmf4x4World; }
	XMFLOAT4X4* GetFrameMatrix() { return m_xmf4x4Frame; }


	void SetCbvGPUDescriptorHandle(D3D12_GPU_DESCRIPTOR_HANDLE cbvGPUDescriptorHandle) { m_cbvGPUDescriptorHandle = cbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtr(UINT64 cbvGPUDescriptorHandlePtr) { m_cbvGPUDescriptorHandle.ptr = cbvGPUDescriptorHandlePtr; }

	void SetCbvGPUDescriptorHandleForBB(D3D12_GPU_DESCRIPTOR_HANDLE cbvGPUDescriptorHandle) { m_cbvGPUDescriptorHandleForBB = cbvGPUDescriptorHandle; }
	void SetCbvGPUDescriptorHandlePtrForBB(UINT64 cbvGPUDescriptorHandlePtr) { m_cbvGPUDescriptorHandleForBB.ptr = cbvGPUDescriptorHandlePtr; }

	D3D12_GPU_DESCRIPTOR_HANDLE GetCbvGPUDescriptorHandle() { return(m_cbvGPUDescriptorHandle); }

protected: // ���� �Լ�
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables();

	virtual void OnPrepareRender();

protected: // ����
	int m_nReferences = 0;

	XMFLOAT4X4 m_xmf4x4World;
	XMFLOAT4X4 m_xmf4x4Frame[128];

	CMesh	**m_ppMeshes{ NULL };
	int m_nMeshes{ 0 };

	CMesh *m_pBoundingMesh{ NULL };

	CShader *m_pShader{ NULL };
	CMaterial	 *m_pMaterial{ NULL };

	D3D12_GPU_DESCRIPTOR_HANDLE m_cbvGPUDescriptorHandle{ NULL };
	D3D12_GPU_DESCRIPTOR_HANDLE m_cbvGPUDescriptorHandleForBB{ NULL };

	ID3D12Resource					*m_pcbGameObject{ NULL };
	UINT8				*m_pMappedObject{ NULL };

	ID3D12GraphicsCommandList *m_pCommandList{ NULL };
};

