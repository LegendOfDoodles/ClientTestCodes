#include "stdafx.h"
#include "Arrow.h"

/// <summary>
/// ����: �۾� �� x, y, z ���� ��� �� ȭ��ǥ ������Ʈ
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-04-11
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
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
