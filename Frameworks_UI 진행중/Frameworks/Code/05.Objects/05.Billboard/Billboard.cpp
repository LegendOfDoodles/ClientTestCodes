#include "stdafx.h"
#include "Billboard.h"

/// <summary>
/// ����: UI �̿��� ���� Bilboard Ŭ���� ����
/// ���� ������:  �̿뼱
/// ������ ���:  �̿뼱
/// ���� ���� ��¥: 2018-04-15
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CBillboardObject::CBillboardObject(CCreateMgr *pCreateMgr)
	: CBaseObject(pCreateMgr)
{
	CTexturedRectMesh *pRectMesh = new CTexturedRectMesh(pCreateMgr, 50.f, 50.0f, 0.f);
	SetMesh(0, pRectMesh);


	CreateShaderVariables(pCreateMgr);
}

CBillboardObject::~CBillboardObject()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CBillboardObject::Animate(float fTimeElapsed)
{
	if (m_pCamera != NULL) {
		XMFLOAT3 xmf3CameraPosition = m_pCamera->GetPosition();
		SetLookAt(xmf3CameraPosition);
	}
}

void CBillboardObject::SetLookAt(XMFLOAT3 & xmf3Target)
{
	// Up Vector�� 1�� �����ϰ� ���
	XMFLOAT3 xmf3Up(0.0f, 1.0f, 0.0f);
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT3 xmf3Look = Vector3::Normalize(Vector3::Subtract(xmf3Target, xmf3Position));
	XMFLOAT3 xmf3Right = Vector3::CrossProduct(xmf3Up, xmf3Look, true);

	// ���� Up ���͸� ī�޶��� �������� �����Ѵ�.
	xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

	m_xmf4x4World._11 = xmf3Right.x;	m_xmf4x4World._12 = xmf3Right.y;	m_xmf4x4World._13 = xmf3Right.z;
	m_xmf4x4World._21 = xmf3Up.x;		m_xmf4x4World._22 = xmf3Up.y;		m_xmf4x4World._23 = xmf3Up.z;
	m_xmf4x4World._31 = xmf3Look.x;		m_xmf4x4World._32 = xmf3Look.y;
}

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CMinimap::CMinimap(CCreateMgr * pCreateMgr)
	:CBillboardObject(pCreateMgr)
{
	m_xmf3Position = XMFLOAT3(0,0,0);

	CreateShaderVariables(pCreateMgr);
}

CMinimap::~CMinimap()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CMinimap::SetPos(XMFLOAT3 xmf3Position)
{
	m_xmf3Position = xmf3Position;

	// UI �� ������
	m_xmf3Position.x -= 1.5;
	m_xmf3Position.y -= 1.5;
}

void CMinimap::Animate(float fTimeElapsed)
{
	m_xmf4x4World = Matrix4x4::Identity();

	// �̴ϸ� ��ġ ������Ʈ (ī�޶� ȭ�� ��)
	// UI ��ü ��ġ
	m_xmf4x4World._41 = m_pCamera->GetPosition().x + (m_fDistance * m_pCamera->GetLookVector().x) + (6 * m_pCamera->GetRightVector().x) - (4 * m_pCamera->GetUpVector().x);
	m_xmf4x4World._42 = m_pCamera->GetPosition().y + (m_fDistance * m_pCamera->GetLookVector().y) + (6 * m_pCamera->GetRightVector().y) - (4 * m_pCamera->GetUpVector().y);
	m_xmf4x4World._43 = m_pCamera->GetPosition().z + (m_fDistance * m_pCamera->GetLookVector().z) + (6 * m_pCamera->GetRightVector().z) - (4 * m_pCamera->GetUpVector().z);

	// ���� UI
	/*
	m_xmf4x4World._41 += (m_xmf3Position.x * m_pCamera->GetRightVector().x) + (m_xmf3Position.y * m_pCamera->GetUpVector().x);
	m_xmf4x4World._42 += (m_xmf3Position.x * m_pCamera->GetRightVector().y) + (m_xmf3Position.y * m_pCamera->GetUpVector().y);
	m_xmf4x4World._43 += (m_xmf3Position.x * m_pCamera->GetRightVector().z) + (m_xmf3Position.y * m_pCamera->GetUpVector().z);
	*/

	// ���� ������ ������Ʈ
	CBillboardObject::Animate(fTimeElapsed);
}
