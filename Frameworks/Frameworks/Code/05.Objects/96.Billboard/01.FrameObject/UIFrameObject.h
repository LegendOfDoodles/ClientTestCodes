#pragma once
#include "05.Objects/96.Billboard/99.BillboardObject/Billboard.h"

class CUIFrameObject : public CBillboardObject
{
public:
	CUIFrameObject(CCreateMgr *pCreateMgr);
	CUIFrameObject(CCreateMgr *pCreateMgr, UIFrameType type);
	virtual ~CUIFrameObject();

public: // �����Լ�
	virtual void Animate(float fTimeElapsed);

	virtual void SetDistance(float x) {
		m_xmf4x4World = Matrix4x4::Identity();

		m_fDistance = x;
	}

protected: // ����
	XMFLOAT3	m_xmf3Position;
	float		m_fDistance;
	UIFrameType		m_type;
};