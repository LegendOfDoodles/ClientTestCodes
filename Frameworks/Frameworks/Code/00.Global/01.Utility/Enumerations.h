#pragma once

/// <summary>
/// ����: ���� �������� ����� ������ �����͸� ���� �����Ѵ�.
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-05-08
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

