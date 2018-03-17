#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"

class CRotatingObject : public CBaseObject
{
public: // ������, �Ҹ���
	CRotatingObject(CCreateMgr *pCreateMgr, int nMeshes = 1);
	virtual ~CRotatingObject();

public: // ���� �Լ�
	virtual void Animate(float timeElapsed);

	void SetRotationSpeed(float fRotationSpeed) { m_rotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_rotationAxis = xmf3RotationAxis; }

protected: // ���� �Լ�
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr);
	virtual void ReleaseShaderVariables();
	virtual void UpdateShaderVariables();

private:	 // ����
	ID3D12Resource					*m_pcbGameObject{ NULL };
	UINT8				*m_pMappedObject{ NULL };

	XMFLOAT3 m_rotationAxis;
	float m_rotationSpeed;
};