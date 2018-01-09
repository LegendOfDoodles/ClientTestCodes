#pragma once

////////////////////////////////////////////////////////////////////////
// �⺻ ����
class CVertex
{
protected:
	//������ ��ġ �����̴�(��� ������ �ּ��� ��ġ ���͸� ������ �Ѵ�).
	XMFLOAT3 m_position;
public:
	CVertex() { m_position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(XMFLOAT3 xmf3Position) { m_position = xmf3Position; }
	~CVertex() { }
};

////////////////////////////////////////////////////////////////////////
// ���� ����
class CDiffusedVertex : public CVertex
{
protected:
	//������ �����̴�.
	XMFLOAT4 m_diffuseColor;
public:
	CDiffusedVertex() {
		m_position = XMFLOAT3(0.0f, 0.0f, 0.0f); 
		m_diffuseColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse) {
		m_position = XMFLOAT3(x, y, z); 
		m_diffuseColor = xmf4Diffuse;
	}
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse) {
		m_position = xmf3Position; 
		m_diffuseColor = xmf4Diffuse;
	}
	~CDiffusedVertex() { }
};