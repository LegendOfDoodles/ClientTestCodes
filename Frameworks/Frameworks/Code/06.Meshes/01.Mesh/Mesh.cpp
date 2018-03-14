#include "stdafx.h"
#include "Mesh.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "06.Meshes/00.Vertex/Vertex.h"
#include "MeshImporter.h"
#include "05.Objects/97.Skeleton/Skeleton.h"
/// <summary>
/// 목적: 테스트 용 메쉬 클래스 생성
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-03-10
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
	Safe_Release(m_pVertexBuffer);
	Safe_Release(m_pVertexUploadBuffer);

	Safe_Release(m_pIndexBuffer);
	Safe_Release(m_pIndexUploadBuffer);
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CMesh::ReleaseUploadBuffers()
{
	Safe_Release(m_pVertexUploadBuffer);
	Safe_Release(m_pIndexUploadBuffer);
};

void CMesh::Render(UINT istanceCnt)
{
	m_pCommandList->IASetPrimitiveTopology(m_primitiveTopology);
	m_pCommandList->IASetVertexBuffers(m_nSlot, 1, &m_vertexBufferView);
	if (m_pIndexBuffer)
	{
		m_pCommandList->IASetIndexBuffer(&m_indexBufferView);
		m_pCommandList->DrawIndexedInstanced(m_nIndices, istanceCnt, 0, 0, 0);
	}
	else
	{
		m_pCommandList->DrawInstanced(m_nVertices, istanceCnt, m_nOffset, 0);
	}
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CMesh::CalculateTriangleListVertexTangents(XMFLOAT3 *pxmf3Tangents, XMFLOAT3 *pxmf3Positions, UINT nVertices,
	XMFLOAT2 *xmf2TexCoord, UINT *pnIndices, UINT nIndices)
{
	int nPrimitives = (pnIndices) ? (nIndices / 3) : (nVertices / 3);
	XMFLOAT3 xmf3SumOfTangent(0.0f, 0.0f, 0.0f);
	UINT nIndex0, nIndex1, nIndex2;
	float deltaU0, deltaU1;
	float deltaV0, deltaV1;

	if (pnIndices)
	{
		for (UINT v = 0; v < nVertices; v++)
		{
			xmf3SumOfTangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
			for (UINT i = 0; i < nPrimitives; i++)
			{
				if (pnIndices[i * 3] == v)
				{
					nIndex0 = pnIndices[i * 3];		nIndex1 = pnIndices[i * 3 + 1];		nIndex2 = pnIndices[i * 3 + 2];
				}
				else if (pnIndices[i * 3 + 1] == v)
				{
					nIndex0 = pnIndices[i * 3 + 1];		nIndex1 = pnIndices[i * 3 + 2];		nIndex2 = pnIndices[i * 3];
				}
				else if (pnIndices[i * 3 + 2] == v)
				{
					nIndex0 = pnIndices[i * 3 + 2];		nIndex1 = pnIndices[i * 3];		nIndex2 = pnIndices[i * 3 + 1];
				}
				else { continue; }

				deltaU0 = xmf2TexCoord[nIndex1].x - xmf2TexCoord[nIndex0].x;
				deltaU1 = xmf2TexCoord[nIndex2].x - xmf2TexCoord[nIndex0].x;
				deltaV0 = xmf2TexCoord[nIndex1].y - xmf2TexCoord[nIndex0].y;
				deltaV1 = xmf2TexCoord[nIndex2].y - xmf2TexCoord[nIndex0].y;

				if (deltaU0 * deltaV1 == deltaU1 * deltaV0) continue;

				float proVal = 1.0f / abs(deltaU0 * deltaV1 - deltaU1 * deltaV0);

				XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Tangent(
					proVal * (deltaV1 * xmf3Edge01.x - deltaV0 * xmf3Edge02.x),
					proVal * (deltaV1 * xmf3Edge01.y - deltaV0 * xmf3Edge02.y),
					proVal * (deltaV1 * xmf3Edge01.z - deltaV0 * xmf3Edge02.z)
				);

				xmf3SumOfTangent = Vector3::Add(xmf3SumOfTangent, xmf3Tangent);
			}
			pxmf3Tangents[v] = Vector3::Normalize(xmf3SumOfTangent);
		}
	}
	else
	{
		for (int i = 0; i < nPrimitives; i++)
		{
			for (int j = 0, k = 1, l = 2; j < 3; ++j, ++k, ++l)
			{
				k = Wrap(k, 0, 3);
				l = Wrap(l, 0, 3);

				nIndex0 = i * 3 + j;		nIndex1 = i * 3 + k;		nIndex2 = i * 3 + l;

				deltaU0 = xmf2TexCoord[nIndex1].x - xmf2TexCoord[nIndex0].x;
				deltaU1 = xmf2TexCoord[nIndex2].x - xmf2TexCoord[nIndex0].x;
				deltaV0 = xmf2TexCoord[nIndex1].y - xmf2TexCoord[nIndex0].y;
				deltaV1 = xmf2TexCoord[nIndex2].y - xmf2TexCoord[nIndex0].y;

				if (deltaU0 * deltaV1 == deltaU1 * deltaV0) continue;

				float proVal = 1.0f / abs(deltaU0 * deltaV1 - deltaU1 * deltaV0);

				XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Tangent(
					proVal * (deltaV1 * xmf3Edge01.x - deltaV0 * xmf3Edge02.x),
					proVal * (deltaV1 * xmf3Edge01.y - deltaV0 * xmf3Edge02.y),
					proVal * (deltaV1 * xmf3Edge01.z - deltaV0 * xmf3Edge02.z)
				);

				pxmf3Tangents[nIndex0] = xmf3Tangent;
			}
		}
	}
}

void CMesh::CalculateTriangleStripVertexTangents(XMFLOAT3 *pxmf3Tangents, XMFLOAT3 *pxmf3Positions, UINT nVertices,
	XMFLOAT2 *xmf2TexCoord, UINT *pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices - 2) : (nVertices - 2);
	XMFLOAT3 xmf3SumOfTangent(0.0f, 0.0f, 0.0f);
	UINT nIndex0, nIndex1, nIndex2;
	float deltaU0, deltaU1;
	float deltaV0, deltaV1;

	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfTangent = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = ((i % 2) == 0) ? (i + 0) : (i + 1);
			if (pnIndices) nIndex0 = pnIndices[nIndex0];
			nIndex1 = ((i % 2) == 0) ? (i + 1) : (i + 0);
			if (pnIndices) nIndex1 = pnIndices[nIndex1];
			nIndex2 = (pnIndices) ? pnIndices[i + 2] : (i + 2);

			if (nIndex0 == j) {}
			else if (nIndex1 == j)
			{
				int temp = nIndex0;
				nIndex0 = nIndex1;
				nIndex1 = nIndex2;
				nIndex2 = temp;
			}
			else if (nIndex2 == j)
			{
				int temp = nIndex0;
				nIndex0 = nIndex2;
				nIndex2 = nIndex1;
				nIndex1 = temp;
			}
			else { continue; }

			deltaU0 = xmf2TexCoord[nIndex1].x - xmf2TexCoord[nIndex0].x;
			deltaU1 = xmf2TexCoord[nIndex2].x - xmf2TexCoord[nIndex0].x;
			deltaV0 = xmf2TexCoord[nIndex1].y - xmf2TexCoord[nIndex0].y;
			deltaV1 = xmf2TexCoord[nIndex2].y - xmf2TexCoord[nIndex0].y;

			if (deltaU0 * deltaV1 == deltaU1 * deltaV0) continue;

			float proVal = 1.0f / abs(deltaU0 * deltaV1 - deltaU1 * deltaV0);

			XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
			XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
			XMFLOAT3 xmf3Tangent(
				proVal * (deltaV1 * xmf3Edge01.x - deltaV0 * xmf3Edge02.x),
				proVal * (deltaV1 * xmf3Edge01.y - deltaV0 * xmf3Edge02.y),
				proVal * (deltaV1 * xmf3Edge01.z - deltaV0 * xmf3Edge02.z)
			);
			xmf3SumOfTangent = Vector3::Add(xmf3SumOfTangent, xmf3Tangent);
		}
		pxmf3Tangents[j] = Vector3::Normalize(xmf3SumOfTangent);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 텍스처 메쉬
CMeshTextured::CMeshTextured(CCreateMgr *pCreateMgr)
	: CMesh(pCreateMgr)
{
}

CMeshTextured::~CMeshTextured()
{
}

//////////////////////////////////////////////////////////////////////////////////
// 텍스처 상자 메쉬
CCubeMeshTextured::CCubeMeshTextured(CCreateMgr *pCreateMgr,
	float fWidth, float fHeight, float fDepth)
	: CMeshTextured(pCreateMgr)
{
	m_nVertices = 36;
	m_nStride = sizeof(CTexturedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	XMFLOAT3 pPositions[36];
	int i = 0;
	pPositions[i++] = XMFLOAT3(-fx, +fy, -fz);
	pPositions[i++] = XMFLOAT3(+fx, +fy, -fz);
	pPositions[i++] = XMFLOAT3(+fx, -fy, -fz);

	pPositions[i++] = XMFLOAT3(-fx, +fy, -fz);
	pPositions[i++] = XMFLOAT3(+fx, -fy, -fz);
	pPositions[i++] = XMFLOAT3(-fx, -fy, -fz);

	pPositions[i++] = XMFLOAT3(-fx, +fy, +fz);
	pPositions[i++] = XMFLOAT3(+fx, +fy, +fz);
	pPositions[i++] = XMFLOAT3(+fx, +fy, -fz);

	pPositions[i++] = XMFLOAT3(-fx, +fy, +fz);
	pPositions[i++] = XMFLOAT3(+fx, +fy, -fz);
	pPositions[i++] = XMFLOAT3(-fx, +fy, -fz);

	pPositions[i++] = XMFLOAT3(-fx, -fy, +fz);
	pPositions[i++] = XMFLOAT3(+fx, -fy, +fz);
	pPositions[i++] = XMFLOAT3(+fx, +fy, +fz);

	pPositions[i++] = XMFLOAT3(-fx, -fy, +fz);
	pPositions[i++] = XMFLOAT3(+fx, +fy, +fz);
	pPositions[i++] = XMFLOAT3(-fx, +fy, +fz);

	pPositions[i++] = XMFLOAT3(-fx, -fy, -fz);
	pPositions[i++] = XMFLOAT3(+fx, -fy, -fz);
	pPositions[i++] = XMFLOAT3(+fx, -fy, +fz);

	pPositions[i++] = XMFLOAT3(-fx, -fy, -fz);
	pPositions[i++] = XMFLOAT3(+fx, -fy, +fz);
	pPositions[i++] = XMFLOAT3(-fx, -fy, +fz);

	pPositions[i++] = XMFLOAT3(-fx, +fy, +fz);
	pPositions[i++] = XMFLOAT3(-fx, +fy, -fz);
	pPositions[i++] = XMFLOAT3(-fx, -fy, -fz);

	pPositions[i++] = XMFLOAT3(-fx, +fy, +fz);
	pPositions[i++] = XMFLOAT3(-fx, -fy, -fz);
	pPositions[i++] = XMFLOAT3(-fx, -fy, +fz);

	pPositions[i++] = XMFLOAT3(+fx, +fy, -fz);
	pPositions[i++] = XMFLOAT3(+fx, +fy, +fz);
	pPositions[i++] = XMFLOAT3(+fx, -fy, +fz);

	pPositions[i++] = XMFLOAT3(+fx, +fy, -fz);
	pPositions[i++] = XMFLOAT3(+fx, -fy, +fz);
	pPositions[i++] = XMFLOAT3(+fx, -fy, -fz);

	XMFLOAT2 pTexCoords[36];
	i = 0;
	pTexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pTexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
	pTexCoords[i++] = XMFLOAT2(1.0f, 1.0f);

	pTexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pTexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
	pTexCoords[i++] = XMFLOAT2(0.0f, 1.0f);

	pTexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pTexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
	pTexCoords[i++] = XMFLOAT2(1.0f, 1.0f);

	pTexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pTexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
	pTexCoords[i++] = XMFLOAT2(0.0f, 1.0f);

	pTexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pTexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
	pTexCoords[i++] = XMFLOAT2(1.0f, 1.0f);

	pTexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pTexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
	pTexCoords[i++] = XMFLOAT2(0.0f, 1.0f);

	pTexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pTexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
	pTexCoords[i++] = XMFLOAT2(1.0f, 1.0f);

	pTexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pTexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
	pTexCoords[i++] = XMFLOAT2(0.0f, 1.0f);

	pTexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pTexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
	pTexCoords[i++] = XMFLOAT2(1.0f, 1.0f);

	pTexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pTexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
	pTexCoords[i++] = XMFLOAT2(0.0f, 1.0f);

	pTexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pTexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
	pTexCoords[i++] = XMFLOAT2(1.0f, 1.0f);

	pTexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pTexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
	pTexCoords[i++] = XMFLOAT2(0.0f, 1.0f);

	CTexturedVertex pVertices[36];
	for (i = 0; i < 36; i++) pVertices[i] = CTexturedVertex(pPositions[i], pTexCoords[i]);

	m_pVertexBuffer = pCreateMgr->CreateBufferResource(
		pVertices,
		m_nStride * m_nVertices,
		D3D12_HEAP_TYPE_DEFAULT,
		D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER,
		&m_pVertexUploadBuffer);

	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = m_nStride;
	m_vertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CCubeMeshTextured::~CCubeMeshTextured()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMeshIlluminated::CMeshIlluminated(CCreateMgr *pCreateMgr) : CMesh(pCreateMgr)
{
}

CMeshIlluminated::~CMeshIlluminated()
{
}

void CMeshIlluminated::CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices)
{
	int nPrimitives = nVertices / 3;
	UINT nIndex0, nIndex1, nIndex2;
	for (int i = 0; i < nPrimitives; i++)
	{
		nIndex0 = i * 3 + 0;
		nIndex1 = i * 3 + 1;
		nIndex2 = i * 3 + 2;
		XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
		XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
		pxmf3Normals[nIndex0] = pxmf3Normals[nIndex1] = pxmf3Normals[nIndex2] = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
	}
}

void CMeshIlluminated::CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices / 3) : (nVertices / 3);
	XMFLOAT3 xmf3SumOfNormal, xmf3Edge01, xmf3Edge02, xmf3Normal;
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = pnIndices[i * 3 + 0];
			nIndex1 = pnIndices[i * 3 + 1];
			nIndex2 = pnIndices[i * 3 + 2];
			if (pnIndices && ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j)))
			{
				xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, false);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void CMeshIlluminated::CalculateTriangleStripVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices)
{
	UINT nPrimitives = (pnIndices) ? (nIndices - 2) : (nVertices - 2);
	XMFLOAT3 xmf3SumOfNormal(0.0f, 0.0f, 0.0f);
	UINT nIndex0, nIndex1, nIndex2;
	for (UINT j = 0; j < nVertices; j++)
	{
		xmf3SumOfNormal = XMFLOAT3(0.0f, 0.0f, 0.0f);
		for (UINT i = 0; i < nPrimitives; i++)
		{
			nIndex0 = ((i % 2) == 0) ? (i + 0) : (i + 1);
			if (pnIndices) nIndex0 = pnIndices[nIndex0];
			nIndex1 = ((i % 2) == 0) ? (i + 1) : (i + 0);
			if (pnIndices) nIndex1 = pnIndices[nIndex1];
			nIndex2 = (pnIndices) ? pnIndices[i + 2] : (i + 2);
			if ((nIndex0 == j) || (nIndex1 == j) || (nIndex2 == j))
			{
				XMFLOAT3 xmf3Edge01 = Vector3::Subtract(pxmf3Positions[nIndex1], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Edge02 = Vector3::Subtract(pxmf3Positions[nIndex2], pxmf3Positions[nIndex0]);
				XMFLOAT3 xmf3Normal = Vector3::CrossProduct(xmf3Edge01, xmf3Edge02, true);
				xmf3SumOfNormal = Vector3::Add(xmf3SumOfNormal, xmf3Normal);
			}
		}
		pxmf3Normals[j] = Vector3::Normalize(xmf3SumOfNormal);
	}
}

void CMeshIlluminated::CalculateVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices, UINT *pnIndices, int nIndices)
{
	switch (m_primitiveTopology)
	{
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST:
		if (pnIndices)
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pnIndices, nIndices);
		else
			CalculateTriangleListVertexNormals(pxmf3Normals, pxmf3Positions, nVertices);
		break;
	case D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP:
		CalculateTriangleStripVertexNormals(pxmf3Normals, pxmf3Positions, nVertices, pnIndices, nIndices);
		break;
	default:
		break;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
CCubeMeshIlluminated::CCubeMeshIlluminated(CCreateMgr *pCreateMgr, float fWidth, float fHeight, float fDepth) : CMeshIlluminated(pCreateMgr)
{
	m_nVertices = 8;
	m_nStride = sizeof(CIlluminatedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_nIndices = 36;
	UINT pnIndices[36];

	pnIndices[0] = 3; pnIndices[1] = 1; pnIndices[2] = 0;
	pnIndices[3] = 2; pnIndices[4] = 1; pnIndices[5] = 3;
	pnIndices[6] = 0; pnIndices[7] = 5; pnIndices[8] = 4;
	pnIndices[9] = 1; pnIndices[10] = 5; pnIndices[11] = 0;
	pnIndices[12] = 3; pnIndices[13] = 4; pnIndices[14] = 7;
	pnIndices[15] = 0; pnIndices[16] = 4; pnIndices[17] = 3;
	pnIndices[18] = 1; pnIndices[19] = 6; pnIndices[20] = 5;
	pnIndices[21] = 2; pnIndices[22] = 6; pnIndices[23] = 1;
	pnIndices[24] = 2; pnIndices[25] = 7; pnIndices[26] = 6;
	pnIndices[27] = 3; pnIndices[28] = 7; pnIndices[29] = 2;
	pnIndices[30] = 6; pnIndices[31] = 4; pnIndices[32] = 5;
	pnIndices[33] = 7; pnIndices[34] = 4; pnIndices[35] = 6;

	m_pIndexBuffer = pCreateMgr->CreateBufferResource(pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pIndexUploadBuffer);

	m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	XMFLOAT3 pxmf3Positions[8];

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	pxmf3Positions[0] = XMFLOAT3(-fx, +fy, -fz);
	pxmf3Positions[1] = XMFLOAT3(+fx, +fy, -fz);
	pxmf3Positions[2] = XMFLOAT3(+fx, +fy, +fz);
	pxmf3Positions[3] = XMFLOAT3(-fx, +fy, +fz);
	pxmf3Positions[4] = XMFLOAT3(-fx, -fy, -fz);
	pxmf3Positions[5] = XMFLOAT3(+fx, -fy, -fz);
	pxmf3Positions[6] = XMFLOAT3(+fx, -fy, +fz);
	pxmf3Positions[7] = XMFLOAT3(-fx, -fy, +fz);

	XMFLOAT3 pxmf3Normals[8];
	for (int i = 0; i < 8; i++) pxmf3Normals[i] = XMFLOAT3(0.0f, 0.0f, 0.0f);

	CalculateVertexNormals(pxmf3Normals, pxmf3Positions, m_nVertices, pnIndices, m_nIndices);

	CIlluminatedVertex pVertices[8];
	for (int i = 0; i < 8; i++) pVertices[i] = CIlluminatedVertex(pxmf3Positions[i], pxmf3Normals[i]);

	m_pVertexBuffer = pCreateMgr->CreateBufferResource(pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pVertexUploadBuffer);

	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = m_nStride;
	m_vertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CCubeMeshIlluminated::~CCubeMeshIlluminated()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////
//
#define _WITH_INDEX_BUFFER

CSphereMeshIlluminated::CSphereMeshIlluminated(CCreateMgr *pCreateMgr, float fRadius, UINT nSlices, UINT nStacks) : CMeshIlluminated(pCreateMgr)
{
	m_nStride = sizeof(CIlluminatedVertex);
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fDeltaPhi = float(XM_PI / nStacks);
	float fDeltaTheta = float((2.0f * XM_PI) / nSlices);
	int k = 0;
#ifdef _WITH_INDEX_BUFFER
	m_nVertices = 2 + (nSlices * (nStacks - 1));
	XMFLOAT3 *pxmf3Positions = new XMFLOAT3[m_nVertices];
	XMFLOAT3 *pxmf3Normals = new XMFLOAT3[m_nVertices];

	pxmf3Positions[k] = XMFLOAT3(0.0f, +fRadius, 0.0f);
	pxmf3Normals[k] = Vector3::Normalize(pxmf3Positions[k]); k++;
	float theta_i, phi_j;
	for (UINT j = 1; j < nStacks; j++)
	{
		phi_j = fDeltaPhi * j;
		for (UINT i = 0; i < nSlices; i++)
		{
			theta_i = fDeltaTheta * i;
			pxmf3Positions[k] = XMFLOAT3(fRadius*sinf(phi_j)*cosf(theta_i), fRadius*cosf(phi_j), fRadius*sinf(phi_j)*sinf(theta_i));
			pxmf3Normals[k] = Vector3::Normalize(pxmf3Positions[k]); k++;
		}
	}
	pxmf3Positions[k] = XMFLOAT3(0.0f, -fRadius, 0.0f);
	pxmf3Normals[k] = Vector3::Normalize(pxmf3Positions[k]); k++;

	CIlluminatedVertex *pVertices = new CIlluminatedVertex[m_nVertices];
	for (UINT i = 0; i < m_nVertices; i++) pVertices[i] = CIlluminatedVertex(pxmf3Positions[i], pxmf3Normals[i]);
	m_pVertexBuffer = pCreateMgr->CreateBufferResource(pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pVertexUploadBuffer);

	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = m_nStride;
	m_vertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	if (pxmf3Positions) delete[] pxmf3Positions;
	if (pxmf3Normals) delete[] pxmf3Normals;

	k = 0;
	m_nIndices = (nSlices * 3) * 2 + (nSlices * (nStacks - 2) * 3 * 2);
	UINT *pnIndices = new UINT[m_nIndices];
	for (UINT i = 0; i < nSlices; i++)
	{
		pnIndices[k++] = 0;
		pnIndices[k++] = 1 + ((i + 1) % nSlices);
		pnIndices[k++] = 1 + i;
	}
	for (UINT j = 0; j < nStacks - 2; j++)
	{
		for (UINT i = 0; i < nSlices; i++)
		{
			pnIndices[k++] = 1 + (i + (j * nSlices));
			pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			pnIndices[k++] = 1 + (i + ((j + 1) * nSlices));
			pnIndices[k++] = 1 + (((i + 1) % nSlices) + (j * nSlices));
			pnIndices[k++] = 1 + (((i + 1) % nSlices) + ((j + 1) * nSlices));
		}
	}
	for (UINT i = 0; i < nSlices; i++)
	{
		pnIndices[k++] = (m_nVertices - 1);
		pnIndices[k++] = ((m_nVertices - 1) - nSlices) + i;
		pnIndices[k++] = ((m_nVertices - 1) - nSlices) + ((i + 1) % nSlices);
	}

	m_pIndexBuffer = pCreateMgr->CreateBufferResource(pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pIndexUploadBuffer);

	m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	if (pnIndices) delete[] pnIndices;
#else
	m_nVertices = (nSlices * 3) * 2 + (nSlices * (nStacks - 2) * 3 * 2);
	pxmf3Positions = new XMFLOAT3[m_nVertices];
	XMFLOAT3 *pxmf3Normals = new XMFLOAT3[m_nVertices];

	float theta_i, theta_ii, phi_j = 0.0f, phi_jj = fDeltaPhi;
	for (int i = 0; i < nSlices; i++)
	{
		theta_i = fDeltaTheta * i;
		theta_ii = fDeltaTheta * (i + 1);
		pxmf3Positions[k] = XMFLOAT3(0.0f, +fRadius, 0.0f);
		D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
		pxmf3Positions[k] = XMFLOAT3(fRadius*cosf(theta_i)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_i)*sinf(phi_jj));
		D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
		pxmf3Positions[k] = XMFLOAT3(fRadius*cosf(theta_ii)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_ii)*sinf(phi_jj));
		D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
	}

	for (int j = 1; j < nStacks - 1; j++)
	{
		phi_j = fDeltaPhi * j;
		phi_jj = fDeltaPhi * (j + 1);
		for (int i = 0; i < nSlices; i++)
		{
			theta_i = fDeltaTheta * i;
			theta_ii = fDeltaTheta * (i + 1);
			pxmf3Positions[k] = XMFLOAT3(fRadius*cosf(theta_i)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_i)*sinf(phi_j));
			D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
			pxmf3Positions[k] = XMFLOAT3(fRadius*cosf(theta_i)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_i)*sinf(phi_jj));
			D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
			pxmf3Positions[k] = XMFLOAT3(fRadius*cosf(theta_ii)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_ii)*sinf(phi_j));
			D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
			pxmf3Positions[k] = XMFLOAT3(fRadius*cosf(theta_i)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_i)*sinf(phi_jj));
			D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
			pxmf3Positions[k] = XMFLOAT3(fRadius*cosf(theta_ii)*sinf(phi_jj), fRadius*cosf(phi_jj), fRadius*sinf(theta_ii)*sinf(phi_jj));
			D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
			pxmf3Positions[k] = XMFLOAT3(fRadius*cosf(theta_ii)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_ii)*sinf(phi_j));
			D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
		}
	}
	phi_j = fDeltaPhi * (nStacks - 1);
	for (int i = 0; i < nSlices; i++)
	{
		theta_i = fDeltaTheta * i;
		theta_ii = fDeltaTheta * (i + 1);
		pxmf3Positions[k] = XMFLOAT3(0.0f, -fRadius, 0.0f);
		D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
		pxmf3Positions[k] = XMFLOAT3(fRadius*cosf(theta_ii)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_ii)*sinf(phi_j));
		D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
		pxmf3Positions[k] = XMFLOAT3(fRadius*cosf(theta_i)*sinf(phi_j), fRadius*cosf(phi_j), fRadius*sinf(theta_i)*sinf(phi_j));
		D3DXVec3Normalize(&pxmf3Normals[k], &pxmf3Positions[k]); k++;
	}

	D3D11_BUFFER_DESC d3dBufferDesc;
	ZeroMemory(&d3dBufferDesc, sizeof(D3D11_BUFFER_DESC));
	d3dBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	d3dBufferDesc.ByteWidth = sizeof(XMFLOAT3) * m_nVertices;
	d3dBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	d3dBufferDesc.CPUAccessFlags = 0;
	D3D11_SUBRESOURCE_DATA d3dBufferData;
	ZeroMemory(&d3dBufferData, sizeof(D3D11_SUBRESOURCE_DATA));
	d3dBufferData.pSysMem = pxmf3Positions;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dPositionBuffer);

	d3dBufferData.pSysMem = pxmf3Normals;
	pd3dDevice->CreateBuffer(&d3dBufferDesc, &d3dBufferData, &m_pd3dNormalBuffer);

	if (pxmf3Normals) delete[] pxmf3Normals;

	ID3D11Buffer *pd3dBuffers[2] = { m_pd3dPositionBuffer, m_pd3dNormalBuffer };
	UINT pnBufferStrides[2] = { sizeof(XMFLOAT3), sizeof(XMFLOAT3) };
	UINT pnBufferOffsets[2] = { 0, 0 };
	AssembleToVertexBuffer(2, pd3dBuffers, pnBufferStrides, pnBufferOffsets);
#endif
}

CSphereMeshIlluminated::~CSphereMeshIlluminated()
{
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
CMeshIlluminatedTextured::CMeshIlluminatedTextured(CCreateMgr *pCreateMgr) : CMeshIlluminated(pCreateMgr)
{
}

CMeshIlluminatedTextured::CMeshIlluminatedTextured(CCreateMgr *pCreateMgr, UINT nVertices, XMFLOAT3 *pxmf3Positions, XMFLOAT3 *pxmf3Normals, XMFLOAT2 *pxmf2UVs, UINT nIndices, UINT *pnIndices) : CMeshIlluminated(pCreateMgr)
{
	m_nStride = sizeof(CIlluminatedTexturedVertex);
	m_nVertices = nVertices;

	CIlluminatedTexturedVertex *pVertices = new CIlluminatedTexturedVertex[m_nVertices];
	for (UINT i = 0; i < m_nVertices; i++) pVertices[i] = CIlluminatedTexturedVertex(pxmf3Positions[i], pxmf3Normals[i], pxmf2UVs[i]);

	m_pVertexBuffer = pCreateMgr->CreateBufferResource(pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pVertexUploadBuffer);

	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = m_nStride;
	m_vertexBufferView.SizeInBytes = m_nStride * m_nVertices;

	if (nIndices > 0)
	{
		m_nIndices = nIndices;

		m_pIndexBuffer = pCreateMgr->CreateBufferResource(pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pIndexUploadBuffer);

		m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
		m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
		m_indexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;
	}
}

CMeshIlluminatedTextured::~CMeshIlluminatedTextured()
{
}

//////////////////////////////////////////////////////////////////////////////////
//
CCubeMeshIlluminatedTextured::CCubeMeshIlluminatedTextured(CCreateMgr *pCreateMgr, float fWidth, float fHeight, float fDepth) : CMeshIlluminatedTextured(pCreateMgr)
{
	m_nVertices = 36;
	m_nStride = sizeof(CIlluminatedTexturedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	XMFLOAT3 pxmf3Positions[36];
	int i = 0;
	//FU
	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, -fz);
	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, -fz);
	//FD
	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, -fz);
	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, -fz);
	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
	//UU
	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, +fz);
	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
	//UD
	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, -fz);
	//BU
	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, +fz);
	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, +fz);
	//BD
	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, +fz);
	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, +fz);
	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
	//DU
	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, -fz);
	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
	//DD
	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, +fz);
	//LU
	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, -fz);
	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
	//LD
	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, +fz);
	//RU
	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, +fz);
	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
	//RD
	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);
	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, -fz);

	XMFLOAT2 pxmf2TexCoords[36];
	i = 0;
	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);

	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);

	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);

	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);

	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);

	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);

	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);

	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);

	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);

	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);

	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);

	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);

	XMFLOAT3 pxmf3Normals[36];
	CalculateVertexNormals(pxmf3Normals, pxmf3Positions, m_nVertices, NULL, 0);

	XMFLOAT3 pxmf3Tangents[36];
	CalculateTriangleListVertexTangents(pxmf3Tangents, pxmf3Positions, m_nVertices, pxmf2TexCoords);

	CIlluminatedTexturedVertex pVertices[36];
	for (i = 0; i < 36; i++) pVertices[i] = CIlluminatedTexturedVertex(pxmf3Positions[i], pxmf3Normals[i], pxmf2TexCoords[i], pxmf3Tangents[i]);

	m_pVertexBuffer = pCreateMgr->CreateBufferResource(pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pVertexUploadBuffer);

	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = m_nStride;
	m_vertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CCubeMeshIlluminatedTextured::~CCubeMeshIlluminatedTextured()
{
}

CCubeWithIndices::CCubeWithIndices(CCreateMgr *pCreateMgr, float fWidth, float fHeight, float fDepth) : CMeshIlluminatedTextured(pCreateMgr)
{
	m_nVertices = 8;
	m_nStride = sizeof(CIlluminatedTexturedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_nIndices = 36;

	UINT pnIndices[36];
	pnIndices[0] = 3; pnIndices[1] = 1; pnIndices[2] = 0;
	pnIndices[3] = 2; pnIndices[4] = 1; pnIndices[5] = 3;
	pnIndices[6] = 0; pnIndices[7] = 5; pnIndices[8] = 4;
	pnIndices[9] = 1; pnIndices[10] = 5; pnIndices[11] = 0;
	pnIndices[12] = 3; pnIndices[13] = 4; pnIndices[14] = 7;
	pnIndices[15] = 0; pnIndices[16] = 4; pnIndices[17] = 3;
	pnIndices[18] = 1; pnIndices[19] = 6; pnIndices[20] = 5;
	pnIndices[21] = 2; pnIndices[22] = 6; pnIndices[23] = 1;
	pnIndices[24] = 2; pnIndices[25] = 7; pnIndices[26] = 6;
	pnIndices[27] = 3; pnIndices[28] = 7; pnIndices[29] = 2;
	pnIndices[30] = 6; pnIndices[31] = 4; pnIndices[32] = 5;
	pnIndices[33] = 7; pnIndices[34] = 4; pnIndices[35] = 6;

	//인덱스 버퍼를 생성한다.
	m_pIndexBuffer = pCreateMgr->CreateBufferResource(
		pnIndices,
		sizeof(UINT) * m_nIndices,
		D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER,
		&m_pIndexUploadBuffer);

	//인덱스 버퍼 뷰를 생성한다.
	m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	float fx = fWidth * 0.5f, fy = fHeight * 0.5f, fz = fDepth * 0.5f;

	XMFLOAT3 pxmf3Positions[8];
	int i = 0;
	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, +fz);
	pxmf3Positions[i++] = XMFLOAT3(-fx, +fy, -fz);
	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, -fz);
	pxmf3Positions[i++] = XMFLOAT3(+fx, +fy, +fz);

	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, +fz);
	pxmf3Positions[i++] = XMFLOAT3(-fx, -fy, -fz);
	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, -fz);
	pxmf3Positions[i++] = XMFLOAT3(+fx, -fy, +fz);

	XMFLOAT2 pxmf2TexCoords[8];
	i = 0;
	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 0.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 0.0f);

	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(1.0f, 1.0f);
	pxmf2TexCoords[i++] = XMFLOAT2(0.0f, 1.0f);

	XMFLOAT3 pxmf3Normals[8];
	CalculateVertexNormals(pxmf3Normals, pxmf3Positions, m_nVertices, pnIndices, m_nIndices);

	XMFLOAT3 pxmf3Tangents[8];
	CalculateTriangleListVertexTangents(pxmf3Tangents, pxmf3Positions, m_nVertices, pxmf2TexCoords, pnIndices, m_nIndices);

	CIlluminatedTexturedVertex pVertices[8];
	for (i = 0; i < 8; i++) pVertices[i] = CIlluminatedTexturedVertex(pxmf3Positions[i], pxmf3Normals[i], pxmf2TexCoords[i], pxmf3Tangents[i]);

	m_pVertexBuffer = pCreateMgr->CreateBufferResource(pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pVertexUploadBuffer);

	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = m_nStride;
	m_vertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CCubeWithIndices::~CCubeWithIndices()
{
}

CFBXMesh::CFBXMesh(CCreateMgr * pCreateMgr) : CMeshIlluminated(pCreateMgr)
{
	CMeshImporter importer;
	importer.LoadMeshData("FBXBinary//teapot_test.meshinfo");
	m_nVertices = importer.m_iVerticesCnt;
	m_nStride = sizeof(CIlluminatedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_nIndices = importer.m_iTriCnt * 3;
	UINT* pnIndices = new UINT[m_nIndices];
	int indicesCount = 0;
	for (auto d : importer.m_xmTriIndex) {
		pnIndices[indicesCount] = d.x;
		pnIndices[indicesCount + 1] = d.y;
		pnIndices[indicesCount + 2] = d.z;
		indicesCount += 3;
	}

	m_pIndexBuffer = pCreateMgr->CreateBufferResource(pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pIndexUploadBuffer);

	m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	int nVertices = importer.m_iVerticesCnt;
	XMFLOAT3* pxmf3Positions = new XMFLOAT3[nVertices];
	XMFLOAT3* pxmf3Normals = new XMFLOAT3[nVertices];

	int vecticesCount = 0;
	for (auto d : importer.m_xmVertex) {
		pxmf3Positions[vecticesCount] = XMFLOAT3(d.pos.x*0.1f, d.pos.y*0.1f, d.pos.z*0.1f);
		pxmf3Normals[vecticesCount] = XMFLOAT3(d.normal.x, d.normal.y, d.normal.z);
		++vecticesCount;

	}

	CalculateVertexNormals(pxmf3Normals, pxmf3Positions, m_nVertices, pnIndices, m_nIndices);

	CIlluminatedVertex* pVertices = new CIlluminatedVertex[nVertices];
	for (int i = 0; i < nVertices; i++) pVertices[i] = CIlluminatedVertex(pxmf3Positions[i], pxmf3Normals[i]);

	m_pVertexBuffer = pCreateMgr->CreateBufferResource(pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pVertexUploadBuffer);

	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = m_nStride;
	m_vertexBufferView.SizeInBytes = m_nStride * m_nVertices;
}

CFBXMesh::~CFBXMesh()
{
}

CSkinnedMesh::CSkinnedMesh(CCreateMgr * pCreateMgr, char* in) : CMeshIlluminatedTextured(pCreateMgr)
{
	CMeshImporter importer;
	importer.LoadMeshData(in);
	m_nVertices = importer.m_iVerticesCnt;
	m_nStride = sizeof(CSkinnedVertex);
	m_nOffset = 0;
	m_nSlot = 0;
	m_primitiveTopology = D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	m_nIndices = importer.m_iTriCnt * 3;
	UINT* pnIndices = new UINT[m_nIndices];
	int indicesCount = 0;
	for (auto d : importer.m_xmTriIndex) {
		pnIndices[indicesCount] = d.x;
		pnIndices[indicesCount + 1] = d.y;
		pnIndices[indicesCount + 2] = d.z;
		indicesCount += 3;
	}

	m_pIndexBuffer = pCreateMgr->CreateBufferResource(pnIndices, sizeof(UINT) * m_nIndices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_INDEX_BUFFER, &m_pIndexUploadBuffer);

	m_indexBufferView.BufferLocation = m_pIndexBuffer->GetGPUVirtualAddress();
	m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;
	m_indexBufferView.SizeInBytes = sizeof(UINT) * m_nIndices;

	int nVertices = importer.m_iVerticesCnt;
	XMFLOAT3* pxmf3Positions = new XMFLOAT3[nVertices];
	XMFLOAT3* pxmf3Normals = new XMFLOAT3[nVertices];

	XMFLOAT2* pxmf2TexCoords = new XMFLOAT2[nVertices];
	XMFLOAT4* pxmf4SkinWeight = new XMFLOAT4[nVertices];
	XMFLOAT4* pxmf4SkinIndex = new XMFLOAT4[nVertices];

	
	int vecticesCount = 0;
	for (auto d : importer.m_xmVertex) {
		pxmf3Normals[vecticesCount] = XMFLOAT3(d.normal.x, d.normal.y, d.normal.z);
		pxmf2TexCoords[vecticesCount] = XMFLOAT2(d.uv.x, d.uv.y);
		pxmf4SkinWeight[vecticesCount] = XMFLOAT4(d.skinweight.x, d.skinweight.y, d.skinweight.z, d.skinweight.w);
		pxmf4SkinIndex[vecticesCount] = XMFLOAT4(d.skinindex.x, d.skinindex.y, d.skinindex.z, d.skinindex.w);
		pxmf3Positions[vecticesCount] = XMFLOAT3(d.pos.x, d.pos.y, d.pos.z);
		++vecticesCount;
	}

	CalculateVertexNormals(pxmf3Normals, pxmf3Positions, m_nVertices, pnIndices, m_nIndices);
	XMFLOAT3* pxmf3Tangents = new XMFLOAT3[nVertices];
	CalculateTriangleListVertexTangents(pxmf3Tangents, pxmf3Positions, m_nVertices, pxmf2TexCoords, pnIndices, m_nIndices);


	m_pVertices = new CSkinnedVertex[nVertices];
	for (int i = 0; i < nVertices; i++) m_pVertices[i] = CSkinnedVertex(pxmf3Positions[i], pxmf3Normals[i], pxmf2TexCoords[i], pxmf3Tangents[i], pxmf4SkinWeight[i], pxmf4SkinIndex[i]);

	m_pVertexBuffer = pCreateMgr->CreateBufferResource(m_pVertices, m_nStride * m_nVertices, D3D12_HEAP_TYPE_DEFAULT, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER, &m_pVertexUploadBuffer);

	m_vertexBufferView.BufferLocation = m_pVertexBuffer->GetGPUVirtualAddress();
	m_vertexBufferView.StrideInBytes = m_nStride;
	m_vertexBufferView.SizeInBytes = m_nStride * m_nVertices;



	delete[](pxmf3Positions);
	delete[](pnIndices);
	delete[](pxmf3Normals);
	delete[](pxmf2TexCoords);
	delete[](pxmf4SkinWeight);
	delete[](pxmf4SkinIndex);
	delete[](pxmf3Tangents);
}

void CSkinnedMesh::ReleaseUploadBuffers()
{
	Safe_Release(m_pIndexUploadBuffer);
}


CSkinnedMesh::~CSkinnedMesh()
{
}
