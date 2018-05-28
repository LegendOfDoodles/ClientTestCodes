#include "stdafx.h"
#include "Mesh.h"


CMesh::CMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList *pd3dCommandList) {

}

CMesh::~CMesh()
{
	if (m_pd3dVertexBuffer) m_pd3dVertexBuffer->Release();
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();

	if (m_pd3dIndexBuffer) m_pd3dIndexBuffer->Release();
	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
}

void CMesh::ReleaseUploadBuffers()
{
	//정점 버퍼를 위한 업로드 버퍼를 소멸시킨다.
	if (m_pd3dVertexUploadBuffer) m_pd3dVertexUploadBuffer->Release();
	m_pd3dVertexUploadBuffer = NULL;

	if (m_pd3dIndexUploadBuffer) m_pd3dIndexUploadBuffer->Release();
	m_pd3dIndexUploadBuffer = NULL;
};

void CMesh::Render(ID3D12GraphicsCommandList *pd3dCommandList)
{
	pd3dCommandList->IASetPrimitiveTopology(m_d3dPrimitiveTopology);
	pd3dCommandList->IASetVertexBuffers(m_nSlot, 1, &m_d3dVertexBufferView);
	if (m_pd3dIndexBuffer)
	{
		pd3dCommandList->IASetIndexBuffer(&m_d3dIndexBufferView);
		pd3dCommandList->DrawIndexedInstanced(m_nIndices, 1, 0, 0, 0);
		//인덱스 버퍼가 있으면 인덱스 버퍼를 파이프라인(IA: 입력 조립기)에 연결하고 인덱스를 사용하여 렌더링한다.
	}
	else
	{
		pd3dCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
	}
}

CDiffusedRectMesh::CDiffusedRectMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList*pd3dCommandList, float fWidth, float fHeight, float fxPosition, float fzPosition) : CMesh(pd3dDevice, pd3dCommandList)
{
	m_nVertices =9;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;

	CDiffusedVertex pVertices[9];

	float fx = fWidth + fxPosition, fy = fHeight + fzPosition;

	pVertices[0] = CDiffusedVertex(XMFLOAT3(fx, fy, 0), XMFLOAT4(1, 1, 1, 1));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(0, fy, 0), XMFLOAT4(1, 1, 1, 1));
	pVertices[2] = CDiffusedVertex(XMFLOAT3(fx, 0, 0), XMFLOAT4(1, 1, 1, 1));
	pVertices[3] = CDiffusedVertex(XMFLOAT3(0, 0, 0), XMFLOAT4(1, 1, 1, 1));

	pVertices[4] = CDiffusedVertex(XMFLOAT3(fx, 0, 0), XMFLOAT4(1, 1, 1, 1));

	pVertices[5] = CDiffusedVertex(XMFLOAT3(fx, 0, 0), XMFLOAT4(1, 1, 1, 1));
	pVertices[6] = CDiffusedVertex(XMFLOAT3(2 * fx, 0, 0), XMFLOAT4(1, 1, 1, 1));
	pVertices[7] = CDiffusedVertex(XMFLOAT3(fx, fy, 0), XMFLOAT4(1, 1, 1, 1));
	pVertices[8] = CDiffusedVertex(XMFLOAT3(2 * fx, fy, 0), XMFLOAT4(1, 1, 1, 1));

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CDiffusedRectMesh::~CDiffusedRectMesh()
{
}