#pragma once
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
	CCollisionObject* RequestNearObject(CCollisionObject* pCol,float lengh);
	~CCollisionManager();
	int(*GetFoW(void))[NODE_HEIGHT];


	NodeMap** GetNodeMap() { return m_nodeMap; }
	float GetnodeSize() { return nodeSize; }
	int GetEnemyCout() {
		return static_cast<int>(m_lstBlueSight.size());
	}
protected:
	bool NearLevel(XMFLOAT2 a, XMFLOAT2 b) {
		if (Vector2::Distance(a, b) < 2)
			return true;
		else
			return false;
	}

	void SearchSight(XMFLOAT2 startpos, int dir, int slength,bool UserTeam = true);


protected:
	TeamType m_Winner{ TeamType::None };
	TeamType m_User{ TeamType::Blue };
	NodeMap** m_nodeMap;
	float nodeSize{0};
	XMFLOAT2 nodeWH;
	int Fow[NODE_WIDTH][NODE_HEIGHT];


	std::list<CCollisionObject*> m_lstColliders;
	
	std::list<CCollisionObject*> m_lstBlueSight;
	std::list<CCollisionObject*> m_lstRedSight;
};

