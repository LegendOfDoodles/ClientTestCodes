#include "stdafx.h"
#include "Arrow.h"

/// <summary>
/// ����: �۾� �� x, y, z ���� ��� �� ȭ��ǥ ������Ʈ
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-04-09
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CArrow::CArrow(CCreateMgr *pCreateMgr) : CBaseObject(pCreateMgr)
{
	CArrowMesh *pArrowMesh = new CArrowMesh(pCreateMgr, 1000);
	SetMesh(0, pArrowMesh);

	CreateShaderVariables(pCreateMgr);
}

CArrow::~CArrow()
{
}
