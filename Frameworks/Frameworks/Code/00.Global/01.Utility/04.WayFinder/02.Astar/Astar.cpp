#include "stdafx.h"
#include "Astar.h"

/// <summary>
/// ����: ��ã�� �� Astar Ŭ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-04-20
/// </summary>

////////////////////////////////////////////////////////////////////////
// IndexedPriorityQLow
////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
template<class KeyType>
IndexedPriorityQLow<KeyType>::IndexedPriorityQLow(std::vector<KeyType>& keys, int maxSize)
	:m_vecKeys(keys), m_maxSize(maxSize), m_size(0)
{
	m_Heap.assign(maxSize + 1, 0);
	m_invHeap.assign(maxSize + 1, 0);
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
template<class KeyType>
void IndexedPriorityQLow<KeyType>::insert(const int idx)
{
	assert(m_size + 1 <= m_maxSize);

	++m_size;

	m_Heap[m_size] = idx;

	m_invHeap[idx] = m_size;

	ReorderUpwards(m_size);
}

template<class KeyType>
int IndexedPriorityQLow<KeyType>::Pop()
{
	Swap(1, m_size);

	ReorderDownwards(1, m_size - 1);

	return m_Heap[m_size--];
}

template<class KeyType>
void IndexedPriorityQLow<KeyType>::ChangePriority(const int idx)
{
	ReorderUpwards(m_invHeap[idx]);
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
template<class KeyType>
void IndexedPriorityQLow<KeyType>::Swap(int a, int b)
{
	int temp = m_Heap[a]; m_Heap[a] = m_Heap[b]; m_Heap[b] = temp;

	m_invHeap[m_Heap[a]] = a; m_invHeap[m_Heap[b]] = b;
}

template<class KeyType>
void IndexedPriorityQLow<KeyType>::ReorderUpwards(int nd)
{
	while ((nd > 1) && (m_vecKeys[m_Heap[nd / 2]] > m_vecKeys[m_Heap[nd]]))
	{
		Swap(nd / 2, nd);

		nd /= 2;
	}
}

template<class KeyType>
void IndexedPriorityQLow<KeyType>::ReorderDownwards(int nd, int HeapSize)
{
	while (2 * nd <= HeapSize)
	{
		int child = 2 * nd;

		if ((child < HeapSize) && (m_vecKeys[m_Heap[child]] > m_vecKeys[m_Heap[child + 1]]))
		{
			++child;
		}

		if (m_vecKeys[m_Heap[nd]] > m_vecKeys[m_Heap[child]])
		{
			Swap(child, nd);
			nd = child;
		}
		else
		{
			break;
		}
	}
}

////////////////////////////////////////////////////////////////////////
// Astar
////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CAstar::CAstar(const CWayFinder *pFinder, int source, int target)
	: m_pFinder(pFinder), m_source(source), m_target(target),
	m_GCosts(pFinder->GetNodeCount(), 0.0), m_FCosts(pFinder->GetNodeCount(), 0.0),
	m_shortestPathTree(pFinder->GetNodeCount()), m_searchFrontier(pFinder->GetNodeCount())
{
	m_pPQ = new IndexedPriorityQLow<double>(m_FCosts, pFinder->GetNodeCount());
	m_pPQ->insert(m_source);
}

CAstar::~CAstar()
{
	if (m_pPQ) delete m_pPQ;
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
int CAstar::FindPath()
{
	int NextClosestNode;

	// �� ã�� ���
	if (m_pPQ->empty()) return Not_Found;

	NextClosestNode = m_pPQ->Pop();
	m_shortestPathTree[NextClosestNode] = m_searchFrontier[NextClosestNode];

	// ��ǥ�� ������ ���
	if (NextClosestNode == m_target) return Found;

	const EdgeVector &edges = m_pFinder->GetEdgesAt(NextClosestNode);
	for (int i = 0; i < edges.size(); ++i)
	{
		// �޸���ƽ ���Ѵ�.           
		double HCost = Heuristic_Menhattan(m_target, edges[i].To());
		// �ܰ� �ڽ�Ʈ ���Ѵ�.
		double GCost = m_GCosts[NextClosestNode] + edges[i].Cost();

		// �ڽ�Ʈ�� ���Ѵ�.
		if (m_searchFrontier[edges[i].To()] == NULL)
		{
			m_FCosts[edges[i].To()] = GCost + HCost;
			m_GCosts[edges[i].To()] = GCost;

			m_pPQ->insert(edges[i].To());

			m_searchFrontier[edges[i].To()] = &edges[i];
		}

		// �̹� ������ �浵 �� ȿ������ ���̸� �������� ����.
		else if ((GCost < m_GCosts[edges[i].To()]) && (m_shortestPathTree[edges[i].To()] == NULL))
		{
			m_FCosts[edges[i].To()] = GCost + HCost;
			m_GCosts[edges[i].To()] = GCost;

			m_pPQ->ChangePriority(edges[i].To());

			m_searchFrontier[edges[i].To()] = &edges[i];
		}
	}

	return Processing;
}

Path* CAstar::GetPath()
{
	Path *path = new Path;

	if (m_target < 0)  return path;

	int nd = m_target;

	while ((nd != m_source) && (m_shortestPathTree[nd] != 0))
	{
		path->push_front(CPathEdge(m_pFinder->GetNodeAt(m_shortestPathTree[nd]->From()).Position(),
			m_pFinder->GetNodeAt(m_shortestPathTree[nd]->To()).Position()));

		nd = m_shortestPathTree[nd]->From();
	}

	return path;
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
double CAstar::Heuristic_Menhattan(int from, int to)
{
	const POINT& fromDividIndex = m_pFinder->GetNodeAt(from).GetDividIndex();
	const POINT& toDividIndex = m_pFinder->GetNodeAt(to).GetDividIndex();
	return ((fromDividIndex.x > toDividIndex.x ? fromDividIndex.x - toDividIndex.x : toDividIndex.x - fromDividIndex.x) +
		(fromDividIndex.y > toDividIndex.y ? fromDividIndex.y - toDividIndex.y : toDividIndex.y - fromDividIndex.y)) * 10.0f;
}