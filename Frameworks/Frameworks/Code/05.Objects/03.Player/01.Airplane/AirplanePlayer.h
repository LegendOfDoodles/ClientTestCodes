#pragma once
#include "05.Objects/03.Player/00.BasePlayer/Player.h"

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