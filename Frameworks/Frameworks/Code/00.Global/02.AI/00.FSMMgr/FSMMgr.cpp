#include "stdafx.h"
#include "FSMMgr.h"
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"

/// <summary>
/// 목적: FSM을 하나의 클래스로 관리하여 처리하기 위함
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-07
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CFSMMgr::CFSMMgr()
{
	// Warning : ColliderMgr 가져와야 함
}

CFSMMgr::~CFSMMgr()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
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
// 내부 함수
void CFSMMgr::PlayIdle(float timeElapsed, CBaseObject * obj)
{
	// Warning: Collider 오브젝트로 변환 해야할 듯

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
	// Warning: Collider 오브젝트로 변환 해야할 듯

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
	// Die 애니메이션 이후(혹은 몇 초 이후) 해당 오브젝트 동적 제거
}
