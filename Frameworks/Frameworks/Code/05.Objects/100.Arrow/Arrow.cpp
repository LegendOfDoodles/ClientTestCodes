#include "stdafx.h"
#include "Arrow.h"

/// <summary>
/// 목적: 작업 시 x, y, z 방향 잡기 용 화살표 오브젝트
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-04-11
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CArrow::CArrow(CCreateMgr *pCreateMgr, float length) : CBaseObject(pCreateMgr)
{
	//CArrowMesh *pArrowMesh = new CArrowMesh(pCreateMgr, length);

	//SetMesh(0, pArrowMesh);
	CCubeMesh4Collider *pCubeMesh = new CCubeMesh4Collider(pCreateMgr, 24.84, 57.96, 24.84);
	SetMesh(0, pCubeMesh);

	CreateShaderVariables(pCreateMgr);
}

CArrow::~CArrow()
{
}
