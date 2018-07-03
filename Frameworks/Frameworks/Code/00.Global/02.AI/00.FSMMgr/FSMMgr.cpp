#include "stdafx.h"
#include "FSMMgr.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"

/// <summary>
/// ����: FSM�� �ϳ��� Ŭ������ �����Ͽ� ó���ϱ� ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-07-03
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CFSMMgr::CFSMMgr(CWayFinder* pWayFinder)
{
	m_pWayFinder = pWayFinder;
}

CFSMMgr::~CFSMMgr()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CFSMMgr::Update(float timeElapsed, CCollisionObject * obj)
{
	switch (obj->GetState())
	{
	case States::Idle:
		obj->PlayIdle(timeElapsed);
		break;
	case States::Walk:
		obj->PlayWalk(timeElapsed);
		break;
	case States::Chase:
		obj->PlayChase(timeElapsed, m_pWayFinder);
		break;
	case States::Attack:
		obj->PlayAttack(timeElapsed);
		break;
	case States::Die:
		obj->PlayDie(timeElapsed);
		break;
	case States::Remove:
		obj->PlayRemove(timeElapsed);
		break;
	case States::Win:
		break;
	case States::Defeat:
		break;
	default:
		assert(!"Error:: There is No State");
	}
	obj->Animate(timeElapsed);
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�