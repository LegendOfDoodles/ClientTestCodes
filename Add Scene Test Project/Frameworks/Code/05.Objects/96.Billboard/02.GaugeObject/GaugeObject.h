#pragma once
#include "05.Objects/96.Billboard/99.BillboardObject/Billboard.h"

class CGaugeObject : public CBillboardObject
{
public:	// ������, �Ҹ���
	CGaugeObject(shared_ptr<CCreateMgr> pCreateMgr);
	CGaugeObject(shared_ptr<CCreateMgr> pCreateMgr, GaugeUIType type);
	virtual ~CGaugeObject();

public: // �����Լ�
	virtual void Animate(float fTimeElapsed);
	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);

	virtual float GetCurrentHP();

	StatesType GetState() { return m_pMasterObject->GetState(); }
	virtual void SetMasterObject(CCollisionObject *pObject) { m_pMasterObject = pObject; };

	CCollisionObject* GetMasterObject() { return m_pMasterObject; }

protected: // ����
	CCollisionObject * m_pMasterObject;
	GaugeUIType		  m_Type;
};