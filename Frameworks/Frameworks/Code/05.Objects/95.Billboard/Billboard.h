#pragma once

#include "stdafx.h"
#include "Billboard.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"
#include "05.Objects/01.Camera/01.AOSCamera/AOSCamera.h"
#include "06.Meshes/00.Vertex/Vertex.h"

enum UIType {
	Minimap = 0,
	KDA		= 1,
	Skill	= 2,
	Status	= 3
};

enum GaugeUiType {
	PlayerGauge,
	MinionGauge
};

class CBillboardObject : public CBaseObject
{
public:	// 생성자, 소멸자
	CBillboardObject(CCreateMgr *pCreateMgr);
	virtual ~CBillboardObject();

public: // 공개함수
	virtual void Animate(float fTimeElapsed);

	void SetLookAt();
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; };

protected: // 내부 함수

private: // 변수

protected:
	CCamera * m_pCamera{ NULL };
};

class CUIObject : public CBillboardObject
{
public:	// 생성자, 소멸자
	CUIObject(CCreateMgr *pCreateMgr);
	CUIObject(CCreateMgr *pCreateMgr, UIType type);
	virtual ~CUIObject();

public: // 공개함수
	virtual void Animate(float fTimeElapsed);
	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);

	virtual void SetDistance(float x) {
		m_xmf4x4World = Matrix4x4::Identity();
		m_fDistance = x;
	}

private: // 변수
	XMFLOAT3	m_xmf3Position;
	float		m_fDistance;
	UIType		m_type;
};

class CHPGaugeObjects : public CBillboardObject
{
public:	// 생성자, 소멸자
	CHPGaugeObjects(CCreateMgr *pCreateMgr);
	CHPGaugeObjects(CCreateMgr *pCreateMgr, GaugeUiType type);
	virtual ~CHPGaugeObjects();

public: // 공개함수
	virtual void Animate(float fTimeElapsed);

	StatesType GetState() { return m_pMasterObject->GetState(); }

	void SetObject(CCollisionObject *pObject) { m_pMasterObject = pObject; };

protected: // 변수
	CCollisionObject * m_pMasterObject;
	GaugeUiType		m_Type;
};