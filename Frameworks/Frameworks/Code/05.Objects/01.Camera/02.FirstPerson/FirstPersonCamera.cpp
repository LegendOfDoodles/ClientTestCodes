#include "stdafx.h"
#include "FirstPersonCamera.h"
#include "05.Objects/03.Player/00.BasePlayer/Player.h"

/// <summary>
/// ����: ī�޶� �׽�Ʈ ��
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-01-13
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CFirstPersonCamera::CFirstPersonCamera(CCamera *pCamera)
	: CCamera(pCamera)
{
	m_nMode = FIRST_PERSON_CAMERA;

	if (pCamera && pCamera->GetMode() == SPACESHIP_CAMERA)
	{
		m_xmf3Up = XMFLOAT3(0.0f, 1.0f, 0.0f);
		m_xmf3Right.y = 0.0f;
		m_xmf3Look.y = 0.0f;
		m_xmf3Right = Vector3::Normalize(m_xmf3Right);
		m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	}
}

CFirstPersonCamera::~CFirstPersonCamera()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CFirstPersonCamera::Rotate(float x, float y, float z)
{
	bool Rotated{ false };
	XMMATRIX xmmtxRotate;

	if (x != 0.0f)
	{
		xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right),
			XMConvertToRadians(x));

		Rotated = true;
	}
	if (m_pPlayer && (y != 0.0f))
	{
		XMFLOAT3 xmf3Up = m_pPlayer->GetUpVector();
		xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Up),
			XMConvertToRadians(y));

		Rotated = true;
	}
	if (m_pPlayer && (z != 0.0f))
	{
		XMFLOAT3 xmf3Look = m_pPlayer->GetLookVector();
		xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&xmf3Look),
			XMConvertToRadians(z));
		m_xmf3Position = Vector3::Subtract(m_xmf3Position, m_pPlayer->GetPosition());
		m_xmf3Position = Vector3::TransformCoord(m_xmf3Position, xmmtxRotate);
		m_xmf3Position = Vector3::Add(m_xmf3Position, m_pPlayer->GetPosition());

		Rotated = true;
	}
	if (Rotated)
	{
		m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
		m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
	}
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�