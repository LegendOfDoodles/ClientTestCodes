#pragma once

#define INVALID_NODE -1

class CNode
{
public:
public: // 생성자, 소멸자
	CNode() {}
	CNode(int index, XMFLOAT2 &position, XMFLOAT2 &size);
	~CNode();

	const POINT GetDividIndex() const;

	int GetDistanceSquareWithPosition(const XMFLOAT2 &position);

	int Index() { return m_index; }
	XMFLOAT2 Position() { return m_position; }
	XMFLOAT2 Position() const { return m_position; }

	int PositionToIndex(XMFLOAT2 position);

	int m_index{ INVALID_NODE };	// 노드 식별 번호
	XMFLOAT2 m_position{ 0, 0 };	// 노드의 월드 공간 상 위치
	XMFLOAT2 m_size{ 0, 0 };	// 노드 한 칸의 크기
};