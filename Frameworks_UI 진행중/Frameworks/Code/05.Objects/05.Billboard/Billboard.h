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

class CBillboardObject : public CBaseObject
{
public:	// 생성자, 소멸자
	CBillboardObject(CCreateMgr *pCreateMgr);
	virtual ~CBillboardObject();

public: // 공개함수
	virtual void Animate(float fTimeElapsed);

	void SetLookAt(XMFLOAT3& xmf3Target);
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; };

protected: // 내부 함수

private: // 변수

protected:
	CCamera *m_pCamera = NULL;
};

class CMinimap : public CBillboardObject
{
public:
	CMinimap(CCreateMgr *pCreateMgr);
	virtual ~CMinimap();

	virtual void SetPos(XMFLOAT3 xmf3Position);
	virtual void SetDistance(float x) {
		m_xmf4x4World = Matrix4x4::Identity();

		m_fDistance = x;
	}

	virtual void Animate(float fTimeElapsed);
	virtual void SetCamera(CCamera *pCamera);

private:
	XMFLOAT3	m_xmf3Position;
	float		m_fDistance;
};