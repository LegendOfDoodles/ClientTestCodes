#pragma once
// ���� Ŭ�������� ���Ǵ� ������ ����

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