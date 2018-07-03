#pragma once
#include "00.Global/01.Utility/04.WayFinder/00.Node/Node.h"
#include "00.Global/01.Utility/04.WayFinder/01.Edge/Edge.h"
#include "06.Meshes/01.Mesh/Mesh.h"

typedef std::vector<CNode> NodeVector;
typedef std::vector<CEdge> EdgeVector;
typedef std::vector<EdgeVector> EdgeArray;
typedef std::list<CPathEdge> Path;

class CAstar;
class CCollisionObject;
struct NodeMap {
	bool Detected{ false };
	bool Static{ false };
};
class CWayFinder
{
public: // ������, �Ҹ���
	CWayFinder();
	~CWayFinder();

public: // ���� �Լ�
	bool CanGoDirectly(const XMFLOAT2 &source, const XMFLOAT2 &target, float collisionSize);
	bool CanGoDirectly(const XMFLOAT3 &source, const XMFLOAT3 &target, float collisionSize);
	XMFLOAT2 GetClosestNotCollidePos(const XMFLOAT2 &source, const XMFLOAT2 &target, float collisionSize);
	XMFLOAT3 GetClosestNotCollidePos(const XMFLOAT3 &source, const XMFLOAT3 &target, float collisionSize);
	Path *GetPathToPosition(const XMFLOAT2 &source, const XMFLOAT2 &target, float collisionSize);

	void SmoothPath(Path *path, float collisionSize);
	void SmoothPathDetail(Path *path, float collisionSize);

	int FindClosestNodeIndexWithPosition(const XMFLOAT2 &position);

	void AdjustValueByWallCollision(CCollisionObject* collider, const XMFLOAT3& dir, float val);

	int GetNodeCount() const { return static_cast<int>(m_nodes.size()); }
	const CNode& GetNodeAt(int idx) const { return m_nodes[idx]; }
	const EdgeVector& GetEdgesAt(int idx) const { return m_edges[idx]; }
	std::vector<NodeMap> GetNodeMap() { return m_nodeMap; }
	float GetNodeSize() { return nodeSize; }
	XMFLOAT2 GetNodeWH() { return m_nodeWH; }
protected: // ���� �Լ�

protected: // ����
	NodeVector m_nodes;
	EdgeArray m_edges;
	
	std::vector<NodeMap> m_nodeMap;
	float nodeSize{ 0 };
	XMFLOAT2 m_nodeWH;

	shared_ptr<CAstar> m_pCurSearch;
	CCollisionMapImage *m_pCollisionMapImage{ NULL };
};