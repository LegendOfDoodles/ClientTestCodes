#pragma once
#include "05.Objects/96.Billboard/99.BillboardObject/Billboard.h"

class CUIFrameObject : public CBillboardObject
{
public:
	CUIFrameObject(CCreateMgr *pCreateMgr);
	CUIFrameObject(CCreateMgr *pCreateMgr, UIFrameType type);
	virtual ~CUIFrameObject();

public: // 공개함수
	virtual void Animate(float fTimeElapsed);
	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);

	virtual int GetType() { return m_type; }
	virtual float GetCurrentHP();

	virtual void SetDistance(float x) {
		m_xmf4x4World = Matrix4x4::Identity();

		m_fDistance = x;
	}

	virtual void SetObject(CCollisionObject *pObject) { m_pMasterObject = pObject; };
	CCollisionObject* GetMasterObject() { return m_pMasterObject; };

protected: // 변수
	XMFLOAT3	m_xmf3Position;
	float		m_fDistance;
	UIFrameType		m_type;

	CCollisionObject *m_pMasterObject{NULL};

};