#pragma once

class CCollisionObject;

class CFSMMgr
{
public:	// ������, �Ҹ���
	CFSMMgr();
	virtual ~CFSMMgr();

public: // ���� �Լ�
	void UpdateState(float timeElapsed, CCollisionObject* obj);

protected: // ���� �Լ�
	void PlayIdle(float timeElapsed, CCollisionObject* obj);
	void PlayWalk(float timeElapsed, CCollisionObject* obj);
	void PlayChase(float timeElapsed, CCollisionObject* obj);
	void PlayAttack(float timeElapsed, CCollisionObject* obj);
	void PlayDie(float timeElapsed, CCollisionObject* obj);

protected:	// ����
};

