#pragma once
#include "05.Objects/96.Billboard/99.BillboardObject/Billboard.h"

class CNumberOjbect : public CBillboardObject
{
public:
	CNumberOjbect(CCreateMgr *pCreateMgr);
	virtual ~CNumberOjbect();

public: // 공개함수
	virtual void Animate(float fTimeElapsed);
	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);

	virtual void SetDistance(float x) {
		m_xmf4x4World = Matrix4x4::Identity();

		m_fDistance = x;
	}

	virtual void SetNum(int i) { if (i >= 0) m_iNum = i; }

	virtual void Num();

protected: // 변수
	float		m_fDistance;
	int			m_iNum;

};