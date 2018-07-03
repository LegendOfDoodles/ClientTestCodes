#pragma once
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"

class CLightCamera : public CCamera
{
public:	// ������, �Ҹ���
	CLightCamera(XMFLOAT3 lightDir);
	virtual ~CLightCamera();

public:	// ���� �Լ�
	virtual void Initialize(CCreateMgr *pCreateMgr) override;

	virtual void UpdateShaderVariables();

	virtual bool IsInFrustum(BoundingOrientedBox& xmBoundingBox) { xmBoundingBox; return true; }

protected: // ���� �Լ�

protected: // ����
	XMFLOAT3 m_lightDirection{ 0, 0, 0 };
};

