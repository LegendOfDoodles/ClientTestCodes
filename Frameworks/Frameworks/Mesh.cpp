#include "stdafx.h"
#include "Mesh.h"
#include "CreateMgr.h"
#include "Vertex.h"

/// <summary>
/// 목적: 테스트 용 메쉬 클래스 생성
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-01-09
/// </summary>

////////////////////////////////////////////////////////////////////////
// 기본 메쉬
////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
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
// 공개 함수
void CMesh::ReleaseUploadBuffers()
{
	if (m_pVertexUploadBuffer) { m_pVertexUploadBuffer->Release(); }
	m_pVertexUploadBuffer = NULL;
};

void CMesh::Render()
{
	//메쉬의 프리미티브 유형을 설정한다.
	m_pCommandList->IASetPrimitiveTopology(m_primitiveTopology);
	//메쉬의 정점 버퍼 뷰를 설정한다.
	m_pCommandList->IASetVertexBuffers(m_nSlot, 1, &m_vertexBufferView);
	//메쉬의 정점 버퍼 뷰를 렌더링한다(파이프라인(입력 조립기)을 작동하게 한다).
	m_pCommandList->DrawInstanced(m_nVertices, 1, m_nOffset, 0);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수


////////////////////////////////////////////////////////////////////////
// 삼각형 메쉬
////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CTriangleMesh::CTriangleMesh(CCreateMgr *pCreateMgr) : CMesh(pCreateMgr)
{
	//삼각형 메쉬를 정의한다.
	m_nVertices = 3;
	m_nStride = sizeof(CDiffusedVertex);
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	CDiffusedVertex pVertices[3];
	pVertices[0] = CDiffusedVertex(XMFLOAT3(0.0f, 0.5f, 0.0f), XMFLOAT4(1.0f, 0.0f, 0.0f,1.0f));
	pVertices[1] = CDiffusedVertex(XMFLOAT3(0.5f, -0.5f, 0.0f), XMFLOAT4(0.0f, 1.0f, 0.0f,1.0f));
	pVertices[2] = CDiffusedVertex(XMFLOAT3(-0.5f, -0.5f, 0.0f), XMFLOAT4(Colors::Blue));

	//삼각형 메쉬를 리소스(정점 버퍼)로 생성한다.
	m_pVertexBuffer = pCreateMgr->CreateBufferResource(
		pVertices,
		m_nStride * m_nVertices, 
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, 
		&m_pVertexUploadBuffer);

	//정점 버퍼 뷰를 생성한다.
	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = m_nStride;
	m_vertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}