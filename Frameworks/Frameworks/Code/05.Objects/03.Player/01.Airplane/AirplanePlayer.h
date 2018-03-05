#pragma once
#include "05.Objects/03.Player/00.BasePlayer/Player.h"

class CCreateMgr;

class CAirplanePlayer : public CPlayer
{
public:	// 생성자, 소멸자
	CAirplanePlayer(CCreateMgr *pCreateMgr);
	virtual ~CAirplanePlayer();

public:	// 공개함수
	virtual CCamera *ChangeCamera(CCreateMgr *pCreateMgr, DWORD nNewCameraMode, float fTimeElapsed);

protected: // 내부 함수
	virtual void OnPrepareRender();

protected: // 변수
};