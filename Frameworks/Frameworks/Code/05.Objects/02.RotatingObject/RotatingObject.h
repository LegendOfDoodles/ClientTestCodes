#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"

class CRotatingObject : public CBaseObject
{
public: // 생성자, 소멸자
	CRotatingObject(CCreateMgr *pCreateMgr, int nMeshes = 1);
	virtual ~CRotatingObject();

public: // 공개 함수
	virtual void Animate(float timeElapsed);

	void SetRotationSpeed(float fRotationSpeed) { m_rotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_rotationAxis = xmf3RotationAxis; }

protected: // 내부 함수
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables();

private:	 // 변수
	ID3D12Resource					*m_pcbGameObject{ NULL };
	UINT8				*m_pMappedObject{ NULL };

	XMFLOAT3 m_rotationAxis;
	float m_rotationSpeed;
};