#pragma once
// 여러 클래스에서 사용되는 나열형 정리

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