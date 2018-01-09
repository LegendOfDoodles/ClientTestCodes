#pragma once

////////////////////////////////////////////////////////////////////////
// 기본 정점
class CVertex
{
protected:
	//정점의 위치 벡터이다(모든 정점은 최소한 위치 벡터를 가져야 한다).
	XMFLOAT3 m_position;
public:
	CVertex() { m_position = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CVertex(XMFLOAT3 xmf3Position) { m_position = xmf3Position; }
	~CVertex() { }
};

////////////////////////////////////////////////////////////////////////
// 색상 정점
class CDiffusedVertex : public CVertex
{
protected:
	//정점의 색상이다.
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