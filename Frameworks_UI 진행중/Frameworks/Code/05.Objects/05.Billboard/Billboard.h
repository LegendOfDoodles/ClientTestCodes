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

class CBillboardObject : public CBaseObject
{
public:	// ������, �Ҹ���
	CBillboardObject(CCreateMgr *pCreateMgr);
	virtual ~CBillboardObject();

public: // �����Լ�
	virtual void Animate(float fTimeElapsed);

	void SetLookAt(XMFLOAT3& xmf3Target);
	void SetCamera(CCamera *pCamera) { m_pCamera = pCamera; };

protected: // ���� �Լ�

private: // ����

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