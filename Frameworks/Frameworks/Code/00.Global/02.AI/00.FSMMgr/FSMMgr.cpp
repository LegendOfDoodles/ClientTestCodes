#include "stdafx.h"
#include "FSMMgr.h"
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"

/// <summary>
/// ����: FSM�� �ϳ��� Ŭ������ �����Ͽ� ó���ϱ� ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-05-07
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CFSMMgr::CFSMMgr()
{
	// Warning : ColliderMgr �����;� ��
}

CFSMMgr::~CFSMMgr()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CFSMMgr::UpdateState(float timeElapsed, CBaseObject * obj)
{
	if (!obj->IsCollider()) return;

	switch (obj->GetState())
	{
	case States::Idle:
		PlayIdle(timeElapsed, obj);
		break;
	case States::Walk:
		PlayWalk(timeElapsed, obj);
		break;
	case States::Chase:
		PlayChase(timeElapsed, obj);
		break;
	case States::Attack:
		PlayAttack(timeElapsed, obj);
		break;
	case States::Die:
		PlayDie(timeElapsed, obj);
		break;
	default:
		assert(!"Error:: There is No State");
	}
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CFSMMgr::PlayIdle(float timeElapsed, CBaseObject * obj)
{
	// Warning: Collider ������Ʈ�� ��ȯ �ؾ��� ��

	if (obj->HaveDieState())
	{

	}
	if (obj->HaveWalkState())
	{

	}
	if (obj->HaveAttackState())
	{

	}
	if (obj->HaveChaseState())
	{

	}
}

void CFSMMgr::PlayWalk(float timeElapsed, CBaseObject * obj)
{
	CAnimatedObject* animObj{ static_cast<CAnimatedObject*>(obj) };

	animObj->MoveToDestination(timeElapsed);

	if (animObj->HaveDieState())
	{

	}
	if (animObj->HaveAttackState())
	{

	}
	if (animObj->HaveChaseState())
	{

	}
}

void CFSMMgr::PlayChase(float timeElapsed, CBaseObject * obj)
{
	CAnimatedObject* animObj{ static_cast<CAnimatedObject*>(obj) };

	animObj->MoveToDestination(timeElapsed);

	if (animObj->HaveDieState())
	{

	}
	if (animObj->HaveAttackState())
	{

	}
	if (animObj->HaveChaseState())
	{

	}
}

void CFSMMgr::PlayAttack(float timeElapsed, CBaseObject * obj)
{
	// Warning: Collider ������Ʈ�� ��ȯ �ؾ��� ��

	if (obj->HaveDieState())
	{

	}
	if (obj->HaveChaseState())
	{

	}
	if (obj->HaveWalkState())
	{

	}
}

void CFSMMgr::PlayDie(float timeElapsed, CBaseObject * obj)
{
	// Die �ִϸ��̼� ����(Ȥ�� �� �� ����) �ش� ������Ʈ ���� ����
}
