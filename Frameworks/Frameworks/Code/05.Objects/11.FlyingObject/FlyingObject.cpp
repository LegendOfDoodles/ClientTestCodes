#include "stdafx.h"
#include "FlyingObject.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CFlyingObject::CFlyingObject(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CCollisionObject(pCreateMgr, nMeshes)
{
}

CFlyingObject::~CFlyingObject()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CFlyingObject::Animate(float timeElapsed)
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�