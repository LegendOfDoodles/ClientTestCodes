#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"

class CPlayer : public CAnimatedObject
{
public:
	CPlayer(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes = 1);
	virtual ~CPlayer();

public:	// �ܺ� �Լ�
	virtual void Animate(float timeElapsed);
	virtual void Render(CCamera *pCamera, UINT instanceCnt = 1);

	virtual void LookAt(XMFLOAT3 objPosition);
	virtual void LookAt(XMFLOAT2 objPosition);

	virtual ProcessType MoveToDestination(float timeElapsed, shared_ptr<CWayFinder> pWayFinder = NULL);
	virtual void MoveToSubDestination(float timeElapsed, shared_ptr<CWayFinder> pWayFinder = NULL);

	virtual void ActiveSkill(AnimationsType act);

	virtual void SetState(StatesType newState);

	virtual void ChangeSkillSet(CSkeleton** ppskill);

	void SaveCurrentState();
	void Respawn();

	virtual void ReceiveDamage(float damage);

	virtual PlayerInfo* GetPlayerStatus() { return &m_StatusInfo; }

	void SetWeaponChangeTriger(bool triger) { m_ChangeWeapon = triger; }
	bool GetWeaponChangeTriger() { return m_ChangeWeapon; }


	UINT GetWeaponType() { return m_StatusInfo.Weapon; }
	UINT GetWeaponNum() { return m_StatusInfo.WeaponNum; }

	UINT GetSpecialCnt() { return m_StatusInfo.SpecialPoint; }
	SpecialType* GetSpecialIndext() { return m_StatusInfo.Special;}
	UINT GetEquipCnt() { return EquipCnt; }

	UINT* GetEquipIndex() { return m_nEquipIndex; }


	void AddEquipCnt(UINT equiptype, UINT specialnum) {
		m_nEquipIndex[EquipCnt++] = equiptype * 4 + specialnum;
		//0123 ��� 4567 Ư��
	}
	
	void SetWeaponData(UINT type , UINT num){
		m_StatusInfo.Weapon = type;
		m_StatusInfo.WeaponNum = num;
	}
protected: // ���� �Լ�
	virtual void AdjustAnimationIndex();

	virtual bool Heal(float timeElapsed);
	/*
	0. Win		1.Defeat		2.Defeat
	3. Idle		4.StartWalk		5.Walking	
	6.Smash		7.Slash		8.Dash		9.Dispute 
	*/

protected: // ����
	PlayerInfo m_StatusInfo;
	bool m_ChangeWeapon{ false };

	UINT m_nEquipIndex[4];
	UINT EquipCnt{ 0 };

	
	XMFLOAT4X4 m_xmf4x4SpawnWorld;	// ������ ���� ��ȯ ���
	XMFLOAT3 m_spawnLocation;	// ���� ��ġ

	float m_BattleDelay{ 0 };
	float m_spawnCoolTime{ 0 };	// ���� ���� �ٽ� ������ �� ���� �ð�
};

