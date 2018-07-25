#include "stdafx.h"
#include "FlyingObject.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CFlyingObject::CFlyingObject(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CCollisionObject(pCreateMgr, nMeshes)
{
}

CFlyingObject::~CFlyingObject()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CFlyingObject::Animate(float timeElapsed)
{
}

////////////////////////////////////////////////////////////////////////
// 내부 함수