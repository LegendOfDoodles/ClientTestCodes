#include "stdafx.h"
#include "WayFinder.h"
#include "00.Global/01.Utility/04.WayFinder/02.Astar/Astar.h"

/// <summary>
/// ����: ��ã�� �˰����� ���� Ŭ���� �ۼ�
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-04-20
/// </summary>


////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CWayFinder::CWayFinder(float sizeX, float sizeY)
	: m_size(sizeX, sizeY)
{
	m_pCollisionMapImage = new CCollisionMapImage(_T("Resource/Terrain/TerrainCollision.raw"),
		TERRAIN_IMAGE_WIDTH, TERRAIN_IMAGE_HEIGHT, TERRAIN_IMAGE_SCALE);

	m_nodeMax.x = (TERRAIN_SIZE_WIDTH / sizeX + 1);
	m_nodeMax.y = (TERRAIN_SIZE_HEIGHT / sizeY + 1);

	m_nodes.reserve(m_nodeMax.x * m_nodeMax.y);
	m_edges.resize(m_nodeMax.x * m_nodeMax.y);

	// ��� �߰�
	for (int y = 0, index = 0; y <= m_nodeMax.y; ++y)
	{
		for (int x = 0; x <= m_nodeMax.x; ++x, ++index)
		{
			if(m_pCollisionMapImage->GetCollision(x * sizeX, y * sizeY))
				m_nodes.emplace_back(INVALID_NODE, XMFLOAT2(x * sizeX, y * sizeY), m_size);
			else
				m_nodes.emplace_back(index, XMFLOAT2(x * sizeX, y * sizeY), m_size);
		}
	}

	// ���� �߰�
	for (int y = 0, i = 0, edgeIdx = 0; y < m_nodeMax.y; ++y)
	{
		for (int x = 0; x < m_nodeMax.x; ++x, ++i)
		{
			int from = m_nodes[i].Index();
			if (from != INVALID_NODE)
			{
				int to;
				bool LRDU[]{ false, false, false, false };
				// ��
				if (x != 0)
				{
					LRDU[0] = true;
					to = m_nodes[m_nodeMax.x * y + x - 1].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].push_back(CEdge(from, to));
						m_edges[from].push_back(CEdge(to, from));
					}
				}
				// ��
				if (x != m_nodeMax.x)
				{
					LRDU[1] = true;
					to = m_nodes[m_nodeMax.x * y + x + 1].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].push_back(CEdge(from, to));
						m_edges[from].push_back(CEdge(to, from));
					}
				}
				// ��
				if (y != 0)
				{
					LRDU[2] = true;
					to = m_nodes[m_nodeMax.x * (y - 1) + x].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].push_back(CEdge(from, to));
						m_edges[from].push_back(CEdge(to, from));
					}
				}
				//  ��
				if (y != m_nodeMax.y)
				{
					LRDU[3] = true;
					to = m_nodes[m_nodeMax.x * (y + 1) + x].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].push_back(CEdge(from, to));
						m_edges[from].push_back(CEdge(to, from));
					}
				}
				// �� �ϴ� �밢��
				if (LRDU[0] && LRDU[2])
				{
					to = m_nodes[m_nodeMax.x * (y - 1) + x - 1].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].push_back(CEdge(from, to));
						m_edges[from].push_back(CEdge(to, from));
					}
				}
				// �� ��� �밢��
				if (LRDU[0] && LRDU[3])
				{
					to = m_nodes[m_nodeMax.x * (y + 1) + x - 1].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].push_back(CEdge(from, to));
						m_edges[from].push_back(CEdge(to, from));
					}
				}
				// �� �ϴ� �밢��
				if (LRDU[1] && LRDU[2])
				{
					to = m_nodes[m_nodeMax.x * (y - 1) + x + 1].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].push_back(CEdge(from, to));
						m_edges[from].push_back(CEdge(to, from));
					}
				}
				// �� ��� �밢��
				if (LRDU[1] && LRDU[3])
				{
					to = m_nodes[m_nodeMax.x * (y + 1) + x + 1].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].push_back(CEdge(from, to));
						m_edges[from].push_back(CEdge(to, from));
					}
				}
			}
		}
	}
}

CWayFinder::~CWayFinder()
{
	m_nodes.clear();
	for (int i = 0; i < m_edges.size(); ++i)
	{
		m_edges[i].clear();
	}
	m_edges.clear();
	if (m_pCollisionMapImage) Safe_Delete(m_pCollisionMapImage);
}

// ���� �̵� �������� �ľ��ϱ� ���� �Լ�
bool CWayFinder::CanGoDirectly(XMFLOAT2 & source, XMFLOAT2 & target, float boundingRadius)
{
	// �ش� �������� ���ݾ� �̵��ϸ鼭 �ֺ��� �浹�ϴ� ��찡 �߻��ϴ��� �ľ��Ͽ� �浹�� ������ ���� �������� �Ǵ��Ѵ�.
	XMFLOAT2 toTarget{ Vector2::Subtract(target, source, true) };
	XMFLOAT2 addVal{ Vector2::ScalarProduct(toTarget, boundingRadius * 0.5f) };

	for (XMFLOAT2 curPos = source; Vector2::DistanceSquare(curPos, target) > boundingRadius * boundingRadius; curPos = Vector2::Add(curPos, addVal))
	{
		if (m_pCollisionMapImage->GetCollision(curPos.x - boundingRadius, curPos.y)) return false;
		if (m_pCollisionMapImage->GetCollision(curPos.x, curPos.y + boundingRadius)) return false;
		if (m_pCollisionMapImage->GetCollision(curPos.x + boundingRadius, curPos.y)) return false;
		if (m_pCollisionMapImage->GetCollision(curPos.x, curPos.y - boundingRadius)) return false;
	}
	return true;
}

// �浹 �������� �浹�� ���� ���� ����� ������ ã�� �Լ�
XMFLOAT2 CWayFinder::GetClosestNotCollidePos(XMFLOAT2 & source, XMFLOAT2 & target, float boundingRadius)
{
	// �ش� �������� ���ݾ� �̵��ϸ鼭 �ֺ��� �浹�ϴ� ��찡 �߻��ϴ��� �ľ��Ͽ� �浹�� ������ ���� �������� �Ǵ��Ѵ�.
	XMFLOAT2 toTarget{ Vector2::Subtract(target, source, true) };
	XMFLOAT2 addVal{ Vector2::ScalarProduct(toTarget, boundingRadius * 0.5f) };

	for (XMFLOAT2 curPos = source; Vector2::DistanceSquare(curPos, target) > boundingRadius * boundingRadius; curPos = Vector2::Add(curPos, addVal))
	{
		if (!m_pCollisionMapImage->GetCollision(curPos.x - boundingRadius, curPos.y)) return curPos;
		if (!m_pCollisionMapImage->GetCollision(curPos.x, curPos.y + boundingRadius)) return curPos;
		if (!m_pCollisionMapImage->GetCollision(curPos.x + boundingRadius, curPos.y)) return curPos;
		if (!m_pCollisionMapImage->GetCollision(curPos.x, curPos.y - boundingRadius)) return curPos;
		if (!m_pCollisionMapImage->GetCollision(curPos.x - boundingRadius, curPos.y + boundingRadius)) return curPos;
		if (!m_pCollisionMapImage->GetCollision(curPos.x + boundingRadius, curPos.y + boundingRadius)) return curPos;
		if (!m_pCollisionMapImage->GetCollision(curPos.x - boundingRadius, curPos.y - boundingRadius)) return curPos;
		if (!m_pCollisionMapImage->GetCollision(curPos.x + boundingRadius, curPos.y - boundingRadius)) return curPos;
	}
	return source;
}

Path *CWayFinder::GetPathToPosition(XMFLOAT2 &source, XMFLOAT2 &target, float boundingRadius)
{
	Path *path;

	// �������� �� �������� ���� ����� ��� �˻�
	int srcIndex = FindClosestNodeIndexWithPosition(source);
	if (srcIndex == INVALID_NODE) return nullptr;
	int dstIndex = FindClosestNodeIndexWithPosition(target);
	if (dstIndex == INVALID_NODE) return nullptr;

	// �������� �浹ü ���� ��� �������� �浹�� ���� ���� ����� ��ġ�� �����Ѵ�.
	if (m_pCollisionMapImage->GetCollision(target.x, target.y))
		target = GetClosestNotCollidePos(target, m_nodes[dstIndex].Position(), boundingRadius);

	// �������� �̵� ������ ���
	if (CanGoDirectly(source, target, boundingRadius))
	{
		// �������� �н��� �ְ� ����
		path = new Path;
		path->push_back(CPathEdge(source, target));

		return path;
	}
	else
	{
		// ��ã�� ����
		m_pCurSearch = new CAstar(this, srcIndex, dstIndex);
		for (int i = 0; i < 10000; ++i)
		{
			int result = m_pCurSearch->FindPath();
			if (result == Found || result == Not_Found)
				break;
		}
		// ã�� �н� ����
		path = m_pCurSearch->GetPath();
		// �н��� �������� �߰��� �����ϰ� �����Ѵ�.
		path->push_back(CPathEdge(path->back().To(), target));

		delete m_pCurSearch;
	}

	// ���� ������ �� �� �ִ� �� ���ư��� �ʵ��� ����
	SmoothPath(path, boundingRadius);

	return path;
}

void CWayFinder::SmoothPath(Path *path, float boundingRadius)
{
	Path::iterator e1(path->begin()), e2(path->begin());

	++e2;

	while (e2 != path->end())
	{
		if (CanGoDirectly(e1->From(), e2->To(), boundingRadius))
		{
			e1->SetDestination(e2->To());
			e2 = path->erase(e2);
		}
		else
		{
			e1 = e2;
			++e2;
		}
	}
}

int CWayFinder::FindClosestNodeIndexWithPosition(const XMFLOAT2 & position)
{
	int closestIndex{ INVALID_NODE };
	int closestRange{ INT_MAX };
	int curRange{ INT_MAX };

	for (int i = 0; i < m_nodes.size(); ++i)
	{
		if (m_nodes[i].Index() == INVALID_NODE) continue;
		curRange = m_nodes[i].GetDistanceSquareWithPosition(position);
		if (curRange < closestRange)
		{
			closestRange = curRange;
			closestIndex = m_nodes[i].Index();
		}
	}

	return closestIndex;
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
