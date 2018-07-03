#include "stdafx.h"
#include "CollisionManager.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

CCollisionManager::CCollisionManager()
{

}

void CCollisionManager::GameOver(TeamType type)
{
	if (type == TeamType::Blue) {
		m_Winner = TeamType::Red;
	}
	if (type == TeamType::Red) {
		m_Winner = TeamType::Blue;
	}
	for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
	{
		(*i)->GameOver();
		if ((*i)->GetTeam() == m_Winner) {
			(*i)->SetState(StatesType::Win);
		}
		else
			(*i)->SetState(StatesType::Defeat);
	}
}

void CCollisionManager::SetNodeMap(std::vector<NodeMap> map, float size, XMFLOAT2 wh)
{
	m_nodeMap = new NodeMap*[static_cast<int>(wh.x)];
	for (int i = 0; i < wh.x; ++i) {
		m_nodeMap[i] = new NodeMap[static_cast<int>(wh.y)];
	}
	for (int i = 0; i < wh.y; ++i) {
		for (int j = 0; j < wh.x; ++j) {


			m_nodeMap[j][i] = map[(int)((i*wh.x) + j)];
		}
	}
	nodeSize = size;
	nodeWH = wh;
}

void CCollisionManager::AddCollider(CCollisionObject* pcol)
{
	if (pcol) {
		if (pcol->GetTeam() == TeamType::Blue)
			pcol->SetDetected(true);
		else
			pcol->SetDetected(false);

		m_lstColliders.push_back(pcol);
		m_nCollisers++;
	}
}


void CCollisionManager::Update(CWayFinder* pWayFinder)
{
	if (m_Winner == TeamType::None)
	{
		int cnt = 0;
		m_lstColliders.remove_if([](CCollisionObject* obj) { return obj->GetState() == States::Die; });
		for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
		{
			if ((*i)->GetTeam() != m_User) {
				(*i)->SetDetected(false);
			}
		}
		for (int i = 0; i < nodeWH.x; ++i) {
			for (int j = 0; j < nodeWH.y; ++j) {

				m_nodeMap[i][j].Detected = false;

			}
		}

		for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
		{
			if ((*i)->GetStaticType() != StaticType::Static) {
				
				for (auto& j = m_lstColliders.begin(); j != m_lstColliders.end(); ++j)
				{
					if (i != j && (*j)->GetStaticType() != StaticType::Static) {
						if (NearLevel((*i)->GetCollisionLevel(), (*j)->GetCollisionLevel()))
						{
							cnt++;
							float sizeA = (*i)->GetCollisionSize();
							float sizeB = (*j)->GetCollisionSize();

							float distance = Vector3::Distance((*i)->GetPosition(), (*j)->GetPosition());
							float collisionLength = sizeA + sizeB;
							if (distance < collisionLength)
							{
								float length = (collisionLength - distance);
								XMFLOAT3 vec3 = Vector3::Subtract((*i)->GetPosition(), (*j)->GetPosition());
								vec3.y = 0;
								vec3 = Vector3::Normalize(vec3);
								pWayFinder->AdjustValueByWallCollision((*i), vec3, length *sizeB / (sizeA + sizeB));
								pWayFinder->AdjustValueByWallCollision((*j), vec3, -length * sizeB / (sizeA + sizeB));
								(*i)->RegenerateLookAt();
								(*j)->RegenerateLookAt();
							}
						}
					}
				}
			}


			if (m_User == (*i)->GetTeam()) {
				int x, y;
				x = static_cast<int>((*i)->GetPosition().x / nodeSize);
				y = static_cast<int>((*i)->GetPosition().z / nodeSize);
				int startLength = static_cast<int>((*i)->GetSightRange() / nodeSize);
				if ((*i)->tag == 1) {
					for (int dir = 0; dir < 8; ++dir) {
						SearchSight(XMFLOAT2(static_cast<float>(x), static_cast<float>(y)), dir, 0, startLength);
					}
				}
				else {
					for (int dir = 0; dir < 8; ++dir) {
						SearchSight(XMFLOAT2(static_cast<float>(x), static_cast<float>(y)), dir, 0, startLength);
					}
				}
			}
		}

		for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
		{
			XMFLOAT2 pos;
			pos.x = (*i)->GetPosition().x / nodeSize;
			pos.y = (*i)->GetPosition().z / nodeSize;
			if (m_nodeMap[(int)pos.x][(int)pos.y].Detected) {
				(*i)->SetDetected(true);
			}
		}

	}
}
// type Sphere data1 = 거리 , data2 = 반지름;
// typeSecterForm data1 = 거리, data2 = 각도
void CCollisionManager::RequestCollide(CollisionType type, CCollisionObject * pCol, float data1, float data2, float damage)
{
	if (m_Winner == TeamType::None)
	{
		switch (type)
		{
			//8:1..
		case CollisionType::SPHERE:
		{
			for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
			{
				if ((*i) != pCol) {
					TeamType a = (*i)->GetTeam();
					TeamType b = pCol->GetTeam();
					if (a != b) {

						if (NearLevel((*i)->GetCollisionLevel(), pCol->GetCollisionLevel()))
						{
							XMFLOAT2 apos = XMFLOAT2((*i)->GetPosition().x, (*i)->GetPosition().z);
							XMFLOAT2 bpos = XMFLOAT2(pCol->GetPosition().x, pCol->GetPosition().z);
							XMFLOAT2 look = XMFLOAT2(pCol->GetLook().x, pCol->GetLook().z);
							float distance = Vector2::Distance(apos, Vector2::Add(bpos, Vector2::ScalarProduct(look, (data1))));
							float collisionLength = (*i)->GetCollisionSize() + (data2);
							if (distance <= collisionLength)
							{
								//std::cout << "col\n";
								(*i)->ReceiveDamage(damage);
							}
						}
					}
				}
			}

			break;
		}
		case CollisionType::SECTERFORM:
		{
			for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
			{
				if ((*i) != pCol) {
					TeamType a = (*i)->GetTeam();
					TeamType b = pCol->GetTeam();
					if (a != b) {
						if (NearLevel((*i)->GetCollisionLevel(), pCol->GetCollisionLevel()))
						{
							XMFLOAT3 apos = (*i)->GetPosition();
							XMFLOAT3 bpos = pCol->GetPosition();

							float distance = Vector3::Distance(apos, bpos);
							float collisionLength = (data1)+(*i)->GetCollisionSize();

							if (distance <= collisionLength)
							{
								XMFLOAT3 result = Vector3::Add(apos, bpos, -1.0f);

								float angle = Vector3::DotProduct(pCol->GetLook(), Vector3::Normalize(result));

								angle = acos(angle);
								angle = angle * 360 / PI;

								// 시야 각 안에 들어오는가
								if (angle <= data2)
								{
									//std::cout << "col\n";
									(*i)->ReceiveDamage(damage);
								}
							}
						}
					}
				}
			}
			break;
		}
		default:
			break;
		}
	}
}

CCollisionObject* CCollisionManager::RequestNearObject(CCollisionObject * pCol, float lengh)
{
	if (m_Winner == TeamType::None)
	{
		CCollisionObject* nearObject{ NULL };
		float nearDistance = 0;
		for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
		{

			if ((*i) != pCol && (*i)->GetTeam() != pCol->GetTeam()) {
				if (NearLevel((*i)->GetCollisionLevel(), pCol->GetCollisionLevel()))
				{
					XMFLOAT2 apos = XMFLOAT2((*i)->GetPosition().x, (*i)->GetPosition().z);
					XMFLOAT2 bpos = XMFLOAT2(pCol->GetPosition().x, pCol->GetPosition().z);
					float distance = Vector2::Distance(apos, bpos);
					if (distance <= lengh)
					{
						if (!nearObject) {
							nearDistance = distance;
							nearObject = (*i);
						}
						else if (nearDistance > distance) {
							nearDistance = distance;
							nearObject = (*i);
							//std::cout << "col\n";
						}
					}
				}
			}
		}
		return nearObject;
	}
	return NULL;
}

/*
	1 0  7
	2 ㅁ 6
	3 4  5
*/
void CCollisionManager::SearchSight(XMFLOAT2 startpos, int dir, int length, int slength)
{
	XMFLOAT2 result;
	XMFLOAT2 direction;
	XMFLOAT2 next;
	m_nodeMap[(int)startpos.x][(int)startpos.y].Detected = true;
	switch (dir) {
	case 0:
		direction = XMFLOAT2(0, -1);
		next = XMFLOAT2(1, 0);
		break;
	case 1:
		direction = XMFLOAT2(-1, -1);
		next = XMFLOAT2(1, 0);
		break;
	case 2:
		direction = XMFLOAT2(-1, 0);
		next = XMFLOAT2(0, -1);
		break;
	case 3:
		direction = XMFLOAT2(-1, 1);
		next = XMFLOAT2(0, -1);
		break;
	case 4:
		direction = XMFLOAT2(0, 1);
		next = XMFLOAT2(-1, 0);
		break;
	case 5:
		direction = XMFLOAT2(1, 1);
		next = XMFLOAT2(-1, 0);
		break;
	case 6:
		direction = XMFLOAT2(1, 0);
		next = XMFLOAT2(0, 1);
		break;
	case 7:
		direction = XMFLOAT2(1, -1);
		next = XMFLOAT2(0, 1);
		break;
	default:
		direction = XMFLOAT2(0, 0);
		next = XMFLOAT2(0, 0);
		break;
	}
	for (int i = 0; i < slength; ++i) {
		XMFLOAT2 nNext = Vector2::Normalize(XMFLOAT2(direction.x*slength + (next.x * i), direction.y*slength + (next.y * i)));
		for (int j = 1; j < slength; ++j) {
			result = startpos;
			if (next.x == 0) {
				result.x += j* direction.x;
				result.y += roundf((j* direction.x*nNext.y / nNext.x));
			}
			else if (next.y == 0) {
				result.y += j* direction.y;
				result.x += roundf((j* direction.y*nNext.x / nNext.y));
			}

			if (result.x < 0 || result.y < 0 || result.x >= nodeWH.x || result.y >= nodeWH.y)
				break;
			float dst = Vector2::Distance(startpos, result);
			if (dst <= slength) {
				m_nodeMap[(int)result.x][(int)result.y].Detected = true;
				if (m_nodeMap[(int)result.x][(int)result.y].Static == true) {
					break;
				}
			}
			else break;
		}
	}

}


CCollisionManager::~CCollisionManager()
{
}
