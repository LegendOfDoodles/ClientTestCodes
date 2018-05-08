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
}

void CCollisionManager::RequestCollide(CollisionType type, CCollisionObject * pCol, float data1, float data2)
{
	switch (type)
	{
		//8:1..
	case CollisionType::SPHERE:
		for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
		{
			if ((*i) != pCol) {
				if (NearLevel((*i)->GetCollisionLevel(), pCol->GetCollisionLevel()))
				{
					XMFLOAT2 apos = XMFLOAT2((*i)->GetPosition().x, (*i)->GetPosition().z);
					XMFLOAT2 bpos = XMFLOAT2(pCol->GetPosition().x, pCol->GetPosition().z);
					XMFLOAT2 look = XMFLOAT2(pCol->GetLook().x, pCol->GetLook().z);
					float distance = Vector2::Distance(apos, Vector2::Add(bpos, Vector2::ScalarProduct(look, CONVERT_PaperUnit_to_InG(data1))));
					float collisionLength = (*i)->GetCollisionSize() + CONVERT_PaperUnit_to_InG(data2);
					if (distance <= collisionLength)
					{
						//std::cout << "col\n";
						(*i)->Rotate(new XMFLOAT3(1, 0, 0), 10);
						//(*i)->Translate(&Vector3::ScalarProduct(pCol->GetLook(),10));
					}
				}
			}
		}

		break;
	case CollisionType::SECTERFORM:
		for (auto i = m_lstColliders.begin(); i != m_lstColliders.end(); ++i)
		{
			if ((*i) != pCol) {
				if (NearLevel((*i)->GetCollisionLevel(), pCol->GetCollisionLevel()))
				{
					XMFLOAT3 apos = (*i)->GetPosition();
					XMFLOAT3 bpos = pCol->GetPosition();
					
					float distance = Vector3::Distance(apos,bpos);
					float collisionLength = CONVERT_PaperUnit_to_InG(data1) - (*i)->GetCollisionSize();
					
					if (distance <= collisionLength)
					{
						XMFLOAT3 result = Vector3::Add(apos, bpos,-1.0f);

						float a = Vector3::DotProduct(pCol->GetLook(), Vector3::Normalize(result));

						a = acos(a);
						a = a * 360 / PI;

						// �þ� �� �ȿ� �����°�
						if (a <= data2)
							(*i)->Rotate(new XMFLOAT3(1, 0, 0), 10);
					}
				}
			}
		}

		break;
	default:
		break;
	}

}


CCollisionManager::~CCollisionManager()
{
}
