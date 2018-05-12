#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"

class CCollisionObject : public CBaseObject
{
public:	// ������, �Ҹ���
	CCollisionObject(CCreateMgr *pCreateMgr, int nMeshes = 1);
	virtual ~CCollisionObject();

public: // ���� �Լ�
	virtual void PlayIdle(float timeElapsed) {}
	virtual void PlayWalk(float timeElapsed) {}
	virtual void PlayChase(float timeElapsed) {}
	virtual void PlayAttack(float timeElapsed) {}
	virtual void PlayDie(float timeElapsed) {}
	virtual void PlayRemove(float timeElapsed) {}

	virtual void RegenerateLookAt() {}

	virtual void SetCollisionSize(float size) { m_fCollisionSize = size; }
	virtual float GetCollisionSize() { return m_fCollisionSize; }
	virtual XMFLOAT2 GetCollisionLevel() { return m_xmf2CollisionLevel; }
	virtual void ReceiveDamage(float Damage) {}

	virtual TeamType GetTeam() { return m_TeamType; }
	virtual void SetTeam(TeamType type) { m_TeamType = type; }

	StatesType GetState() { return m_curState; }
	virtual void SetState(StatesType newState) { m_curState = newState; }
	
	void SetNextState(StatesType newState) { m_nextState = newState; }

protected: // ���� �Լ�
	void ResetCollisionLevel() {
		m_xmf2CollisionLevel.x = floor(GetPosition().x/ (TERRAIN_SIZE_WIDTH / 50));
		m_xmf2CollisionLevel.y = floor(GetPosition().z/ (TERRAIN_SIZE_WIDTH / 50));
	}

protected: // ����
	float m_fCollisionSize{ 1 };
	XMFLOAT2 m_xmf2CollisionLevel{ 0,0 };

	StatesType m_curState{ States::Idle };
	StatesType m_nextState{ States::Idle };
	TeamType m_TeamType{ None };
	float m_detectRange{ 0.0f };
	float m_attackRange{ 0.0f };
};
