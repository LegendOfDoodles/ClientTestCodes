#include "stdafx.h"
#include "FlyingObject.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CFlyingObject::CFlyingObject(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CCollisionObject(pCreateMgr, nMeshes)
{
	m_ObjectType = ObjectType::FlyingObject;
}

CFlyingObject::~CFlyingObject()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CFlyingObject::Animate(float timeElapsed)
{
	if (m_flyingObjectType == FlyingObjectType::Roider_Dumbel)
	{
		m_distance += timeElapsed * m_speed;
		MoveToDirection(timeElapsed * m_speed);
		// Warning! ���� ó�� �ʿ�
		// �Ź� �˻� �� ��� �����ɸ����� Ȯ�� �ʿ�
		// m_pColManager->RequestCollide(CollisionType::SPHERE, this, m_attackRange, m_attackRange, m_damage * timeElapsed);
		if (m_distance > m_maxDistance)
		{
			m_curState = StatesType::Remove;
		}
	}
}

void CFlyingObject::SetFlyingObjectsType(FlyingObjectType type)
{
	m_flyingObjectType = type;

	if (type == FlyingObjectType::Roider_Dumbel)
	{
		m_fCollisionSize = CONVERT_PaperUnit_to_InG(1);
		m_attackRange = 0.0f;
		m_damage = 0.0f;
		m_distance = 0.0f;
		m_maxDistance = 0.0f;
		m_speed = CONVERT_cm_to_InG(1.805f);
	}
}

void CFlyingObject::SetDirection(const XMFLOAT3 & direction)
{
	m_direction = direction;
	LookAt(Vector3::ScalarProduct(direction, 100000, false));
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CFlyingObject::LookAt(XMFLOAT3 target)
{
	if (m_curState == States::Attack) return;
	if (m_curState == States::Win) return;
	if (m_curState == States::Defeat) return;

	XMFLOAT3 upVector{ 0.f, 1.f, 0.f };
	XMFLOAT3 objLookVector = GetLook();
	XMFLOAT3 objPos = GetPosition();

	target.y = objPos.y;

	XMFLOAT3 towardVector = Vector3::Subtract(target, GetPosition(), true);

	float angle{ Vector3::DotProduct(towardVector, objLookVector) };
	angle = XMConvertToDegrees(acos(angle));

	if (isnan(angle)) return;

	float check{ Vector3::DotProduct(Vector3::CrossProduct(towardVector, objLookVector), upVector) };

	// ȸ�� ���� ����
	if (check < 0.0f)
		Rotate(0.0f, 0.0f, -angle);
	else if (check > 0.0f)
		Rotate(0.0f, 0.0f, angle);
}

void CFlyingObject::LookAt(XMFLOAT2 target)
{
	LookAt(XMFLOAT3(target.x, 0, target.y));
}

XMFLOAT3 CFlyingObject::GetLook()
{
	return(Vector3::ScalarProduct(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23), -1));
}

XMFLOAT3 CFlyingObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

void CFlyingObject::MoveToDirection(float dist)
{
	CBaseObject::SetPosition(Vector3::Add(GetPosition(), m_direction, dist));
}
