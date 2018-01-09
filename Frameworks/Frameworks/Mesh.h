#pragma once

class CCreateMgr;

////////////////////////////////////////////////////////////////////////
// 기본 메쉬
class CMesh
{
public: // 생성자, 소멸자
	CMesh(CCreateMgr *pCreateMgr);
	virtual ~CMesh();

public: // 공개 함수
	virtual void Render();

	void ReleaseUploadBuffers();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

private: // 내부 함수

protected: // 변수
	int m_nReferences = 0;

	ID3D12Resource *m_pVertexBuffer = NULL;
	ID3D12Resource *m_pVertexUploadBuffer = NULL;

	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	UINT m_nSlot = 0;
	UINT m_nVertices = 0;
	UINT m_nStride = 0;
	UINT m_nOffset = 0;

	ID3D12GraphicsCommandList *m_pCommandList;
};

////////////////////////////////////////////////////////////////////////
// 삼각형 메쉬
class CTriangleMesh : public CMesh
{
public: // 생성자, 소멸자
	CTriangleMesh(CCreateMgr *pCreateMgr);
	virtual ~CTriangleMesh() { }
};