#pragma once

class CCreateMgr;
class CSkinnedVertex;
////////////////////////////////////////////////////////////////////////
// �⺻ �޽�
class CMesh
{
public: // ������, �Ҹ���
	CMesh(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CMesh();

public: // ���� �Լ�
	virtual void Render(UINT istanceCnt = 1);

	virtual void ReleaseUploadBuffers();

	bool CheckRayIntersection(XMFLOAT3& rayPosition, XMFLOAT3& rayDirection, float &nearHitDistance);

	void SetBoundingBox(const XMFLOAT3& center, const XMFLOAT3& extents);
	
	ComPtr<ID3D12Resource> GetVertexBuffer() { return m_pVertexBuffer; }
	ComPtr<ID3D12Resource> GetVertexUploadBuffer() { 	return m_pVertexUploadBuffer; }

	BoundingOrientedBox GetBoundingBox() { return(*m_pBoundingBox); }

	bool HasBoundingBox() { return false; }

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected: // ���� �Լ�

protected: // ����
	int m_nReferences{ 0 };

	ComPtr<ID3D12Resource> m_pVertexBuffer;
	ComPtr<ID3D12Resource> m_pVertexUploadBuffer;

	ComPtr<ID3D12Resource> m_pIndexBuffer;
	ComPtr<ID3D12Resource> m_pIndexUploadBuffer;

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

	BoundingOrientedBox	 *m_pBoundingBox{ NULL };

	ComPtr<ID3D12GraphicsCommandList> m_pCommandList;
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMeshTextured : public CMesh
{
public: // ������, �Ҹ���
	CMeshTextured(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CMeshTextured();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMeshIlluminated : public CMesh
{
public: // ������, �Ҹ���
	CMeshIlluminated(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CMeshIlluminated();

public: // ���� �Լ�
	void CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices);
	void CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices);
	void CalculateTriangleStripVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices);
	void CalculateVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices, UINT *pnIndices, int nIndices);

	void CalculateTriangleListVertexTangents(XMFLOAT3 *pxmf3Tangents, XMFLOAT3 *pxmf3Positions, UINT nVertices, XMFLOAT2 *xmf2TexCoord, UINT *pnIndices = NULL, UINT nIndices = NULL);
	void CalculateTriangleStripVertexTangents(XMFLOAT3 *pxmf3Tangents, XMFLOAT3 *pxmf3Positions, UINT nVertices, XMFLOAT2 *xmf2TexCoord, UINT *pnIndices = NULL, UINT nIndices = NULL);
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMeshIlluminatedTextured : public CMeshIlluminated
{
public: // ������, �Ҹ���
	CMeshIlluminatedTextured(shared_ptr<CCreateMgr> pCreateMgr);
	CMeshIlluminatedTextured(shared_ptr<CCreateMgr> pCreateMgr, UINT nVertices, XMFLOAT3 *pxmf3Positions, XMFLOAT3 *pxmf3Normals, XMFLOAT2 *pxmf2UVs, UINT nIndices, UINT *pnIndices);
	virtual ~CMeshIlluminatedTextured();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CSkinnedMesh : public CMeshIlluminatedTextured
{
public: // ������, �Ҹ���
	CSkinnedMesh(shared_ptr<CCreateMgr> pCreateMgr, char* in);
	virtual ~CSkinnedMesh();
};
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

class CStaticMesh : public CMeshIlluminatedTextured
{
public: // ������, �Ҹ���
	CStaticMesh(shared_ptr<CCreateMgr> pCreateMgr, char* in, XMFLOAT3 scalevalue = XMFLOAT3(1,1,1));
	virtual ~CStaticMesh();
};
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CHeightMapImage
{
public: // ������, �Ҹ���
	CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	virtual ~CHeightMapImage(void);

public: // ���� �Լ�
	//���� �� �̹������� (x, z) ��ġ�� �ȼ� ���� ����� ������ ���̸� ��ȯ�Ѵ�.
	float GetHeight(float x, float z);
	//���� �� �̹������� (x, z) ��ġ�� ���� ���͸� ��ȯ�Ѵ�.
	XMFLOAT3 GetHeightMapNormal(int x, int z);

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }

	BYTE *GetHeightMapPixels() { return(m_pHeightMapPixels); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }

private: // ����
	//���� �� �̹��� �ȼ�(8-��Ʈ)���� ������ �迭�̴�. �� �ȼ��� 0~255�� ���� ���´�.
	BYTE * m_pHeightMapPixels{ NULL };
	//���� �� �̹����� ���ο� ���� ũ���̴�.
	int m_nWidth{ 0 };
	int m_nLength{ 0 };
	//���� �� �̹����� ������ �� �� Ȯ���Ͽ� ����� ���ΰ��� ��Ÿ���� ������ �����̴�.
	XMFLOAT3 m_xmf3Scale{};
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CHeightMapGridMesh : public CMesh
{
public: // ������, �Ҹ���
	CHeightMapGridMesh(shared_ptr<CCreateMgr> pCreateMgr, int nWidth, int nLength);
	virtual ~CHeightMapGridMesh();

public: // ���� �Լ�

protected: // ����

};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CTexturedRectMesh : public CMesh
{
public: // ������, �Ҹ���
	CTexturedRectMesh(shared_ptr<CCreateMgr> pCreateMgr, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 20.0f, float fxPosition = 0.0f, float fyPosition = 0.0f, float fzPosition = 0.0f);
	virtual ~CTexturedRectMesh();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CArrowMesh : public CMesh
{
public: // ������, �Ҹ���
	CArrowMesh(shared_ptr<CCreateMgr> pCreateMgr, float length = 100.0f);
	virtual ~CArrowMesh();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CCubeMesh : public CMesh
{
public: // ������, �Ҹ���
	//������ü�� ����, ����, ������ ���̸� �����Ͽ� ������ü �޽��� �����Ѵ�.
	CCubeMesh(shared_ptr<CCreateMgr> pCreateMgr, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f, float xOffset = 0.0f, float yOffSet = 0.0f, float zOffSet = 0.0f);
	virtual ~CCubeMesh();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CCollisionMapImage
{
public: // ������, �Ҹ���
	CCollisionMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	virtual ~CCollisionMapImage(void);

public: // ���� �Լ�
	bool GetCollision(float x, float z);

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }

	BYTE *GetCollisionMapPixels() { return(m_pCollisionMapPixels); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }

private: // ����
	BYTE * m_pCollisionMapPixels{ NULL };
	int m_nWidth{ 0 };
	int m_nLength{ 0 };

	XMFLOAT3 m_xmf3Scale{};
};