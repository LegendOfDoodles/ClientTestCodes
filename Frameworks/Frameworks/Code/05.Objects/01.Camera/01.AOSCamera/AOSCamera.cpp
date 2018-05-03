#include "stdafx.h"
#include "AOSCamera.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// 목적: In Game 에서 사용할 카메라
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-04-14
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CAOSCamera::CAOSCamera() : CCamera()
{
}

CAOSCamera::~CAOSCamera()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CAOSCamera::Initialize(CCreateMgr * pCreateMgr)
{
	int width = pCreateMgr->GetWindowWidth();
	int height = pCreateMgr->GetWindowHeight();

	m_edgeSize.left = Window_Edge_Size;
	m_edgeSize.top = Window_Edge_Size;
	m_edgeSize.right = width - Window_Edge_Size;
	m_edgeSize.bottom = height - Window_Edge_Size;
	
	m_hWnd = pCreateMgr->GetHwnd();
	m_pCommandList = pCreateMgr->GetCommandList();

	SetViewport(0, 0, width, height, 0.0f, 1.0f);
	SetScissorRect(0, 0, width, height);
	GenerateProjectionMatrix(1.0f, 50000.0f, float(width) / float(height), m_angleDegree);
	GenerateViewMatrix(
		XMFLOAT3(0.0f, 600.0f, -200.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f));

	CreateShaderVariables(pCreateMgr);
}

void CAOSCamera::Move(DWORD direction, float distance, bool bVelocity)
{
	if (direction)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);

		if (direction & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, XMFLOAT3(0, 0, 1), distance);
		if (direction & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, XMFLOAT3(0, 0, 1), -distance);

		if (direction & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, XMFLOAT3(1, 0, 0), distance);
		if (direction & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, XMFLOAT3(1, 0, 0), -distance);

		if (direction & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, XMFLOAT3(0, 1, 0), distance);
		if (direction & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, XMFLOAT3(0, 1, 0), -distance);

		CCamera::Move(xmf3Shift);
	}
}

bool CAOSCamera::OnProcessMouseWheel(WPARAM wParam, LPARAM lParam)
{
	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	if (zDelta < 0)	// 휠 다운
	{
		if (m_angleDegree < Zoom_Out_Max)
			m_angleDegree += Zoom_Change_Value;
	}
	else    // 휠 업
	{
		if (m_angleDegree > Zoom_In_Max)
			m_angleDegree -= Zoom_Change_Value;
	}

	GenerateProjectionMatrix(1.0f, 50000.0f, float(m_scissorRect.right) / float(m_scissorRect.bottom), m_angleDegree);
	return true;
}

bool CAOSCamera::OnProcessMouseInput(UCHAR * pKeyBuffer)
{
	POINT cursorPos;
	DWORD direction{ NULL };

	GetCursorPos(&cursorPos);
	ScreenToClient(m_hWnd, &cursorPos);

	if (cursorPos.x < m_edgeSize.left)
	{
		direction |= DIR_LEFT;
	}
	if (cursorPos.x > m_edgeSize.right)
	{
		direction |= DIR_RIGHT;
	}
	if (cursorPos.y < m_edgeSize.top)
	{
		direction |= DIR_FORWARD;
	}
	if (cursorPos.y > m_edgeSize.bottom)
	{
		direction |= DIR_BACKWARD;
	}
	m_direction = direction;

	return true;
}

bool CAOSCamera::OnProcessKeyInput(UCHAR * pKeyBuffer)
{
	return true;
}

////////////////////////////////////////////////////////////////////////
// 내부 함수