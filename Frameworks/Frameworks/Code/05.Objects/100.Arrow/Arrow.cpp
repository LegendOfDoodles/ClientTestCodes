#include "stdafx.h"
#include "Arrow.h"

/// <summary>
/// 목적: 작업 시 x, y, z 방향 잡기 용 화살표 오브젝트
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-04-09
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CArrow::CArrow(CCreateMgr *pCreateMgr) : CBaseObject(pCreateMgr)
{
	CArrowMesh *pArrowMesh = new CArrowMesh(pCreateMgr, 1000);
	SetMesh(0, pArrowMesh);

	CreateShaderVariables(pCreateMgr);
}

CArrow::~CArrow()
{
}
