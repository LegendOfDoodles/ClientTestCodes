#pragma once
#include "Camera.h"

class CSpaceShipCamera : public CCamera
{
public:	// 생성자, 소멸자
	CSpaceShipCamera(CCamera *pCamera = NULL);
	virtual ~CSpaceShipCamera();

public:	// 공개 함수
	virtual void Rotate(float fPitch = 0.0f, float fYaw = 0.0f, float fRoll = 0.0f);

protected:	// 내부 함수

protected: // 변수
};

