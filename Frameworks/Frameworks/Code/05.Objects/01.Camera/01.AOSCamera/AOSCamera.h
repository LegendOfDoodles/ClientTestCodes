#pragma once
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"

#define Window_Edge_Size 40

class CAOSCamera : public CCamera
{
public:	// ������, �Ҹ���
	CAOSCamera();
	virtual ~CAOSCamera();

public:	// ���� �Լ�
	virtual void Initialize(CCreateMgr *pCreateMgr) override;

	virtual void Move(DWORD direction, float distance, bool bVelocity) override;

	virtual void OnProcessMouseMove(WPARAM wParam, LPARAM lParam, float timeElapsed);

protected: // ���� �Լ�

protected: // ����
	RECT m_edgeSize{ 0, 0, 0, 0 };
};

