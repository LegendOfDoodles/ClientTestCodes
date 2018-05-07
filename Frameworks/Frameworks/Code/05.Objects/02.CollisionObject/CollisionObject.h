#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"

class CCollisionObject : public CBaseObject
{
public:	// 생성자, 소멸자
	CCollisionObject(CCreateMgr *pCreateMgr, int nMeshes = 1);
	virtual ~CCollisionObject();

public: // 공개 함수
	virtual void SetCollisionSize(float size) { m_fCollisionSize = size; }
	virtual float GetCollisionSize() { return m_fCollisionSize; }

protected: // 내부 함수

protected: // 변수
	float m_fCollisionSize{ 1 };
};
