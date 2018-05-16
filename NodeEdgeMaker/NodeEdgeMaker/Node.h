#pragma once

#define INVALID_NODE -1

class CNode
{
public:
public: // ������, �Ҹ���
	CNode() {}
	CNode(int index, XMFLOAT2 &position, XMFLOAT2 &size);
	~CNode();

	const POINT GetDividIndex() const;

	int GetDistanceSquareWithPosition(const XMFLOAT2 &position);

	int Index() { return m_index; }
	XMFLOAT2 Position() { return m_position; }
	XMFLOAT2 Position() const { return m_position; }

	int PositionToIndex(XMFLOAT2 position);

	int m_index{ INVALID_NODE };	// ��� �ĺ� ��ȣ
	XMFLOAT2 m_position{ 0, 0 };	// ����� ���� ���� �� ��ġ
	XMFLOAT2 m_size{ 0, 0 };	// ��� �� ĭ�� ũ��
};