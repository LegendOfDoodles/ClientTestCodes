#pragma once
#include "00.Global\01.Utility/08.EffectManager/EffectManager.h"

class CCollisionObject;
class CWayFinder;
struct NodeMap;
struct CharacterStatus {
	int HP;
	int Atk;
	CharacterStatus(int hp, int atk) {
		hp; atk;
	}
};



class CCollisionManager
{
public:
	CCollisionManager();
	void GameOver(TeamType type);
	void SetNodeMap(std::vector<NodeMap> map,float size, XMFLOAT2 wh);
	void AddCollider(CCollisionObject* pcol);
	void Update(shared_ptr<CWayFinder> pWayFinder);
	void RequestCollide(CollisionType type, CCollisionObject* pCol, float data1 = 0, float data2 = 0,float damage=0);
	
	CCollisionObject* RequestNearObject(CCollisionObject* pCol,float lengh, TeamType type,bool player=false);
	
	
	PlayerInfo* NearFightingValue(CCollisionObject* pCol, TeamType type);
	XMFLOAT2 GetFrontLinePosition(int line,TeamType type);
	std::list<CCollisionObject*>* GetEnemyList(CCollisionObject* pCol, TeamType type);
	std::list<CCollisionObject*>* GetTeamList(CCollisionObject* pCol, TeamType type);
	
	
	~CCollisionManager();
	int(*GetFoW(void))[NODE_HEIGHT];

	void SetEffectManager(shared_ptr<CEffectMgr> manager) { m_pEffectMgr = manager; }

	float GetnodeSize() { return nodeSize; }
	int GetEnemyCout() {
		return static_cast<int>(m_lstBlueSight.size());
	}
protected:
	bool NearLevel(XMFLOAT2 a, XMFLOAT2 b,bool attack = false) {

		if ((Vector2::Distance(a, b) <15&&!attack)|| (Vector2::Distance(a, b) <20&& attack))
			return true;
		else
			return false;
	}

	void SearchSight(int startX, int startY, int dir, int slength, TeamType team);


protected:
	TeamType m_Winner{ TeamType::None };
	TeamType m_User{ TeamType::Blue };
	NodeMap** m_BlueSight;
	NodeMap** m_RedSight;
	float nodeSize{0};
	XMFLOAT2 nodeWH;
	int Fow[NODE_WIDTH][NODE_HEIGHT];
	int BlueFow[NODE_WIDTH][NODE_HEIGHT];
	int RedFow[NODE_WIDTH][NODE_HEIGHT];

	shared_ptr<CEffectMgr> m_pEffectMgr;


	std::list<CCollisionObject*> m_lstColliders;
	
	std::list<CCollisionObject*> m_lstBlueSight;
	std::list<CCollisionObject*> m_lstRedSight;

	std::list<CCollisionObject*> m_lstReturn;


};
