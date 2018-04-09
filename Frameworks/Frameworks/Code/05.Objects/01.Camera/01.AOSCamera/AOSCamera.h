#pragma once
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"

#define Window_Edge_Size 40

#define Zoom_In_Max  30.0f
#define Zoom_Out_Max  90.0f
#define Zoom_Change_Value 5.0f

class CAOSCamera : public CCamera
{
public:	// 생성자, 소멸자
	CAOSCamera();
	virtual ~CAOSCamera();

public:	// 공개 함수
	virtual void Initialize(CCreateMgr *pCreateMgr) override;

	virtual void Move(DWORD direction, float distance, bool bVelocity) override;

	virtual void OnProcessMouseMove(WPARAM wParam, LPARAM lParam, float timeElapsed);
	virtual void OnProcessMouseWheel(WPARAM wParam, LPARAM lParam);

	virtual void OnProcessKeyUp(WPARAM wParam, LPARAM lParam);
	virtual void OnProcessKeyDown(WPARAM wParam, LPARAM lParam);

protected: // 내부 함수

protected: // 변수
	RECT m_edgeSize{ 0, 0, 0, 0 };

	float m_angleDegree{ Zoom_Out_Max };
};

