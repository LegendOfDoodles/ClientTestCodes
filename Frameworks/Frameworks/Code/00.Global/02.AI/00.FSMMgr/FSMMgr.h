#pragma once

class CBaseObject;

class CFSMMgr
{
public:	// ������, �Ҹ���
	CFSMMgr();
	virtual ~CFSMMgr();

public: // ���� �Լ�
	void UpdateState(float timeElapsed, CBaseObject* obj);

protected: // ���� �Լ�
	void PlayIdle(float timeElapsed, CBaseObject* obj);
	void PlayWalk(float timeElapsed, CBaseObject* obj);
	void PlayChase(float timeElapsed, CBaseObject* obj);
	void PlayAttack(float timeElapsed, CBaseObject* obj);
	void PlayDie(float timeElapsed, CBaseObject* obj);

protected:	// ����
};

