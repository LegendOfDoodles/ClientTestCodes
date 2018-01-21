#include "stdafx.h"
#include "Player.h"
#include "CreateMgr.h"
#include "SpaceShipCamera.h"
#include "FirstPersonCamera.h"
#include "ThirdPersonCamera.h"

/// <summary>
/// 목적: 테스트용 플레이어 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-01-21
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CPlayer::CPlayer(CCreateMgr *pCreateMgr)
	: CBaseObject(pCreateMgr)
{
	m_hWnd = pCreateMgr->GetHwnd();
}

CPlayer::~CPlayer()
{
	if (m_pCamera)
	{
		m_pCamera->Finalize();
		Safe_Delete(m_pCamera);
	}
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CPlayer::Render(CCamera *pCamera)
{
	DWORD nCameraMode = (pCamera) ? pCamera->GetMode() : 0x00;
	if (nCameraMode == THIRD_PERSON_CAMERA)
	{
		UpdateShaderVariables();
		CBaseObject::Render(pCamera);
	}
}

void CPlayer::ProcessInput(float timeElapsed)
{
	static UCHAR pKeyBuffer[256];
	DWORD dwDirection = 0;
	if (::GetKeyboardState(pKeyBuffer))
	{
		if (pKeyBuffer[VK_UP] & 0xF0) dwDirection |= DIR_FORWARD;
		if (pKeyBuffer[VK_DOWN] & 0xF0) dwDirection |= DIR_BACKWARD;
		if (pKeyBuffer[VK_LEFT] & 0xF0) dwDirection |= DIR_LEFT;
		if (pKeyBuffer[VK_RIGHT] & 0xF0) dwDirection |= DIR_RIGHT;
		if (pKeyBuffer[VK_PRIOR] & 0xF0) dwDirection |= DIR_UP;
		if (pKeyBuffer[VK_NEXT] & 0xF0) dwDirection |= DIR_DOWN;
	}
	float cxDelta = 0.0f, cyDelta = 0.0f;
	POINT cursorPos;
	if (::GetCapture() == m_hWnd)
	{
		::SetCursor(NULL);
		::GetCursorPos(&cursorPos);
		cxDelta = (float)(cursorPos.x - m_pOldCursorPos->x) / 3.0f;
		cyDelta = (float)(cursorPos.y - m_pOldCursorPos->y) / 3.0f;
		::SetCursorPos(m_pOldCursorPos->x, m_pOldCursorPos->y);
	}
	if ((dwDirection != 0) || (cxDelta != 0.0f) || (cyDelta != 0.0f))
	{
		if (cxDelta || cyDelta)
		{
			if (pKeyBuffer[VK_RBUTTON] & 0xF0)
				Rotate(cyDelta, 0.0f, -cxDelta);
			else
				Rotate(cyDelta, cxDelta, 0.0f);
		}
		if (dwDirection) Move(dwDirection, 50.0f *  timeElapsed, true);
	}
	Update(timeElapsed);
}

void CPlayer::Move(DWORD direction, float distance, bool bVelocity)
{
	if (direction)
	{
		XMFLOAT3 xmf3Shift = XMFLOAT3(0, 0, 0);

		if (direction & DIR_FORWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, distance);
		if (direction & DIR_BACKWARD) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Look, -distance);

		if (direction & DIR_RIGHT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, distance);
		if (direction & DIR_LEFT) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Right, -distance);

		if (direction & DIR_UP) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, distance);
		if (direction & DIR_DOWN) xmf3Shift = Vector3::Add(xmf3Shift, m_xmf3Up, -distance);

		Move(xmf3Shift, bVelocity);
	}
}

void CPlayer::Move(XMFLOAT3& xmf3Shift, bool bUpdateVelocity)
{
	if (bUpdateVelocity)
	{
		m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, xmf3Shift);
	}
	else
	{
		m_xmf3Position = Vector3::Add(m_xmf3Position, xmf3Shift);
		m_pCamera->Move(xmf3Shift);
	}
}

void CPlayer::Rotate(float x, float y, float z)
{
	DWORD nCameraMode = m_pCamera->GetMode();
	if ((nCameraMode == FIRST_PERSON_CAMERA) || (nCameraMode == THIRD_PERSON_CAMERA))
	{
		if (x != 0.0f)
		{
			m_fPitch += x;
			if (m_fPitch > +89.0f) { x -= (m_fPitch - 89.0f); m_fPitch = +89.0f; }
			if (m_fPitch < -89.0f) { x -= (m_fPitch + 89.0f); m_fPitch = -89.0f; }
		}
		if (y != 0.0f)
		{
			m_fYaw += y;
			if (m_fYaw > 360.0f) m_fYaw -= 360.0f;
			if (m_fYaw < 0.0f) m_fYaw += 360.0f;
		}
		if (z != 0.0f)
		{
			m_fRoll += z;
			if (m_fRoll > +20.0f) { z -= (m_fRoll - 20.0f); m_fRoll = +20.0f; }
			if (m_fRoll < -20.0f) { z -= (m_fRoll + 20.0f); m_fRoll = -20.0f; }
		}
		m_pCamera->Rotate(x, y, z);
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up),
				XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}
	else if (nCameraMode == SPACESHIP_CAMERA)
	{
		m_pCamera->Rotate(x, y, z);
		if (x != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Right), XMConvertToRadians(x));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
		}
		if (y != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Up), XMConvertToRadians(y));
			m_xmf3Look = Vector3::TransformNormal(m_xmf3Look, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
		if (z != 0.0f)
		{
			XMMATRIX xmmtxRotate = XMMatrixRotationAxis(XMLoadFloat3(&m_xmf3Look), XMConvertToRadians(z));
			m_xmf3Up = Vector3::TransformNormal(m_xmf3Up, xmmtxRotate);
			m_xmf3Right = Vector3::TransformNormal(m_xmf3Right, xmmtxRotate);
		}
	}

	m_xmf3Look = Vector3::Normalize(m_xmf3Look);
	m_xmf3Right = Vector3::CrossProduct(m_xmf3Up, m_xmf3Look, true);
	m_xmf3Up = Vector3::CrossProduct(m_xmf3Look, m_xmf3Right, true);
}

void CPlayer::Update(float timeElapsed)
{
	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Gravity, timeElapsed, false));

	float fLength = sqrtf(m_xmf3Velocity.x * m_xmf3Velocity.x + m_xmf3Velocity.z * m_xmf3Velocity.z);
	float fMaxVelocityXZ = m_fMaxVelocityXZ * timeElapsed;

	if (fLength > m_fMaxVelocityXZ)
	{
		m_xmf3Velocity.x *= (fMaxVelocityXZ / fLength);
		m_xmf3Velocity.z *= (fMaxVelocityXZ / fLength);
	}

	float fMaxVelocityY = m_fMaxVelocityY * timeElapsed;
	fLength = sqrtf(m_xmf3Velocity.y * m_xmf3Velocity.y);
	if (fLength > m_fMaxVelocityY) m_xmf3Velocity.y *= (fMaxVelocityY / fLength);

	Move(m_xmf3Velocity, false);

	if (m_pPlayerUpdatedContext) OnPlayerUpdateCallback(timeElapsed);
	DWORD nCameraMode = m_pCamera->GetMode();

	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->Update(m_xmf3Position, timeElapsed);

	if (m_pCameraUpdatedContext) OnCameraUpdateCallback(timeElapsed);

	if (nCameraMode == THIRD_PERSON_CAMERA) m_pCamera->SetLookAt(m_xmf3Position);

	m_pCamera->RegenerateViewMatrix();

	fLength = Vector3::Length(m_xmf3Velocity);
	float fDeceleration = (m_fFriction * timeElapsed);
	if (fDeceleration > fLength) fDeceleration = fLength;

	m_xmf3Velocity = Vector3::Add(m_xmf3Velocity, Vector3::ScalarProduct(m_xmf3Velocity, -fDeceleration, true));
}

void CPlayer::OnPlayerUpdateCallback(float timeElapsed)
{
}

void CPlayer::OnCameraUpdateCallback(float timeElapsed)
{
}

void CPlayer::SetPosition(XMFLOAT3& xmf3Position)
{
	Move(XMFLOAT3(
		xmf3Position.x - m_xmf3Position.x, 
		xmf3Position.y - m_xmf3Position.y, 
		xmf3Position.z - m_xmf3Position.z),	
		false);
}

CCamera *CPlayer::OnChangeCamera(DWORD nNewCameraMode, DWORD nCurrentCameraMode)
{
	CCamera *pNewCamera{ NULL };

	switch (nNewCameraMode)
	{
	case FIRST_PERSON_CAMERA:
		pNewCamera = new CFirstPersonCamera(m_pCamera);
		break;
	case THIRD_PERSON_CAMERA:
		pNewCamera = new CThirdPersonCamera(m_pCamera);
		break;
	case SPACESHIP_CAMERA:
		pNewCamera = new CSpaceShipCamera(m_pCamera);
		break;
	}

	if (nCurrentCameraMode == SPACESHIP_CAMERA)
	{
		m_xmf3Right = Vector3::Normalize(XMFLOAT3(m_xmf3Right.x, 0.0f, m_xmf3Right.z));
		m_xmf3Up = Vector3::Normalize(XMFLOAT3(0.0f, 1.0f, 0.0f));
		m_xmf3Look = Vector3::Normalize(XMFLOAT3(m_xmf3Look.x, 0.0f, m_xmf3Look.z));
		m_fPitch = 0.0f;
		m_fRoll = 0.0f;
		m_fYaw = Vector3::Angle(XMFLOAT3(0.0f, 0.0f, 1.0f), m_xmf3Look);
		if (m_xmf3Look.x < 0.0f) m_fYaw = -m_fYaw;
	}
	else if ((nNewCameraMode == SPACESHIP_CAMERA) && m_pCamera)
	{
		m_xmf3Right = m_pCamera->GetRightVector();
		m_xmf3Up = m_pCamera->GetUpVector();
		m_xmf3Look = m_pCamera->GetLookVector();
	}
	if (pNewCamera)
	{
		pNewCamera->SetMode(nNewCameraMode);
		pNewCamera->SetPlayer(this);
	}
	
	if(m_pCamera) m_pCamera->Finalize();
	Safe_Delete(m_pCamera);

	return(pNewCamera);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CPlayer::CreateShaderVariables(CCreateMgr *pCreateMgr)
{
	UINT elementBytes = ((sizeof(CB_PLAYER_INFO) + 255) & ~255); //256의 배수

	m_pConstBuffer = pCreateMgr->CreateBufferResource(
		NULL,
		elementBytes,
		D3D12_HEAP_TYPE_UPLOAD, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, 
		NULL);

	m_pConstBuffer->Map(0, NULL, (void **)&m_pMappedPlayer);
}

void CPlayer::ReleaseShaderVariables()
{
	if (!m_pConstBuffer) return;

	m_pConstBuffer->Unmap(0, NULL);
	Safe_Release(m_pConstBuffer);

	CBaseObject::ReleaseShaderVariables();
}

void CPlayer::UpdateShaderVariables()
{
	XMStoreFloat4x4(&m_pMappedPlayer->m_xmf4x4World, XMMatrixTranspose(XMLoadFloat4x4(&m_xmf4x4World)));

	D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress = m_pConstBuffer->GetGPUVirtualAddress();
	m_pCommandList->SetGraphicsRootConstantBufferView(0, gpuVirtualAddress);
}

void CPlayer::OnPrepareRender()
{
	m_xmf4x4World._11 = m_xmf3Right.x; m_xmf4x4World._12 = m_xmf3Right.y;	m_xmf4x4World._13 = m_xmf3Right.z;
	m_xmf4x4World._21 = m_xmf3Up.x; m_xmf4x4World._22 = m_xmf3Up.y; m_xmf4x4World._23 = m_xmf3Up.z;
	m_xmf4x4World._31 = m_xmf3Look.x; m_xmf4x4World._32 = m_xmf3Look.y; m_xmf4x4World._33 = m_xmf3Look.z;
	m_xmf4x4World._41 = m_xmf3Position.x; m_xmf4x4World._42 = m_xmf3Position.y; m_xmf4x4World._43 = m_xmf3Position.z;
}