#include "stdafx.h"
#include "FSMMgr.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"

/// <summary>
/// 목적: FSM을 하나의 클래스로 관리하여 처리하기 위함
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-11
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CFSMMgr::CFSMMgr()
{
}

CFSMMgr::~CFSMMgr()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
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
		obj->PlayChase(timeElapsed);
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
	default:
		assert(!"Error:: There is No State");
	}
	obj->Animate(timeElapsed);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CFSMMgr::PlayIdle(float timeElapsed, CCollisionObject * obj)
{
	// 주변에 적 존재하는지 검사
	// {
	//      해당 적의 상태 조사(Die / Remove 아닌지 확인)
	//       {
	//				SetEnemy(other);
	//              공격 범위 안에 있는지 검사 -> 성공 시 obj->SetState(States::Attack);
	//               -> 실패 시 obj->SetState(States::Chase); 
	//        }
	// }
}

void CFSMMgr::PlayWalk(float timeElapsed, CCollisionObject * obj)
{
	CAnimatedObject* animObj{ static_cast<CAnimatedObject*>(obj) };

	if (animObj->MoveToDestination(timeElapsed) == States::Done)
	{
		animObj->SetState(States::Idle);
		return;
	}

	// 주변에 적 존재하는지 검사
	// {
	//      해당 적의 상태 조사(Die / Remove 아닌지)
	//       {
	//				SetEnemy(other);
	//              공격 범위 안에 있는지 검사 -> 성공 시 obj->SetState(States::Attack);
	//               -> 실패 시 obj->SetState(States::Chase); 
	//        }
	// }
}

void CFSMMgr::PlayChase(float timeElapsed, CCollisionObject * obj)
{
	CAnimatedObject* animObj{ static_cast<CAnimatedObject*>(obj) };

	//   해당 적의 상태 조사(Die / Remove 아닌지, 범위 내에 있는지 확인)
	//   {
	//			animObj->MoveToEnemy(timeElapsed);
	//			SetEnemy(other);
	//             공격 범위 안에 있는지 검사 -> 성공 시 obj->SetState(States::Attack);
	//    } 
	//	   해당 적이 정상 상태가 아닌 경우
	//		SetEnemy(NULL);
	//     obj->SetState(States::Walk); 
}

void CFSMMgr::PlayAttack(float timeElapsed, CCollisionObject * obj)
{
	CAnimatedObject* animObj{ static_cast<CAnimatedObject*>(obj) };

	// 해당 적이 공격 가능한 상태인지 확인
	// 실패시 -> SetEnemy(NULL);	obj->SetNextState(States::Walk); 
	//	해당 적이 공격 범위 안에 있는지 검사 -> 실패 시 obj->SetNextState(States::Chase);
}

void CFSMMgr::PlayDie(float timeElapsed, CCollisionObject * obj)
{
	// 죽기 애니메이션 실행
	// 죽기 애니메이션 종료 후 자동으로 RemoveState로 변환
}

void CFSMMgr::PlayRemove(float timeElapsed, CCollisionObject * obj)
{
	// 제거 직전 스테이트
}
