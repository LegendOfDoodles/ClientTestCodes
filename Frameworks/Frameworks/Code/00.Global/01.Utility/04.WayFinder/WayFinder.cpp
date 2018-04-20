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
	m_nodeMax.x = (TERRAIN_SIZE_WIDTH / sizeX + 1);
	m_nodeMax.y = (TERRAIN_SIZE_HEIGHT / sizeY + 1);
	m_nodes.reserve((m_nodeMax.x) * (m_nodeMax.y));
	// ���� ���� ���� ���� ��, ���� ���ΰ� 2�� �̻��� �Ǿ�� �Ѵ�.
	m_edges.reserve(24 + (m_nodeMax.x-2) * 20 + (m_nodeMax.y-2) * 20 + ((m_nodeMax.x - 2) * (m_nodeMax.y - 2)) * 16);

	// ��� �߰�
	for (int y = 0, index = 0; y <= m_nodeMax.y; ++y)
	{
		for (int x = 0; x <= m_nodeMax.x; ++x, ++index)
		{
			// Height Map �����ͼ� �� ���� ������� �ƴ��� �˻� �� �ʿ� ����
			// �� ����� ���� index ��� INVALID_NODE �߰�
			// �׸��� �� ��� edge�� �߰� ����
			m_nodes.emplace_back(index, XMFLOAT2(x * sizeX, y * sizeY), m_size);
			if (m_nodes.back().Index() != INVALID_NODE)
				m_edges.emplace_back(EdgeVector());
		}
	}

	// ���� �߰�
	for (int y = 0, i = 0, edgeIdx = 0; y <= m_nodeMax.y; ++y)
	{
		for (int x = 0; x <= m_nodeMax.x; ++x, ++i)
		{
			int from = m_nodes[i].Index();
			if (from != INVALID_NODE)
			{
				int to;
				bool LRDU[]{ false, false, false, false };
				// ��
				if (x != 0)
				{
					to = m_nodes[m_nodeMax.x * y + x - 1].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].emplace_back(from, to);
						m_edges[from].emplace_back(to, from);
						LRDU[0] = true;
					}
				}
				// ��
				if (x != m_nodeMax.x)
				{
					to = m_nodes[m_nodeMax.x * y + x + 1].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].emplace_back(from, to);
						m_edges[from].emplace_back(to, from);
						LRDU[1] = true;
					}
				}
				// ��
				if (y != 0)
				{
					to = m_nodes[m_nodeMax.x * (y - 1) + x].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].emplace_back(from, to);
						m_edges[from].emplace_back(to, from);
						LRDU[2] = true;
					}
				}
				//  ��
				if (y != m_nodeMax.y)
				{
					to = m_nodes[m_nodeMax.x * (y + 1) + x].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].emplace_back(from, to);
						m_edges[from].emplace_back(to, from);
						LRDU[3] = true;
					}
				}
				// �� �ϴ� �밢��
				if (LRDU[0] && LRDU[2])
				{
					to = m_nodes[m_nodeMax.x * (y - 1) + x - 1].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].emplace_back(from, to);
						m_edges[from].emplace_back(to, from);
					}
				}
				// �� ��� �밢��
				if (LRDU[0] && LRDU[3])
				{
					to = m_nodes[m_nodeMax.x * (y + 1) + x - 1].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].emplace_back(from, to);
						m_edges[from].emplace_back(to, from);
					}
				}
				// �� �ϴ� �밢��
				if (LRDU[1] && LRDU[2])
				{
					to = m_nodes[m_nodeMax.x * (y - 1) + x + 1].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].emplace_back(from, to);
						m_edges[from].emplace_back(to, from);
					}
				}
				// �� ��� �밢��
				if (LRDU[1] && LRDU[3])
				{
					to = m_nodes[m_nodeMax.x * (y + 1) + x + 1].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].emplace_back(from, to);
						m_edges[from].emplace_back(to, from);
					}
				}
			}
		}
	}
}

CWayFinder::~CWayFinder()
{
}

bool CWayFinder::CanGoDirectly(const XMFLOAT2 & source, const XMFLOAT2 & target)
{
	return false;
}

Path *CWayFinder::GetPathToPosition(const XMFLOAT2 &source, const XMFLOAT2 &target)
{
	Path *path;

	if (CanGoDirectly(source, target))
	{
		path = new Path;
		path->push_back(CPathEdge(source, target));
	}
	else
	{
		int srcIndex = FindClosestNodeIndexWithPosition(source);
		int dstIndex = FindClosestNodeIndexWithPosition(target);
		m_pCurSearch = new CAstar(this, srcIndex, dstIndex);
		m_pCurSearch->FindPath();
		path = m_pCurSearch->GetPath();
		path->push_back(CPathEdge(source, target));
		delete m_pCurSearch;
	}

	return path;
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
