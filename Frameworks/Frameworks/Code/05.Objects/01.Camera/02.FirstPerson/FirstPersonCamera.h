#pragma once
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"

class CFirstPersonCamera : public CCamera
{
public:	// ������, �Ҹ���
	CFirstPersonCamera(CCamera *pCamera = NULL);
	virtual ~CFirstPersonCamera();

public:	// ���� �Լ�
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);

protected:	// ���� �Լ�

protected: // ����
};

