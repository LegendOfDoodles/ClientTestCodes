#include "stdafx.h"
#include "AOSCamera.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// ����: In Game ���� ����� ī�޶�
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-04-07
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CAOSCamera::CAOSCamera() : CCamera()
{
}

CAOSCamera::~CAOSCamera()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
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
		XMFLOAT3(0.0f, 300.0f, -100.0f),
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

void CAOSCamera::OnProcessMouseMove(WPARAM wParam, LPARAM lParam, float timeElapsed)
{
	int mx = LOWORD(lParam);
	int my = HIWORD(lParam);

	if (mx < m_edgeSize.left)
	{
		m_direction |= DIR_LEFT;
	}
	else
	{
		m_direction &= ~DIR_LEFT;
	}

	if (mx > m_edgeSize.right)
	{
		m_direction |= DIR_RIGHT;
	}
	else
	{
		m_direction &= ~DIR_RIGHT;
	}

	if (my < m_edgeSize.top)
	{
		m_direction |= DIR_FORWARD;
	}
	else
	{
		m_direction &= ~DIR_FORWARD;
	}

	if(my > m_edgeSize.bottom)
	{
		m_direction |= DIR_BACKWARD;
	}
	else
	{
		m_direction &= ~DIR_BACKWARD;
	}
}

void CAOSCamera::OnProcessMouseWheel(WPARAM wParam, LPARAM lParam, float timeElapsed)
{
	short zDelta = GET_WHEEL_DELTA_WPARAM(wParam);

	if (zDelta < 0)	// �� �ٿ�
	{
		if (m_angleDegree < Zoom_Out_Max)
			m_angleDegree += Zoom_Change_Value;
	}
	else    // �� ��
	{
		if (m_angleDegree > Zoom_In_Max)
			m_angleDegree -= Zoom_Change_Value;
	}

	GenerateProjectionMatrix(1.0f, 50000.0f, float(m_scissorRect.right) / float(m_scissorRect.bottom), m_angleDegree);
}

void CAOSCamera::OnProcessKeyUp(WPARAM wParam, LPARAM lParam, float timeElapsed)
{
}

void CAOSCamera::OnProcessKeyDown(WPARAM wParam, LPARAM lParam, float timeElapsed)
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�