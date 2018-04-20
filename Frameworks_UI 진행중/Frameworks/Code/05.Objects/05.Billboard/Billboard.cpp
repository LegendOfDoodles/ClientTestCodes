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
	CTexturedRectMesh *pRectMesh = new CTexturedRectMesh(pCreateMgr, 5.f, 5.0f, 0.f);
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
		SetLookAt();
	}
}

void CBillboardObject::SetLookAt()
{
	// Up Vector�� 1�� �����ϰ� ���
	XMFLOAT3 xmf3Up = m_pCamera->GetUpVector();
	XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	XMFLOAT3 xmf3Look = Vector3::ScalarProduct(m_pCamera->GetLookVector(), -1);
	XMFLOAT3 xmf3Right = Vector3::ScalarProduct(m_pCamera->GetRightVector(), -1);

	// ���� Up ���͸� ī�޶��� �������� �����Ѵ�.
	//xmf3Up = Vector3::CrossProduct(xmf3Look, xmf3Right, true);

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
	CBillboardObject::Animate(fTimeElapsed);

	// �̴ϸ� ��ġ ������Ʈ (ī�޶� ȭ�� ��)
	// UI ��ü ��ġ
	XMFLOAT3 newPos = Vector3::Add(m_pCamera->GetPosition(), Vector3::ScalarProduct(m_pCamera->GetLookVector(), m_fDistance));
	newPos = Vector3::Add(Vector3::Add(newPos, Vector3::ScalarProduct(m_pCamera->GetUpVector(), -7.f)), Vector3::ScalarProduct(m_pCamera->GetRightVector(), 10.f));
	
	m_xmf4x4World._41 = newPos.x; 
	m_xmf4x4World._42 = newPos.y; 
	m_xmf4x4World._43 = newPos.z; 
}
