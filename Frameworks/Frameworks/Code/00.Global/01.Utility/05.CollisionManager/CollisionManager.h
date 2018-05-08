#pragma once

class CCollisionObject;
class CWayFinder;

struct CharacterStatus {
	int HP;
	int Atk;
	CharacterStatus(int hp, int atk) {

	}
};


class CCollisionManager
{
	std::list<CCollisionObject*> m_lstColliders;
	int m_nCollisers;
public:
	CCollisionManager();
	void AddCollider(CCollisionObject* pcol);
	void Update(CWayFinder* pWayFinder);
	~CCollisionManager();

protected:
	bool NearLevel(XMFLOAT2 a, XMFLOAT2 b) {
		if (Vector2::Distance(a, b) < 2)
			return true;
		else
			return false;
	}
};

