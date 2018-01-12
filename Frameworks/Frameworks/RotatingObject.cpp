#include "stdafx.h"
#include "RotatingObject.h"

/// <summary>
/// ����: Create ���� �� �׽�Ʈ �� Ŭ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-01-12
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CRotatingObject::CRotatingObject()
{
	m_rotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_rotationSpeed = 90.0f;
}

CRotatingObject::~CRotatingObject()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CRotatingObject::Animate(float timeElapsed)
{
	CBaseObject::Rotate(&m_rotationAxis, m_rotationSpeed * timeElapsed);
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�