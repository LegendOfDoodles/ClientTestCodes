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

protected: // 내부 함수
	void CalculateTriangleListVertexTangents(XMFLOAT3 *pxmf3Tangents, XMFLOAT3 *pxmf3Positions, UINT nVertices, XMFLOAT2 *xmf2TexCoord, UINT *pnIndices = NULL, UINT nIndices = NULL);
	void CalculateTriangleStripVertexTangents(XMFLOAT3 *pxmf3Tangents, XMFLOAT3 *pxmf3Positions, UINT nVertices, XMFLOAT2 *xmf2TexCoord, UINT *pnIndices = NULL, UINT nIndices = NULL);

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMeshIlluminated : public CMesh
{
public:
	CMeshIlluminated(CCreateMgr *pCreateMgr);
	virtual ~CMeshIlluminated();

public:
	void CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices);
	void CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices);
	void CalculateTriangleStripVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices);
	void CalculateVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices, UINT *pnIndices, int nIndices);
};

class CCubeMeshIlluminated : public CMeshIlluminated
{
public:
	CCubeMeshIlluminated(CCreateMgr *pCreateMgr, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshIlluminated();
};


class CFBXMesh :public CMeshIlluminated
{
public :
	CFBXMesh(CCreateMgr *pCreateMgr);
	virtual ~CFBXMesh();
};

class CSphereMeshIlluminated : public CMeshIlluminated
{
public:
	CSphereMeshIlluminated(CCreateMgr *pCreateMgr, float fRadius = 2.0f, UINT nSlices = 20, UINT nStacks = 20);
	virtual ~CSphereMeshIlluminated();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMeshIlluminatedTextured : public CMeshIlluminated
{
public:
	CMeshIlluminatedTextured(CCreateMgr *pCreateMgr);
	CMeshIlluminatedTextured(CCreateMgr *pCreateMgr, UINT nVertices, XMFLOAT3 *pxmf3Positions, XMFLOAT3 *pxmf3Normals, XMFLOAT2 *pxmf2UVs, UINT nIndices, UINT *pnIndices);
	virtual ~CMeshIlluminatedTextured();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CCubeMeshIlluminatedTextured : public CMeshIlluminatedTextured
{
public:
	CCubeMeshIlluminatedTextured(CCreateMgr *pCreateMgr, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshIlluminatedTextured();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CCubeWithIndices : public CMeshIlluminatedTextured
{
public:
	CCubeWithIndices(CCreateMgr *pCreateMgr, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeWithIndices();
};