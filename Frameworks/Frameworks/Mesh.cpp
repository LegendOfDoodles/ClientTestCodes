#include "stdafx.h"
#include "Mesh.h"
#include "CreateMgr.h"
#include "Vertex.h"

/// <summary>
/// ����: �׽�Ʈ �� �޽� Ŭ���� ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-01-09
/// </summary>

////////////////////////////////////////////////////////////////////////
// �⺻ �޽�
////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CMesh::CMesh(CCreateMgr *pCreateMgr)
{
	m_pCommandList = pCreateMgr->GetCommandList();
}

CMesh::~CMesh()
{
	if (m_pVertexBuffer) m_pVertexBuffer->Release();
	if (m_pVertexUploadBuffer) m_pVertexUploadBuffer->Release();
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CMesh::ReleaseUploadBuffers()
{
	if (m_pVertexUploadBuffer) { m_pVertexUploadBuffer->Release(); }
	m_pVertexUploadBuffer = NULL;
};

void CMesh::Render()
{
	//�޽��� ������Ƽ�� ������ �����Ѵ�.
	m_pCommandList->IASetPrimitiveTopology(m_primitiveTopology);
	//�޽��� ���� ���� �並 �����Ѵ�.
	m_pCommandList->IASetVertexBuffers(m_nSlot, 1, &m_vertexBufferView);
	//�޽��� ���� ���� �並 �������Ѵ�(����������(�Է� ������)�� �۵��ϰ� �Ѵ�).
	m_pCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�


////////////////////////////////////////////////////////////////////////
// �ﰢ�� �޽�
////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CTriangleMesh::CTriangleMesh(CCreateMgr *pCreateMgr) : CMesh(pCreateMgr)
{
	//�ﰢ�� �޽��� �����Ѵ�.
	m_nVertices = 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CDiffusedVertex pVertices[3];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f,1.0f));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f,1.0f));
	pVertices[2] = CDiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Blue));

	//�ﰢ�� �޽��� ���ҽ�(���� ����)�� �����Ѵ�.
	m_pVertexBuffer = pCreateMgr->CreateBufferResource(
		pVertices,
		m_nStride * m_nVertices, 
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, 
		&m_pVertexUploadBuffer);

	//���� ���� �並 �����Ѵ�.
	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = m_nStride;
	m_vertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}