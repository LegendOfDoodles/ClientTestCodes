#pragma once
#include "Camera.h"

class CThirdPersonCamera : public CCamera
{
public:	// ������, �Ҹ���
	CThirdPersonCamera(CCamera *pCamera = NULL);
	virtual ~CThirdPersonCamera();

public:	// ���� �Լ�
	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed);
	virtual void SetLookAt(XMFLOAT3& vLookAt);

protected:	// ���� �Լ�

protected: // ����
};

