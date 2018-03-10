#include "stdafx.h"
#include "AirplanePlayer.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

/// <summary>
/// ����: �׽�Ʈ�� �÷��̾� Ŭ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-01-13
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CAirplanePlayer::CAirplanePlayer(CCreateMgr *pCreateMgr)
	: CPlayer(pCreateMgr)
{
	m_pCamera = ChangeCamera(pCreateMgr, SPACESHIP_CAMERA, 0.0f);

	CreateShaderVariables(pCreateMgr);

	SetPosition(XMFLOAT3(0.0f, 0.0f, -50.0f));
}

CAirplanePlayer::~CAirplanePlayer()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
CCamera *CAirplanePlayer::ChangeCamera(CCreateMgr *pCreateMgr, DWORD newCameraMode, float timeElapsed)
{
	DWORD nCurrentCameraMode = (m_pCamera) ? m_pCamera->GetMode() : 0x00;
	if (nCurrentCameraMode == newCameraMode) return(m_pCamera);
	switch (newCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		SetFriction(200.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(FIRST_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		break;
	case SPACESHIP_CAMERA:
		SetFriction(125.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(400.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(SPACESHIP_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.0f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 0.0f, 0.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		break;
	case THIRD_PERSON_CAMERA:
		SetFriction(250.0f);
		SetGravity(XMFLOAT3(0.0f, 0.0f, 0.0f));
		SetMaxVelocityXZ(125.0f);
		SetMaxVelocityY(400.0f);
		m_pCamera = OnChangeCamera(THIRD_PERSON_CAMERA, nCurrentCameraMode);
		m_pCamera->SetTimeLag(0.25f);
		m_pCamera->SetOffset(XMFLOAT3(0.0f, 20.0f, -50.0f));
		m_pCamera->GenerateProjectionMatrix(1.01f, 5000.0f, ASPECT_RATIO, 60.0f);
		break;
	default:
		break;
	}
	Update(timeElapsed);

	m_pCamera->Initialize(pCreateMgr);

	return(m_pCamera);
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CAirplanePlayer::OnPrepareRender()
{
	CPlayer::OnPrepareRender();

	XMMATRIX mtxRotate = XMMatrixRotationRollPitchYaw(XMConvertToRadians(90.0f), 0.0f, 0.0f);
	m_xmf4x4World = Matrix4x4::Multiply(mtxRotate, m_xmf4x4World);
}