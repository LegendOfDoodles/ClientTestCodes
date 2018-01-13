#pragma once
#include "Camera.h"

class CThirdPersonCamera : public CCamera
{
public:	// 생성자, 소멸자
	CThirdPersonCamera(CCamera *pCamera = NULL);
	virtual ~CThirdPersonCamera();

public:	// 공개 함수
	virtual void Update(XMFLOAT3& xmf3LookAt, float fTimeElapsed);
	virtual void SetLookAt(XMFLOAT3& vLookAt);

protected:	// 내부 함수

protected: // 변수
};

