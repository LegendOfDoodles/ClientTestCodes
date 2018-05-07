#pragma once

class CBaseObject;

class CFSMMgr
{
public:	// 생성자, 소멸자
	CFSMMgr();
	virtual ~CFSMMgr();

public: // 공개 함수
	void UpdateState(float timeElapsed, CBaseObject* obj);

protected: // 내부 함수
	void PlayIdle(float timeElapsed, CBaseObject* obj);
	void PlayWalk(float timeElapsed, CBaseObject* obj);
	void PlayChase(float timeElapsed, CBaseObject* obj);
	void PlayAttack(float timeElapsed, CBaseObject* obj);
	void PlayDie(float timeElapsed, CBaseObject* obj);

protected:	// 변수
};

