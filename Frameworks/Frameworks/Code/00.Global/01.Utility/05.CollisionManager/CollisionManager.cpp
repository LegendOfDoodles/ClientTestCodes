#include "stdafx.h"
#include "CollisionManager.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

CCollisionManager::CCollisionManager()
{
}

void CCollisionManager::AddCollider(CCollisionObject* pcol)
{
	if (pcol) {
		m_lstColliders.push_back(pcol);

		m_nCollisers++;
	}

}


void CCollisionManager::Update(CWayFinder* pWayFinder)
{
	int cnt = 0;
	for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
	{
		for (auto& j = m_lstColliders.begin(); j != m_lstColliders.end(); ++j)
		{
			if (i != j) {
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
	//std::cout << cnt << "\n";
}


CCollisionManager::~CCollisionManager()
{
}
