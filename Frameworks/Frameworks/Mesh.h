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
	virtual void Render();

	void ReleaseUploadBuffers();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

private: // ���� �Լ�

protected: // ����
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
// �ﰢ�� �޽�
class CTriangleMesh : public CMesh
{
public: // ������, �Ҹ���
	CTriangleMesh(CCreateMgr *pCreateMgr);
	virtual ~CTriangleMesh() { }
};