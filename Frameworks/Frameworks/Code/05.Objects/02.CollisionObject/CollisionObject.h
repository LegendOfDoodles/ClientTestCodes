#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"

class CWayFinder;

class CCollisionObject : public CBaseObject
{
public:	// 생성자, 소멸자
	CCollisionObject(CCreateMgr *pCreateMgr, int nMeshes = 1);
	virtual ~CCollisionObject();

public: // 공개 함수
	int tag{ 0 };
	bool CheckEnemyState(CCollisionObject* other);
	virtual bool Attackable(CCollisionObject* other);

	virtual void PlayIdle(float timeElapsed) { timeElapsed; }
	virtual void PlayWalk(float timeElapsed) { timeElapsed; }
	virtual void PlayChase(float timeElapsed, CWayFinder* pWayFinder) { timeElapsed; pWayFinder; }
	virtual void PlayAttack(float timeElapsed) { timeElapsed; }
	virtual void PlayDie(float timeElapsed) { timeElapsed; }
	virtual void PlayRemove(float timeElapsed) { timeElapsed; }

	virtual void RegenerateLookAt() {}

	virtual void SetCollisionSize(float size) { m_fCollisionSize = size; }
	virtual float GetCollisionSize() const { return m_fCollisionSize; }
	virtual XMFLOAT2 GetCollisionLevel() { return m_xmf2CollisionLevel; }
	virtual void ReceiveDamage(float Damage) { Damage; }

	virtual TeamType GetTeam() { return m_TeamType; }
	virtual void SetTeam(TeamType type) { m_TeamType = type; }

	StatesType GetState() { return m_curState; }
	virtual void SetState(StatesType newState) { m_curState = newState; }

	void SetNextState(StatesType newState) { m_nextState = newState; }
	void SetEnemy(CCollisionObject* enemy) { m_pEnemy = enemy; }

	virtual void SetCollisionManager(CCollisionManager* manager) { m_pColManager = manager; }
	void ResetCollisionLevel() {
		m_xmf2CollisionLevel.x = floor(GetPosition().x / (TERRAIN_SIZE_WIDTH / 50));
		m_xmf2CollisionLevel.y = floor(GetPosition().z / (TERRAIN_SIZE_WIDTH / 50));
	}
	void SetStatic(StaticType type) { m_StaticType = type; }
	StaticType GetStaticType() { return m_StaticType; }

	virtual PlayerInfo* GetPlayerStatus()			{ return NULL; }
	virtual CommonInfo* GetCommonStatus()			{ return NULL; }
	virtual StaticInfo* GetNexusAndTowerStatus()	{ return NULL; }
	
	float GetDetectRange() { return m_detectRange; }
	float GetSightRange() { return m_sightRange;}
	
	void SetDetected(bool value) { m_Detected = value; }
	bool GetDetected() { return m_Detected; }
	
	void GameOver() { m_GameOver = true; }
protected: // 내부 함수

protected: // 변수
	float m_fCollisionSize{ 1 };
	XMFLOAT2 m_xmf2CollisionLevel{ 0,0 };

	StatesType m_curState{ States::Idle };
	StatesType m_nextState{ States::Idle };
	TeamType m_TeamType{ None };
	bool m_Detected{ false };

	float m_sightRange{ 10.0f };
	float m_detectRange{ 0.0f };
	float m_attackRange{ 0.0f };

	CCollisionObject* m_pEnemy{ NULL };
	StaticType m_StaticType {StaticType::Move};
	CCollisionManager * m_pColManager{ NULL };
	bool m_GameOver{ false };
};
