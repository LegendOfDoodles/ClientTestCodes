#pragma once
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"

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

