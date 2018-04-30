#include "stdafx.h"
#include "WayFinder.h"
#include "00.Global/01.Utility/04.WayFinder/02.Astar/Astar.h"

/// <summary>
/// 목적: 길찾기 알고리즘을 위한 클래스 작성
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-04-20
/// </summary>


////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CWayFinder::CWayFinder(float sizeX, float sizeY)
	: m_size(sizeX, sizeY)
{
	m_pCollisionMapImage = new CCollisionMapImage(_T("Resource/Terrain/TerrainCollision.raw"),
		TERRAIN_IMAGE_WIDTH, TERRAIN_IMAGE_HEIGHT, TERRAIN_IMAGE_SCALE);

	m_nodeMax.x = (TERRAIN_SIZE_WIDTH / sizeX + 1);
	m_nodeMax.y = (TERRAIN_SIZE_HEIGHT / sizeY + 1);

	m_nodes.reserve(m_nodeMax.x * m_nodeMax.y);
	m_edges.resize(m_nodeMax.x * m_nodeMax.y);

	// 노드 추가
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

	// 엣지 추가
	for (int y = 0, i = 0, edgeIdx = 0; y < m_nodeMax.y; ++y)
	{
		for (int x = 0; x < m_nodeMax.x; ++x, ++i)
		{
			int from = m_nodes[i].Index();
			if (from != INVALID_NODE)
			{
				int to;
				bool LRDU[]{ false, false, false, false };
				// 좌
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
				// 우
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
				// 하
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
				//  상
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
				// 좌 하단 대각선
				if (LRDU[0] && LRDU[2])
				{
					to = m_nodes[m_nodeMax.x * (y - 1) + x - 1].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].push_back(CEdge(from, to));
						m_edges[from].push_back(CEdge(to, from));
					}
				}
				// 좌 상단 대각선
				if (LRDU[0] && LRDU[3])
				{
					to = m_nodes[m_nodeMax.x * (y + 1) + x - 1].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].push_back(CEdge(from, to));
						m_edges[from].push_back(CEdge(to, from));
					}
				}
				// 우 하단 대각선
				if (LRDU[1] && LRDU[2])
				{
					to = m_nodes[m_nodeMax.x * (y - 1) + x + 1].Index();
					if (to != INVALID_NODE)
					{
						m_edges[from].push_back(CEdge(from, to));
						m_edges[from].push_back(CEdge(to, from));
					}
				}
				// 우 상단 대각선
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
		if (srcIndex == INVALID_NODE) return nullptr;
		int dstIndex = FindClosestNodeIndexWithPosition(target);
		if (dstIndex == INVALID_NODE) return nullptr;

		m_pCurSearch = new CAstar(this, srcIndex, dstIndex);
		for (int i = 0; i < 10000; ++i)
		{
			int result = m_pCurSearch->FindPath();
			if (result == Found || result == Not_Found)
				break;
		}
		path = m_pCurSearch->GetPath();
		if(path->empty())
			path->push_back(CPathEdge(source, target));
		else
		{
			path->push_back(CPathEdge(path->back().To(), target));
		}
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
// 공개 함수

////////////////////////////////////////////////////////////////////////
// 내부 함수
