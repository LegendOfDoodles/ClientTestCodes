#pragma once
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"

#define Window_Edge_Size 40

class CAOSCamera : public CCamera
{
public:	// 생성자, 소멸자
	CAOSCamera();
	virtual ~CAOSCamera();

public:	// 공개 함수
	virtual void Initialize(CCreateMgr *pCreateMgr) override;

	virtual void Move(DWORD direction, float distance, bool bVelocity) override;

	virtual void OnProcessMouseMove(WPARAM wParam, LPARAM lParam, float timeElapsed);

protected: // 내부 함수

protected: // 변수
	RECT m_edgeSize{ 0, 0, 0, 0 };
};

