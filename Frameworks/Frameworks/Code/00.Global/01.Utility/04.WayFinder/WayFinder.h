#pragma once
#include "00.Global/01.Utility/04.WayFinder/00.Node/Node.h"
#include "00.Global/01.Utility/04.WayFinder/01.Edge/Edge.h"
#include "06.Meshes/01.Mesh/Mesh.h"

typedef std::vector<CNode> NodeVector;
typedef std::vector<CEdge> EdgeVector;
typedef std::vector<EdgeVector> EdgeArray;
typedef std::list<CPathEdge> Path;

class CAstar;

class CWayFinder
{
public: // ������, �Ҹ���
	CWayFinder(float sizeX, float sizeY);
	~CWayFinder();

public: // ���� �Լ�
	bool CanGoDirectly(const XMFLOAT2 &source, const XMFLOAT2 &target);
	Path *GetPathToPosition(const XMFLOAT2 &source, const XMFLOAT2 &target);

	int FindClosestNodeIndexWithPosition(const XMFLOAT2 &position);

	int GetNodeCount() const { return m_nodes.size(); }
	const CNode& GetNodeAt(int idx) const { return m_nodes[idx]; }
	const EdgeVector& GetEdgesAt(int idx) const { return m_edges[idx]; }

protected: // ���� �Լ�

protected: // ����
	NodeVector m_nodes;
	EdgeArray m_edges;

	XMFLOAT2 m_size{ 0, 0 };	// ��� �� ĭ�� ũ��
	POINT m_nodeMax{ 0, 0 };	// ��� �ִ� ����(����, ����)

	CAstar* m_pCurSearch{ NULL };
	CCollisionMapImage *m_pCollisionMapImage{ NULL };
};