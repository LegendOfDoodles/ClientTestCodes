#pragma once
#include "05.Objects/96.Billboard/99.BillboardObject/Billboard.h"

class CGaugeObject : public CBillboardObject
{
public:	// ������, �Ҹ���
	CGaugeObject(CCreateMgr *pCreateMgr);
	CGaugeObject(CCreateMgr *pCreateMgr, GagueUIType type);
	virtual ~CGaugeObject();

public: // �����Լ�
	virtual void Animate(float fTimeElapsed);
	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);

	virtual float GetCurrentHP();

	void GetmasterObjectType(ObjectType type) { m_MasterObjectType = type; };

	StatesType GetState() { return m_pMasterObject->GetState(); }
	virtual void SetObject(CCollisionObject *pObject) { m_pMasterObject = pObject; };


protected: // ����
	CCollisionObject * m_pMasterObject;
	GagueUIType		  m_Type;
	ObjectType		  m_MasterObjectType;
};