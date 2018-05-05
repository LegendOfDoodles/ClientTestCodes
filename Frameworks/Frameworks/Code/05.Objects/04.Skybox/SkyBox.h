#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"

class CSkyBox : public CBaseObject
{
public: // 생성자, 소멸자
	CSkyBox(CCreateMgr *pCreateMgr);
	virtual ~CSkyBox();

public: // 공개 함수
	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);
};

