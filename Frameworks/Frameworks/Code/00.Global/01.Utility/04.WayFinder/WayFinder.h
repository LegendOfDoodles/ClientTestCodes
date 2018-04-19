#pragma once
#include "00.Global/01.Utility/04.WayFinder/00.Node/Node.h"
#include "00.Global/01.Utility/04.WayFinder/01.Edge/Edge.h"

typedef std::vector<CNode> NodeVector;
typedef std::vector<CEdge> EdgeVector;
typedef std::vector<EdgeVector> EdgeArray;
typedef std::list<CPathEdge> Path;

class CAstar;

class CWayFinder
{
public: // 생성자, 소멸자
	CWayFinder(XMFLOAT2 size);
	~CWayFinder();

public: // 공개 함수
	bool CanGoDirectly(const XMFLOAT2 &source, const XMFLOAT2 &target);
	Path GetPathToPosition(const XMFLOAT2 &source, const XMFLOAT2 &target);

	int GetNodeCount() const { return m_nodes.size(); }
	const CNode& GetNodeAt(int idx) const { return m_nodes[idx]; }
	const EdgeVector& GetEdgesAt(int idx) const { return m_edges[idx]; }

protected: // 내부 함수

protected: // 변수
	NodeVector m_nodes;
	EdgeArray m_edges;

	XMFLOAT2 m_size{ 0, 0 };	// 노드 한 칸의 크기
	XMFLOAT2 m_nodeMax{ 0, 0 };	// 노드 최대 개수(가로, 세로)

	CAstar* m_pCurSearch{ NULL };
};