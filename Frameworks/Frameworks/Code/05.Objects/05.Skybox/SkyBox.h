#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"

class CSkyBox : public CBaseObject
{
public: // ������, �Ҹ���
	CSkyBox(CCreateMgr *pCreateMgr);
	virtual ~CSkyBox();

public: // ���� �Լ�
	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);
};

