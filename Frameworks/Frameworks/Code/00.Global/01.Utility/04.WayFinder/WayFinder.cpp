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

// 직선 이동 가능한지 파악하기 위한 함수
bool CWayFinder::CanGoDirectly(XMFLOAT2 & source, XMFLOAT2 & target, float boundingRadius)
{
	// 해당 방향으로 조금씩 이동하면서 주변에 충돌하는 경우가 발생하는지 파악하여 충돌이 없으면 진행 가능으로 판단한다.
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

// 충돌 지점에서 충돌이 없는 가장 가까운 지점을 찾는 함수
XMFLOAT2 CWayFinder::GetClosestNotCollidePos(XMFLOAT2 & source, XMFLOAT2 & target, float boundingRadius)
{
	// 해당 방향으로 조금씩 이동하면서 주변에 충돌하는 경우가 발생하는지 파악하여 충돌이 없으면 진행 가능으로 판단한다.
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

	// 시작지와 끝 지점에서 가장 가까운 노드 검색
	int srcIndex = FindClosestNodeIndexWithPosition(source);
	if (srcIndex == INVALID_NODE) return nullptr;
	int dstIndex = FindClosestNodeIndexWithPosition(target);
	if (dstIndex == INVALID_NODE) return nullptr;

	// 도착지가 충돌체 위인 경우 도착지를 충돌이 없는 가장 가까운 위치로 변경한다.
	if (m_pCollisionMapImage->GetCollision(target.x, target.y))
		target = GetClosestNotCollidePos(target, m_nodes[dstIndex].Position(), boundingRadius);

	// 직선으로 이동 가능한 경우
	if (CanGoDirectly(source, target, boundingRadius))
	{
		// 목적지만 패스에 넣고 종료
		path = new Path;
		path->push_back(CPathEdge(source, target));

		return path;
	}
	else
	{
		// 길찾기 수행
		m_pCurSearch = new CAstar(this, srcIndex, dstIndex);
		for (int i = 0; i < 10000; ++i)
		{
			int result = m_pCurSearch->FindPath();
			if (result == Found || result == Not_Found)
				break;
		}
		// 찾은 패스 저장
		path = m_pCurSearch->GetPath();
		// 패스에 도착지를 추가로 연결하고 종료한다.
		path->push_back(CPathEdge(path->back().To(), target));

		delete m_pCurSearch;
	}

	// 직선 상으로 갈 수 있는 길 돌아가지 않도록 설정
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
// 공개 함수

////////////////////////////////////////////////////////////////////////
// 내부 함수
