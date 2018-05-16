#include "stdafx.h"

/// <summary>
/// 목적: Astar에 사용될 노드 및 에지를 콜리젼 맵에 맞춰 생성하고 파일로 만들기 위함
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-05-16
/// </summary>

int main()
{
	printf("Data: Data/TerrainCollision.raw 불러오기\n");
	CCollisionMapImage m_collisionMapImage(_T("Data/TerrainCollision.raw"),
		TERRAIN_IMAGE_WIDTH, TERRAIN_IMAGE_HEIGHT, TERRAIN_IMAGE_SCALE);
	printf("Data: Data/TerrainCollision.raw 불러오기 성공\n\n");

	int nodeMaxX = (TERRAIN_SIZE_WIDTH / NODE_SIZE + 1);
	int nodeMaxY = (TERRAIN_SIZE_HEIGHT / NODE_SIZE + 1);

	NodeVector nodes;
	EdgeArray edges(nodeMaxX * nodeMaxY);

	nodes.reserve(nodeMaxX * nodeMaxY);

	printf("노드 추가 시작\n");
	// 노드 추가
	for (int y = 0, index = 0; y <= nodeMaxY; ++y)
	{
		for (int x = 0; x <= nodeMaxX; ++x, ++index)
		{
			if (m_collisionMapImage.GetCollision(x * NODE_SIZE, y * NODE_SIZE))
				nodes.emplace_back(INVALID_NODE, XMFLOAT2(x * NODE_SIZE, y * NODE_SIZE), XMFLOAT2(NODE_SIZE, NODE_SIZE));
			else
				nodes.emplace_back(index, XMFLOAT2(x * NODE_SIZE, y * NODE_SIZE), XMFLOAT2(NODE_SIZE, NODE_SIZE));
		}
	}
	printf("노드 추가 완료\n\n");

	printf("에지 추가 시작\n");
	// 엣지 추가
	for (int y = 0, i = 0, edgeIdx = 0; y < nodeMaxY; ++y)
	{
		for (int x = 0; x < nodeMaxX; ++x, ++i)
		{
			int from = nodes[i].Index();
			if (from != INVALID_NODE)
			{
				int to;
				bool LRDU[]{ false, false, false, false };
				// 좌
				if (x != 0)
				{
					LRDU[0] = true;
					to = nodes[nodeMaxX * y + x - 1].Index();
					if (to != INVALID_NODE)
					{
						edges[from].push_back(CEdge(from, to));
						edges[from].push_back(CEdge(to, from));
					}
				}
				// 우
				if (x != nodeMaxX)
				{
					LRDU[1] = true;
					to = nodes[nodeMaxX * y + x + 1].Index();
					if (to != INVALID_NODE)
					{
						edges[from].push_back(CEdge(from, to));
						edges[from].push_back(CEdge(to, from));
					}
				}
				// 하
				if (y != 0)
				{
					LRDU[2] = true;
					to = nodes[nodeMaxX * (y - 1) + x].Index();
					if (to != INVALID_NODE)
					{
						edges[from].push_back(CEdge(from, to));
						edges[from].push_back(CEdge(to, from));
					}
				}
				//  상
				if (y != nodeMaxY)
				{
					LRDU[3] = true;
					to = nodes[nodeMaxX * (y + 1) + x].Index();
					if (to != INVALID_NODE)
					{
						edges[from].push_back(CEdge(from, to));
						edges[from].push_back(CEdge(to, from));
					}
				}
				// 좌 하단 대각선
				if (LRDU[0] && LRDU[2])
				{
					to = nodes[nodeMaxX * (y - 1) + x - 1].Index();
					if (to != INVALID_NODE)
					{
						edges[from].push_back(CEdge(from, to, 14));
						edges[from].push_back(CEdge(to, from, 14));
					}
				}
				// 좌 상단 대각선
				if (LRDU[0] && LRDU[3])
				{
					to = nodes[nodeMaxX * (y + 1) + x - 1].Index();
					if (to != INVALID_NODE)
					{
						edges[from].push_back(CEdge(from, to, 14));
						edges[from].push_back(CEdge(to, from, 14));
					}
				}
				// 우 하단 대각선
				if (LRDU[1] && LRDU[2])
				{
					to = nodes[nodeMaxX * (y - 1) + x + 1].Index();
					if (to != INVALID_NODE)
					{
						edges[from].push_back(CEdge(from, to, 14));
						edges[from].push_back(CEdge(to, from, 14));
					}
				}
				// 우 상단 대각선
				if (LRDU[1] && LRDU[3])
				{
					to = nodes[nodeMaxX * (y + 1) + x + 1].Index();
					if (to != INVALID_NODE)
					{
						edges[from].push_back(CEdge(from, to, 14));
						edges[from].push_back(CEdge(to, from, 14));
					}
				}
			}
		}
	}
	printf("에지 추가 완료\n\n");

	// 노드 출력
	printf("노드 출력 시작\n");
	std::ofstream nodeOut("Result/nodesData.txt", std::ios::out, std::ios::binary);

	nodeOut << nodes.size() << std::endl;
	for (int i = 0; i < nodes.size(); ++i)
	{
		nodeOut << nodes[i].m_index << ' ' 
			<< nodes[i].m_position.x << ' ' << nodes[i].m_position.y << ' '
			<< nodes[i].m_size.x << ' ' << nodes[i].m_size.y << std::endl;
	}

	nodeOut.close();
	printf("노드 출력 완료\n\n");

	// 에지 출력
	printf("에지 출력 시작\n");
	std::ofstream edgeOut("Result/edgesData.txt", std::ios::out, std::ios::binary);

	edgeOut << edges.size() << std::endl;
	for (int i = 0; i < edges.size(); ++i)
	{
		edgeOut << edges[i].size() << std::endl;
		for (auto &d : edges[i])
		{
			edgeOut << d.m_from << ' ' << d.m_to << ' ' << d.m_cost << std::endl;
		}
	}

	edgeOut.close();
	printf("에지 출력 완료\n\n");

	// 노드, 에지 제거
	nodes.clear();
	edges.clear();

	printf("노드 및 에지 파일 출력 완료\n");
}

