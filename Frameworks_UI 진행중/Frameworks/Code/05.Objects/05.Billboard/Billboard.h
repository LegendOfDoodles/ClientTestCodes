#pragma once

#include "stdafx.h"
#include "Billboard.h"
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "06.Meshes/00.Vertex/Vertex.h"

/// <summary>
/// ����: UI �̿��� ���� Bilboard Ŭ���� ����
/// ���� ������:  �̿뼱
/// ������ ���:  �̿뼱
/// ���� ���� ��¥: 2018-04-04
/// </summary>

enum Type {
	Minimap = 0,
	KDA		= 1,
	Skill	= 2,
	Status	= 3
};

class CBillboardObject : public CBaseObject
{
public:	// ������, �Ҹ���
	CBillboardObject(CCreateMgr *pCreateMgr);
	virtual ~CBillboardObject();

public: // �����Լ�
	virtual void Animate(float fTimeElapsed);

	void SetLookAt();
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; };

protected: // ���� �Լ�

private: // ����

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
