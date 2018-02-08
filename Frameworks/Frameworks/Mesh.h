#pragma once

class CCreateMgr;

////////////////////////////////////////////////////////////////////////
// �⺻ �޽�
class CMesh
{
public: // ������, �Ҹ���
	CMesh(CCreateMgr *pCreateMgr);
	virtual ~CMesh();

public: // ���� �Լ�
	virtual void Render(UINT istanceCnt = 1);

	void ReleaseUploadBuffers();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

private: // ���� �Լ�

protected: // ����
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
// �ﰢ�� �޽�
class CTriangleMesh : public CMesh
{
public: // ������, �Ҹ���
	CTriangleMesh(CCreateMgr *pCreateMgr);
	virtual ~CTriangleMesh() { }
};

////////////////////////////////////////////////////////////////////////
// �簢�� �޽�
class CCubeMeshDiffused : public CMesh
{
public: // ������, �Ҹ���
	//������ü�� ����, ����, ������ ���̸� �����Ͽ� ������ü �޽��� �����Ѵ�.
	CCubeMeshDiffused(CCreateMgr *pCreateMgr, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshDiffused();
};

////////////////////////////////////////////////////////////////////////
// ����� �޽�
class CAirplaneMeshDiffused : public CMesh
{
public:	// ������, �Ҹ���
	CAirplaneMeshDiffused(CCreateMgr *pCreateMgr,
		float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 4.0f,
		XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f));
	virtual ~CAirplaneMeshDiffused();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �ؽ�ó �޽�
class CMeshTextured : public CMesh
{
public:
	CMeshTextured(CCreateMgr *pCreateMgr);
	virtual ~CMeshTextured();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// �ؽ�ó ���� �޽�
class CCubeMeshTextured : public CMeshTextured
{
public:
	CCubeMeshTextured(CCreateMgr *pCreateMgr, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshTextured();
};