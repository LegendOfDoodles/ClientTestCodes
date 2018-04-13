#include "stdafx.h"
#include "RotatingObject.h"
#include "06.Meshes/00.Vertex/Vertex.h"
/// <summary>
/// 목적: Create 통합 중 테스트 용 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-03-31
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CRotatingObject::CRotatingObject(CCreateMgr *pCreateMgr)
	: CBaseObject(pCreateMgr)
{
	m_rotationAxis = XMFLOAT3(0.0f, 1.0f, 0.0f);
	m_rotationSpeed = 90.0f;
}

CRotatingObject::~CRotatingObject()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CRotatingObject::Animate(float timeElapsed)
{
	CBaseObject::Rotate(&m_rotationAxis, m_rotationSpeed * timeElapsed);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수

CAnimatedObject::CAnimatedObject(CCreateMgr * pCreateMgr) : CBaseObject(pCreateMgr)
{
	m_fFrameTime = 0;
}

void CAnimatedObject::Animate(float timeElapsed)
{
	
	m_fFrameTime += 30*timeElapsed;
	if (aniState == 0&&m_fFrameTime > 33) {
		m_fFrameTime -= 33;
	}
	else if (aniState == 1 && m_fFrameTime > 49) {
		m_fFrameTime -= 49;
	}
	else if (aniState == 2 && m_fFrameTime > 33) {
		m_fFrameTime -= 33;
	}
}

void CAnimatedObject::Render(CCamera * pCamera, UINT instanceCnt)
{
	CBaseObject::Render(pCamera, instanceCnt);
	CSkinnedMesh* pMesh = dynamic_cast<CSkinnedMesh*>(m_ppMeshes[0]);

	CSkinnedVertex* pVertices = new CSkinnedVertex[pMesh->m_nVerticesCnt];
	int VerticesLength = pMesh->GetStride() * pMesh->m_nVerticesCnt;
	memcpy(pVertices, pMesh->m_pVertices, VerticesLength);
	int Bcnt = m_pSkeleton->GetBoneCount();

	XMFLOAT4X4*  mat = new XMFLOAT4X4[Bcnt];
	for (int i = 0; i < Bcnt; ++i) {
		if(aniState==0)
			mat[i] = m_pSkeleton->GetBone(i).GetFrame((int)m_fFrameTime);
		else if(aniState ==1 )
			mat[i] = m_pSkeleton1->GetBone(i).GetFrame((int)m_fFrameTime);
		else if (aniState == 2)
			mat[i] = m_pSkeleton2->GetBone(i).GetFrame((int)m_fFrameTime);
	}

	int nVertices = pMesh->m_nVerticesCnt;

	XMFLOAT4 pxmf4SkinWeight;
	XMFLOAT4 pxmf4SkinIndex;
	XMFLOAT3 xmf3position;
	XMFLOAT3 pos;
	XMFLOAT3 result;
	for (int i = 0; i <nVertices; ++i) {

		pxmf4SkinWeight = pVertices[i].GetSkinWeight();
		pxmf4SkinIndex = pVertices[i].GetSkinSkinIndex();
		xmf3position = pVertices[i].GetPosition();
		result = XMFLOAT3(0, 0, 0);

		if (pxmf4SkinWeight.x != 0) {
			pos = Vector3::TransformCoord(xmf3position, mat[(int)pxmf4SkinIndex.x]);
			result.x += pxmf4SkinWeight.x*pos.x;
			result.y += pxmf4SkinWeight.x*pos.y;
			result.z += pxmf4SkinWeight.x*pos.z;

			if (pxmf4SkinWeight.y != 0) {
				pos = Vector3::TransformCoord(xmf3position, mat[(int)pxmf4SkinIndex.y]);

				result.x += pxmf4SkinWeight.y*pos.x;
				result.y += pxmf4SkinWeight.y*pos.y;
				result.z += pxmf4SkinWeight.y*pos.z;

				if (pxmf4SkinWeight.z != 0) {
					pos = Vector3::TransformCoord(xmf3position, mat[(int)pxmf4SkinIndex.z]);

					result.x += pxmf4SkinWeight.z*pos.x;
					result.y += pxmf4SkinWeight.z*pos.y;
					result.z += pxmf4SkinWeight.z*pos.z;

					if (pxmf4SkinWeight.w != 0) {
						pos = Vector3::TransformCoord(xmf3position, mat[(int)pxmf4SkinIndex.w]);

						result.x += pxmf4SkinWeight.w*pos.x;
						result.y += pxmf4SkinWeight.w*pos.y;
						result.z += pxmf4SkinWeight.w*pos.z;
					}
				}
			}
		}
		xmf3position = result;
		pVertices[i].SetPosition(xmf3position);

	}

	delete[] pVertices;
}

CAnimatedObject::~CAnimatedObject()
{
}
