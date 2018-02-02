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
	virtual void Render(UINT istanceCnt = 1);

	void ReleaseUploadBuffers();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

private: // 내부 함수

protected: // 변수
	int m_nReferences{ 0 };

	ID3D12Resource *m_pVertexBuffer{ NULL };
	ID3D12Resource *m_pVertexUploadBuffer{ NULL };

	ID3D12Resource *m_pIndexBuffer{ NULL };
	ID3D12Resource *m_pIndexUploadBuffer{ NULL };

	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY m_primitiveTopology{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	UINT m_nVertices{ 0 };
	UINT m_nIndices{ 0 };

	UINT m_nSlot{ 0 };
	UINT m_nStride{ 0 };
	UINT m_nOffset{ 0 };

	UINT m_nStartIndex{ 0 };
	UINT m_nBaseVertex{ 0 };

	ID3D12GraphicsCommandList *m_pCommandList{ NULL };
};

////////////////////////////////////////////////////////////////////////
// 삼각형 메쉬
class CTriangleMesh : public CMesh
{
public: // 생성자, 소멸자
	CTriangleMesh(CCreateMgr *pCreateMgr);
	virtual ~CTriangleMesh() { }
};

////////////////////////////////////////////////////////////////////////
// 사각형 메쉬
class CCubeMeshDiffused : public CMesh
{
public: // 생성자, 소멸자
	//직육면체의 가로, 세로, 깊이의 길이를 지정하여 직육면체 메쉬를 생성한다.
	CCubeMeshDiffused(CCreateMgr *pCreateMgr, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshDiffused();
};

////////////////////////////////////////////////////////////////////////
// 비행기 메쉬
class CAirplaneMeshDiffused : public CMesh
{
public:	// 생성자, 소멸자
	CAirplaneMeshDiffused(CCreateMgr *pCreateMgr,
		float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f,
		XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CAirplaneMeshDiffused();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 텍스처 메쉬
class CMeshTextured : public CMesh
{
public:
	CMeshTextured(CCreateMgr *pCreateMgr);
	virtual ~CMeshTextured();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 텍스처 상자 메쉬
class CCubeMeshTextured : public CMeshTextured
{
public:
	CCubeMeshTextured(CCreateMgr *pCreateMgr, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshTextured();
};