#pragma once
#include "Player.h"

class CCreateMgr;

class CAirplanePlayer : public CPlayer
{
public:	// ������, �Ҹ���
	CAirplanePlayer(CCreateMgr *pCreateMgr);
	virtual ~CAirplanePlayer();

public:	// �����Լ�
	virtual CCamera *ChangeCamera(CCreateMgr *pCreateMgr, DWORD nNewCameraMode, float fTimeElapsed);

protected: // ���� �Լ�
	virtual void OnPrepareRender();

protected: // ����
};