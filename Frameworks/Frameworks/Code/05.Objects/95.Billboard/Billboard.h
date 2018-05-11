#pragma once

#include "stdafx.h"
#include "Billboard.h"
#include "05.Objects/00.BaseObject/BaseObject.h"
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
public:
	CUIObject(CCreateMgr *pCreateMgr);
	CUIObject(CCreateMgr *pCreateMgr, UIType type);
	virtual ~CUIObject();

	virtual void SetPos(XMFLOAT3 xmf3Position);
	virtual void SetDistance(float x) {
		m_xmf4x4World = Matrix4x4::Identity();

		m_fDistance = x;
	}

	virtual void Animate(float fTimeElapsed);
	virtual void Render(CCamera *pCamera, UINT istanceCnt = 1);

private:
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
	void SetObject(CBaseObject *pObject) { m_pMasterObject = pObject; };

	virtual void Animate(float fTimeElapsed);

protected:
	CBaseObject * m_pMasterObject;

private: // 변수
	GaugeUiType		m_Type;

protected:

};

class CMinimapIconObjects :public CUIObject
{
public:
	CMinimapIconObjects(CCreateMgr *pCreateMgr) ;
	virtual ~CMinimapIconObjects();

	void SetObject(CBaseObject *pObject) { m_pMasterObject = pObject; };

	virtual void Animate(float fTimeElapsed);

protected:
	CBaseObject * m_pMasterObject;

};