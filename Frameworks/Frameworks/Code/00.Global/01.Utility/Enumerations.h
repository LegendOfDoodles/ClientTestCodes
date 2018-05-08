#pragma once

/// <summary>
/// 목적: 여러 지점에서 사용할 나열형 데이터를 통합 관리한다.
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-08
/// </summary>

enum States {
	Idle,
	Walk,
	Chase,
	Attack,
	Die
};

enum CollisionType {
	SPHERE,
	SECTERFORM,
};

enum Minion_Species {
	Blue_Up,
	Blue_Down,
	Red_Up,
	Red_Down,
	Data_Prepare
};

enum ObjectType {
	SwordPlayer,
	StaffPlayer,
	BowPlayer,
	SwordMinion,
	StaffMinion,
	BowMinion,
	Loyde,
	CAM,
	GOLEM,
	FirstTower,
	SecnondTower,
	Nexus
};

