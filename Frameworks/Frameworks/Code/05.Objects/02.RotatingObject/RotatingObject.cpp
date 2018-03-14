#include "stdafx.h"
#include "RotatingObject.h"
#include "06.Meshes/00.Vertex/Vertex.h"
/// <summary>
/// 목적: Create 통합 중 테스트 용 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-01-12
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
}

void CAnimatedObject::Animate(float timeElapsed)
{
	
	CSkinnedMesh* pMesh = dynamic_cast<CSkinnedMesh*>(m_ppMeshes[0]);
	
	CSkinnedVertex* pVertices = pMesh->m_pVertices;
	int Bcnt = m_pSkeleton->GetBoneCount();

	XMFLOAT4X4* mat4x4 = new XMFLOAT4X4[Bcnt];
	for (int i = 0; i < Bcnt; ++i) {
		mat4x4[i] = m_pSkeleton->GetBone(i).m_m4x4Matrix;
	}

	XMFLOAT4X4* ani4x4 = new XMFLOAT4X4[Bcnt];
	for (int i = 0; i < Bcnt; ++i) {
		ani4x4[i] = m_pSkeleton->GetBone(i).GetFrame(47);
	}

	XMMATRIX*  mat = new XMMATRIX[Bcnt];
	for (int i = 0; i < Bcnt; ++i) {
		mat[i] = XMMatrixMultiply(XMLoadFloat4x4(&ani4x4[i]), XMLoadFloat4x4(&mat4x4[i]));
	}

	int nVertices = pMesh->m_nVerticesCnt;
	
	int vecticesCount  = 0;
	
	
	for (int i = 0; i <nVertices;++i) {
		
		XMFLOAT4 pxmf4SkinWeight = pVertices[i].GetSkinWeight();
		XMFLOAT4 pxmf4SkinIndex = pVertices[i].GetSkinSkinIndex();
		XMFLOAT3 xmf3position = pVertices->GetPosition();
		XMFLOAT3 pos;
		XMFLOAT3 result = XMFLOAT3(0, 0, 0);

		if (pxmf4SkinWeight.x != 0) {
			XMStoreFloat3(&pos, DirectX::XMVector3TransformCoord(XMLoadFloat3(&xmf3position),
				XMMatrixTranspose(mat[(int)pxmf4SkinIndex.x])));
			result.x += pxmf4SkinWeight.x*pos.x;
			result.y += pxmf4SkinWeight.x*pos.y;
			result.z += pxmf4SkinWeight.x*pos.z;

			if (pxmf4SkinWeight.y != 0) {
				XMStoreFloat3(&pos, DirectX::XMVector3TransformCoord(XMLoadFloat3(&xmf3position),
					XMMatrixTranspose(mat[(int)pxmf4SkinIndex.y])));
				result.x += pxmf4SkinWeight.y*pos.x;
				result.y += pxmf4SkinWeight.y*pos.y;
				result.z += pxmf4SkinWeight.y*pos.z;

				if (pxmf4SkinWeight.z != 0) {
					XMStoreFloat3(&pos, DirectX::XMVector3TransformCoord(XMLoadFloat3(&xmf3position),
						XMMatrixTranspose(mat[(int)pxmf4SkinIndex.z])));
					result.x += pxmf4SkinWeight.z*pos.x;
					result.y += pxmf4SkinWeight.z*pos.y;
					result.z += pxmf4SkinWeight.z*pos.z;

					if (pxmf4SkinWeight.w != 0) {
						XMStoreFloat3(&pos, DirectX::XMVector3TransformCoord(XMLoadFloat3(&xmf3position),
							XMMatrixTranspose(mat[(int)pxmf4SkinIndex.w])));
						result.x += pxmf4SkinWeight.w*pos.x;
						result.y += pxmf4SkinWeight.w*pos.y;
						result.z += pxmf4SkinWeight.w*pos.z;
					}
				}
			}
		}
		xmf3position = result;
		pVertices[i].SetPosition(xmf3position);
		++vecticesCount;
	}


}

CAnimatedObject::~CAnimatedObject()
{
}
