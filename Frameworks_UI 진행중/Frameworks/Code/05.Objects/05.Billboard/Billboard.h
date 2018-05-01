#pragma once

#include "stdafx.h"
#include "Billboard.h"
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "06.Meshes/00.Vertex/Vertex.h"

/// <summary>
/// 목적: UI 이용을 위한 Bilboard 클래스 제작
/// 최종 수정자:  이용선
/// 수정자 목록:  이용선
/// 최종 수정 날짜: 2018-04-04
/// </summary>

enum Type {
	Minimap = 0,
	KDA		= 1,
	Skill	= 2,
	Status	= 3
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
	CCamera *m_pCamera = NULL;
};

class CUIObject : public CBillboardObject
{
public:
	CUIObject(CCreateMgr *pCreateMgr);
	CUIObject(CCreateMgr *pCreateMgr, Type type);
	virtual ~CUIObject();

	virtual void SetPos(XMFLOAT3 xmf3Position);
	virtual void SetDistance(float x) {
		m_xmf4x4World = Matrix4x4::Identity();

		m_fDistance = x;
	}

	virtual void Animate(float fTimeElapsed);

private:
	XMFLOAT3	m_xmf3Position;
	float		m_fDistance;
	Type		m_type;
};
