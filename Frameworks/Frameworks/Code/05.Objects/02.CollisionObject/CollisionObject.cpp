#include "stdafx.h"
#include "CollisionObject.h"

/// <summary>
/// ����: �浹ü�� �ִ� ������Ʈ ���п� Ŭ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-07-20
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CCollisionObject::CCollisionObject(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CBaseObject(pCreateMgr, nMeshes)
{
	ResetCollisionLevel();
}

CCollisionObject::~CCollisionObject()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
bool CCollisionObject::CheckEnemyState(CCollisionObject * other)
{
	if (!other) return false;
	if (other->GetState() == States::Die) return false;
	if (other->GetState() == States::Remove) return false;
	return true;
}

bool CCollisionObject::Attackable(CCollisionObject * other)
{
	float dstSqr = Vector3::DistanceSquare(GetPosition(), other->GetPosition());
	return (dstSqr < m_attackRange * m_attackRange);
}
