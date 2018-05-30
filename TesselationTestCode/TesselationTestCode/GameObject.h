#pragma once
#include "Mesh.h"
#include "Camera.h"

class CShader;

class CGameObject
{
public:
	CGameObject();
	virtual ~CGameObject();

public:
	int m_nReferences = 0;
	CMesh	**m_ppMeshes{ NULL };
	int m_nMeshes{ 0 };

public:
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected:
	XMFLOAT4X4 m_xmf4x4World;
	CMesh *m_pMesh = NULL;
	CShader *m_pShader = NULL;

public:
	void ReleaseUploadBuffers();
	virtual void SetMesh(int nIndex, CMesh *pMesh);
	virtual void SetMesh(CMesh *pMesh);
	virtual void SetShader(CShader *pShader);
	virtual void Animate(float fTimeElapsed);
	virtual void OnPrepareRender();
	virtual void Render(ID3D12GraphicsCommandList *pd3dCommandList, CCamera *pCamera);

public:
	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);

	//게임 객체의 월드 변환 행렬에서 위치 벡터와 방향(x-축, y-축, z-축) 벡터를 반환한다.
	XMFLOAT3 GetPosition();
	XMFLOAT3 GetLook();
	XMFLOAT3 GetUp();
	XMFLOAT3 GetRight();

	//게임 객체의 위치를 설정한다.
	void SetPosition(float x, float y, float z);
	void SetPosition(XMFLOAT3 xmf3Position);

	//게임 객체를 로컬 x-축, y-축, z-축 방향으로 이동한다.
	void MoveStrafe(float fDistance = 1.0f);
	void MoveUp(float fDistance = 1.0f);
	void MoveForward(float fDistance = 1.0f);

	//게임 객체를 회전(x-축, y-축, z-축)한다.
	void Rotate(float fPitch = 10.0f, float fYaw = 10.0f, float fRoll = 10.0f);

public:
	//상수 버퍼를 생성한다.
	virtual void CreateShaderVariables(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList
		*pd3dCommandList);
	//상수 버퍼의 내용을 갱신한다.
	virtual void UpdateShaderVariables(ID3D12GraphicsCommandList *pd3dCommandList);
	virtual void ReleaseShaderVariables();	
};


class CRotatingObject : public CGameObject
{
public:
	CRotatingObject();
	virtual ~CRotatingObject();

private:
	XMFLOAT3 m_xmf3RotationAxis;
	float m_fRotationSpeed;

public:
	void SetRotationSpeed(float fRotationSpeed) { m_fRotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) {
		m_xmf3RotationAxis = xmf3RotationAxis;
	}

	virtual void Animate(float fTimeElapsed);
};


class CHeightMapTerrain : public CGameObject
{
public: // 생성자, 소멸자
	CHeightMapTerrain(ID3D12Device *pd3dDevice,
		ID3D12GraphicsCommandList *pd3dCommandList, XMFLOAT3 xmf3Scale, XMFLOAT4 xmf4Color);
	virtual ~CHeightMapTerrain();

public: // 공개 함수

protected: // 내부 함수

private:	 // 변수
	//높이 맵의 가로와 세로 크기이다.
	int m_nWidth;
	int m_nLength;
	//지형을 실제로 몇 배 확대할 것인가를 나타내는 스케일 벡터이다.
	XMFLOAT3 m_xmf3Scale;
};