#include "stdafx.h"
#include "WayFinder.h"
#include "00.Global/01.Utility/04.WayFinder/02.Astar/Astar.h"

/// <summary>
/// ����: ��ã�� �˰����� ���� Ŭ���� �ۼ�
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-04-19
/// </summary>


////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CWayFinder::CWayFinder(XMFLOAT2 size)
	: m_size(size)
{
	m_nodeMax.x = (TERRAIN_SIZE_WIDTH / size.x + 1);
	m_nodeMax.y = (TERRAIN_SIZE_HEIGHT / size.y + 1);
	m_nodes.reserve((TERRAIN_SIZE_WIDTH / size.x + 1) * (TERRAIN_SIZE_HEIGHT / size.y) + 1);
	m_edges.reserve(m_nodes.capacity());

	// ��� �߰�
	for (int y = 0, index = 0; y <= m_nodeMax.y; ++y)
	{
		for (int x = 0; x <= m_nodeMax.x; ++x, ++index)
		{
			// Height Map �����ͼ� �� ���� ������� �ƴ��� �˻� �� �ʿ� ����
			// �� ����� ���� index ��� INVALID_NODE �߰�
			// �׸��� �� ��� edge�� �߰� ����
			m_nodes.emplace_back(index, XMFLOAT2(x * size.x, y * size.y), size);
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

Path CWayFinder::GetPathToPosition(const XMFLOAT2 &source, const XMFLOAT2 &target)
{
	Path path;

	if (CanGoDirectly(source, target))
	{
		path.emplace_back();
	}
	else
	{
		// source , target ����� ��� ã�� �Լ� �ʿ�
		m_pCurSearch = new CAstar(this, 0, 0);
		m_pCurSearch->FindPath();
		path = m_pCurSearch->GetPath();
	}

	return path;
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
