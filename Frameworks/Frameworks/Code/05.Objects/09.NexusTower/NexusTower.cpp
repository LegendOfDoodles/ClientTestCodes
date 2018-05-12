#include "stdafx.h"
#include "NexusTower.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CNexusTower::CNexusTower(CCreateMgr *pCreateMgr, int nMeshes) : CCollisionObject(pCreateMgr, nMeshes)
{
	m_StatusInfo.maxHP = 1000;
	m_StatusInfo.HP = 1000;
}


CNexusTower::~CNexusTower()
{
}

void CNexusTower::Animate(float timeElapsed)
{
	m_fEndTime += timeElapsed;
	switch (m_curState)
	{
	case States::Idle:
		break;
	case States::Die:
		Translate(new XMFLOAT3(0, -1, 0));
		Translate(new XMFLOAT3(0, sin(m_fEndTime)*1.f, 0));
		break;
	case States::Remove:
		break;
	default:
		assert(!"Error:: There is No State");
	}
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�


////////////////////////////////////////////////////////////////////////
// ���� �Լ�