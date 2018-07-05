#pragma once

#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "05.Objects/01.Camera/00.BaseCamera/Camera.h"
#include "05.Objects/01.Camera/01.AOSCamera/AOSCamera.h"

struct CB_GAUGE_INFO
{
	XMFLOAT4X4	m_xmf4x4World;
	float		m_fCurrentHP;
};

enum UIFrameType {
	MinimapFrame	,
	KDAFrame		,
	TimerFrame		,
	SkillFrameQ		,
	SkillFrameW		,
	SkillFrameE		,
	SkillFrameR		,
	StatusFrame		,
	CharacterFrame	,
	SpecialFrame	,
	CharacterFrameHP,
	CharacterFrameMP,
	Minimap,
};

enum NumberType {
	BlueTeamKill,
	RedTeamKill,
	TimeMinute,
	TimeSec,
	PersonalKill,
	PersonalDeath,
	PersonalAssist,
};

enum SkillUIType {
	GreyQSkill,
	GreyWSkill,
	GreyESkill,
	GreyRSkill,
	QSkill,
	WSkill,
	ESkill,
	RSkill
};

enum GagueUIType {
	PlayerGauge,
	MinionGauge,
	NexusAndTower,
};

enum IconUIType {
	PlayerIcon,
	MinionIcon,
	NexusAndTowerIcon
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

protected: // ����
	CCamera * m_pCamera{ NULL };
};