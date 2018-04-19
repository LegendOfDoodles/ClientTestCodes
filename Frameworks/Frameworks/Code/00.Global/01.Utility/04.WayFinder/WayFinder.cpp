#include "stdafx.h"
#include "WayFinder.h"
#include "00.Global/01.Utility/04.WayFinder/02.Astar/Astar.h"

/// <summary>
/// 목적: 길찾기 알고리즘을 위한 클래스 작성
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-04-19
/// </summary>


////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CWayFinder::CWayFinder(XMFLOAT2 size)
	: m_size(size)
{
	m_nodeMax.x = (TERRAIN_SIZE_WIDTH / size.x + 1);
	m_nodeMax.y = (TERRAIN_SIZE_HEIGHT / size.y + 1);
	m_nodes.reserve((TERRAIN_SIZE_WIDTH / size.x + 1) * (TERRAIN_SIZE_HEIGHT / size.y) + 1);
	m_edges.reserve(m_nodes.capacity());

	// 노드 추가
	for (int y = 0, index = 0; y <= m_nodeMax.y; ++y)
	{
		for (int x = 0; x <= m_nodeMax.x; ++x, ++index)
		{
			// Height Map 가져와서 못 가는 노드인지 아닌지 검사 할 필요 있음
			// 그 결과에 따라 index 대신 INVALID_NODE 추가
			// 그리고 그 경우 edge에 추가 안함
			m_nodes.emplace_back(index, XMFLOAT2(x * size.x, y * size.y), size);
			if (m_nodes.back().Index() != INVALID_NODE)
				m_edges.emplace_back(EdgeVector());
		}
	}

	// 엣지 추가
	for (int y = 0, i = 0, edgeIdx = 0; y <= m_nodeMax.y; ++y)
	{
		for (int x = 0; x <= m_nodeMax.x; ++x, ++i)
		{
			int from = m_nodes[i].Index();
			if (from != INVALID_NODE)
			{
				int to;
				bool LRDU[]{ false, false, false, false };
				// 좌
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
				// 우
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
				// 하
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
				//  상
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
				// 좌 하단 대각선
				if (LRDU[0] && LRDU[2])
				{
					to = m_nodes[m_nodeMax.x * (y - 1) + x - 1].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].emplace_back(from, to);
						m_edges[from].emplace_back(to, from);
					}
				}
				// 좌 상단 대각선
				if (LRDU[0] && LRDU[3])
				{
					to = m_nodes[m_nodeMax.x * (y + 1) + x - 1].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].emplace_back(from, to);
						m_edges[from].emplace_back(to, from);
					}
				}
				// 우 하단 대각선
				if (LRDU[1] && LRDU[2])
				{
					to = m_nodes[m_nodeMax.x * (y - 1) + x + 1].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].emplace_back(from, to);
						m_edges[from].emplace_back(to, from);
					}
				}
				// 우 상단 대각선
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
		// source , target 가까운 노드 찾는 함수 필요
		m_pCurSearch = new CAstar(this, 0, 0);
		m_pCurSearch->FindPath();
		path = m_pCurSearch->GetPath();
	}

	return path;
}

////////////////////////////////////////////////////////////////////////
// 공개 함수

////////////////////////////////////////////////////////////////////////
// 내부 함수
