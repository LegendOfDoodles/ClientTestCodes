#include "stdafx.h"
#include "RotatingObject.h"

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
	pMesh->m_pVertices;

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

	int vecticesCount = 0;
	
	
	/*
	
	for (auto d : pVertices) {
		
		pxmf4SkinWeight[vecticesCount] = XMFLOAT4(d.skinweight.x, d.skinweight.y, d.skinweight.z, d.skinweight.w);
		pxmf4SkinIndex[vecticesCount] = XMFLOAT4(d.skinindex.x, d.skinindex.y, d.skinindex.z, d.skinindex.w);
		XMFLOAT3 xmf3position = XMFLOAT3(d.pos.x, d.pos.y, d.pos.z);
		XMFLOAT3 pos;
		XMFLOAT3 result = XMFLOAT3(0, 0, 0);

		if (pxmf4SkinWeight[vecticesCount].x != 0) {
			XMStoreFloat3(&pos, DirectX::XMVector3TransformCoord(XMLoadFloat3(&xmf3position),
				XMMatrixTranspose(mat[(int)pxmf4SkinIndex[vecticesCount].x])));
			result.x += pxmf4SkinWeight[vecticesCount].x*pos.x;
			result.y += pxmf4SkinWeight[vecticesCount].x*pos.y;
			result.z += pxmf4SkinWeight[vecticesCount].x*pos.z;

			if (pxmf4SkinWeight[vecticesCount].y != 0) {
				XMStoreFloat3(&pos, DirectX::XMVector3TransformCoord(XMLoadFloat3(&xmf3position),
					XMMatrixTranspose(mat[(int)pxmf4SkinIndex[vecticesCount].y])));
				result.x += pxmf4SkinWeight[vecticesCount].y*pos.x;
				result.y += pxmf4SkinWeight[vecticesCount].y*pos.y;
				result.z += pxmf4SkinWeight[vecticesCount].y*pos.z;

				if (pxmf4SkinWeight[vecticesCount].z != 0) {
					XMStoreFloat3(&pos, DirectX::XMVector3TransformCoord(XMLoadFloat3(&xmf3position),
						XMMatrixTranspose(mat[(int)pxmf4SkinIndex[vecticesCount].z])));
					result.x += pxmf4SkinWeight[vecticesCount].z*pos.x;
					result.y += pxmf4SkinWeight[vecticesCount].z*pos.y;
					result.z += pxmf4SkinWeight[vecticesCount].z*pos.z;

					if (pxmf4SkinWeight[vecticesCount].w != 0) {
						XMStoreFloat3(&pos, DirectX::XMVector3TransformCoord(XMLoadFloat3(&xmf3position),
							XMMatrixTranspose(mat[(int)pxmf4SkinIndex[vecticesCount].w])));
						result.x += pxmf4SkinWeight[vecticesCount].w*pos.x;
						result.y += pxmf4SkinWeight[vecticesCount].w*pos.y;
						result.z += pxmf4SkinWeight[vecticesCount].w*pos.z;
					}
				}
			}
		}
		xmf3position = result;
		pxmf3Positions[vecticesCount] = xmf3position;
		++vecticesCount;
	}
	*/
		
}

CAnimatedObject::~CAnimatedObject()
{
}
