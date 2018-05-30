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
	m_nVertices = 4;
	m_nStride = sizeof(CDiffusedVertex);
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;

	CDiffusedVertex pVertices[4];

	float fx = fWidth + fxPosition, fy = fHeight + fzPosition;

	pVertices[0] = CDiffusedVertex(XMFLOAT3(fx, fy, 0), XMFLOAT4(1, 1, 1, 1));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(0, fy, 0), XMFLOAT4(1, 0, 0, 1));
	pVertices[2] = CDiffusedVertex(XMFLOAT3(fx, 0, 0), XMFLOAT4(0, 1, 0, 1));
	pVertices[3] = CDiffusedVertex(XMFLOAT3(0, 0, 0), XMFLOAT4(0, 0, 1, 1));

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// CHeightMapGridMesh
////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CHeightMapGridMesh::CHeightMapGridMesh(ID3D12Device *pd3dDevice, ID3D12GraphicsCommandList*pd3dCommandList, int xStart, int zStart, int nWidth, int
	nLength, XMFLOAT4 xmf4Color) : CMesh(pd3dDevice, pd3dCommandList)
{
	XMFLOAT3 xmf3Scale = TERRAIN_IMAGE_CELL_SCALE;
	//격자의 교점(정점)의 개수는 (nWidth * nLength)이다.
	m_nVertices = nWidth * nLength;
	m_nStride = sizeof(CDiffusedVertex);

	//격자는 삼각형 스트립으로 구성한다.
	m_d3dPrimitiveTopology = D3D_PRIMITIVE_TOPOLOGY_4_CONTROL_POINT_PATCHLIST;

	m_nIndices = (nWidth * 2) * (nLength - 1) * 3;
	CDiffusedVertex *pVertices = new CDiffusedVertex[m_nVertices];

	for (int i = 0, z = zStart; z < (zStart + nLength); z++)
	{
		for (int x = xStart; x < (xStart + nWidth); x++, i++)
		{
			pVertices[i] = CDiffusedVertex(XMFLOAT3((x * xmf3Scale.x), -153, (z * xmf3Scale.z)), RANDOM_COLOR);
		}
	}

	m_pd3dVertexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList, pVertices,
		m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pd3dVertexUploadBuffer);

	m_d3dVertexBufferView.BufferLocation = m_pd3dVertexBuffer->GetGPUVirtualAddress();
	m_d3dVertexBufferView.StrideInBytes = m_nStride;
	m_d3dVertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	delete[] pVertices;

	UINT *pnIndices = new UINT[m_nIndices];
	for (int j = 0, z = 0; z < nLength - 1; z++)
	{
		if ((z % 2) == 0)
		{
			//홀수 번째 줄이므로(z = 0, 2, 4, ...) 인덱스의 나열 순서는 왼쪽에서 오른쪽 방향이다.
			for (int x = 0; x < nWidth; x++)
			{
				//위(x, z+1), 아래(x, z)의 순서로 인덱스를 추가한다.
				if ((x != 0 || z != 0) && (x != nWidth - 1 || z != nLength - 2))
				{
					pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
					pnIndices[j++] = (UINT)(x + (z * nWidth));
				}
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
				pnIndices[j++] = (UINT)(x + (z * nWidth));
			}
		}
		else
		{
			//짝수 번째 줄이므로(z = 1, 3, 5, ...) 인덱스의 나열 순서는 오른쪽에서 왼쪽 방향이다.
			for (int x = nWidth - 1; x >= 0; x--)
			{
				if (x != 0 || z != nLength - 2)
				{
					pnIndices[j++] = (UINT)(x + (z * nWidth));
					pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
				}
				pnIndices[j++] = (UINT)(x + (z * nWidth));
				pnIndices[j++] = (UINT)((x + (z * nWidth)) + nWidth);
			}
		}
	}

	m_pd3dIndexBuffer = ::CreateBufferResource(pd3dDevice, pd3dCommandList,
		pnIndices,
		sizeof(UINT) * m_nIndices,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pd3dVertexUploadBuffer);

	m_d3dIndexBufferView.BufferLocation = m_pd3dIndexBuffer->GetGPUVirtualAddress();
	m_d3dIndexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_d3dIndexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	delete[] pnIndices;
}

CHeightMapGridMesh::~CHeightMapGridMesh()
{
}