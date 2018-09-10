#include "stdafx.h"
#include "Player.h"
#include "00.Global/01.Utility/04.WayFinder/WayFinder.h"

/// <summary>
/// ����: �÷��̾� ���� Ŭ����
/// ���� ������:  �質��
/// ������ ���:  ������, �質��
/// ���� ���� ��¥: 2018-08-31
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CPlayer::CPlayer(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes) : CAnimatedObject(pCreateMgr, nMeshes)
{
	m_detectRange = CONVERT_PaperUnit_to_InG(80.0f);
	m_sightRange = CONVERT_PaperUnit_to_InG(80.0f);
	SetSpeed(CONVERT_cm_to_InG(3.285f));
}


CPlayer::~CPlayer()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CPlayer::Animate(float timeElapsed)
{
	switch (m_curState) {
	case States::Idle:
		if (m_nCurrAnimation != Animations::Idle) m_nCurrAnimation = Animations::Idle;
		if (m_mainPath) SetState(States::Walk);
		break;
	case States::Attack:
		if (GetType() == ObjectType::SwordPlayer)
		{
			if (m_nCurrAnimation == Animations::Attack1 &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pColManager->RequestCollide(CollisionType::SPHERE, this, CONVERT_PaperUnit_to_InG(5), CONVERT_PaperUnit_to_InG(4), m_StatusInfo.Atk);

				//// EffectMgr
				m_pSoundMgr->play(SOUND::Player_Sword_Attack_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillQ &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				// CoolTime �ʱ�ȭ
				m_StatusInfo.QSkillCoolTime = 0.0f;

				m_pColManager->RequestCollide(CollisionType::SPHERE, this, CONVERT_PaperUnit_to_InG(8), CONVERT_PaperUnit_to_InG(8), 100);

				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_SwordSkill_Q_Effect);
				m_pSoundMgr->play(SOUND::Player_Sword_Q_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillW &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) 
			{
				m_StatusInfo.WSkillCoolTime = 0.0f;

				m_pColManager->RequestCollide(CollisionType::SECTERFORM, this, CONVERT_PaperUnit_to_InG(24), 180, 500);

				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_SwordSkill_W_Effect);
				m_pSoundMgr->play(SOUND::Player_Sword_W_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillE &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) 
			{
				m_StatusInfo.ESkillCoolTime = 0.0f;

				m_pColManager->RequestCollide(CollisionType::SECTERFORM, this, CONVERT_PaperUnit_to_InG(16), 150, 100);

				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_SwordSkill_E_Effect);
				m_pSoundMgr->play(SOUND::Player_Sword_E_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillR &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.666f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.666f)
			{
				m_StatusInfo.RSkillCoolTime = 0.0f;

				m_pColManager->RequestCollide(CollisionType::SPHERE, this, 0, CONVERT_PaperUnit_to_InG(12), 500);

				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_SwordSkill_R_Effect);
				m_pSoundMgr->play(SOUND::Player_Sword_R_Sound, GetPosition());
			}
		}
		else if (GetType() == ObjectType::StaffPlayer)
		{
			if (m_nCurrAnimation == Animations::Attack1 &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_Magic, 100);
				
				m_pSoundMgr->play(SOUND::Player_Staff_Attack_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillQ &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) 
			{
				// CoolTime �ʱ�ȭ
				m_StatusInfo.QSkillCoolTime = 0.0f;
				
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_MagicSkill_Q, 100);

				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_StaffQSkill_Effect);
				m_pSoundMgr->play(SOUND::Player_Staff_Q_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillW &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) 
			{
				m_StatusInfo.WSkillCoolTime = 0.0f;
				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_StaffAttack_Effect);
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_StaffSkill_W_Effect);
				m_pSoundMgr->play(SOUND::Player_Staff_W_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillE &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) 
			{
				m_StatusInfo.ESkillCoolTime = 0.0f;

				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_StaffESkill_Effect);
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_StaffSkill_E_Effect);
				m_pSoundMgr->play(SOUND::Player_Staff_E_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillR &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.666f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.666f) 
			{
				m_StatusInfo.RSkillCoolTime = 0.0f;

				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_MagicSkill_R, 100);

				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_StaffAttack_Effect);
			}
		}
		else if (GetType() == ObjectType::BowPlayer)
		{
			if (m_nCurrAnimation == Animations::Attack1 &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_Arrow, 100);
				
				m_pSoundMgr->play(SOUND::Player_Arrow_Attack_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillQ &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) 
			{
				// CoolTime �ʱ�ȭ
				m_StatusInfo.QSkillCoolTime = 0.0f;

				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_ArrowSkill_Q, 100);

				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_ArrowAttack_Effect);
				m_pSoundMgr->play(SOUND::Player_Arrow_Q_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillW &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f) 
			{
				m_StatusInfo.WSkillCoolTime = 0.0f;

				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_ArrowSkill_W, 100);

				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_ArrowAttack_Effect);
				m_pSoundMgr->play(SOUND::Player_Arrow_W_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillE && 
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.5f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.5f)
			{
				m_StatusInfo.ESkillCoolTime = 0.0f;

				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_ArrowSkill_E, 100);

				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_ArrowAttack_Effect);
				m_pSoundMgr->play(SOUND::Player_Arrow_E_Sound, GetPosition());
			}
			else if (m_nCurrAnimation == Animations::SkillR &&
				m_fFrameTime >= m_nAniLength[m_nAniIndex] * 0.666f &&
				m_fPreFrameTime < m_nAniLength[m_nAniIndex] * 0.666f) 
			{
				m_StatusInfo.RSkillCoolTime = 0.0f;

				m_pThrowingMgr->RequestSpawn(GetPosition(), GetLook(), m_TeamType, FlyingObjectType::Player_ArrowSkill_R, 100);

				// EffectMgr
				m_pEffectMgr->RequestSpawn(GetPosition(), GetLook(), m_nAniLength[m_nAniIndex], EffectObjectType::Player_ArrowAttack_Effect);
				m_pSoundMgr->play(SOUND::Player_Arrow_R_Sound, GetPosition());
			}
		}
		m_fPreFrameTime = m_fFrameTime;
		m_fFrameTime += ANIMATION_SPEED * timeElapsed * m_StatusInfo.AtkSpeed;

		if (m_fFrameTime >= m_nAniLength[m_nAniIndex] - 1)
		{
			SetState(States::Idle);
		}
		break;

	case States::Walk:
		if (m_nCurrAnimation != Animations::StartWalk&&
			m_nCurrAnimation != Animations::Walking)
			m_nCurrAnimation = Animations::StartWalk;

		if (m_nCurrAnimation == Animations::StartWalk) {
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] - 1)
			{
				m_nCurrAnimation = Animations::Walking;
				m_fFrameTime = 0;
			}
		}
		m_fPreFrameTime = m_fFrameTime;
		m_fFrameTime += ANIMATION_SPEED * timeElapsed * m_StatusInfo.WalkSpeed;
		break;
	case States::Die:
		if (GetAnimTimeRemainRatio() < 0.05)
		{
			SetState(States::Remove);
		}
		break;
	case States::Win:
		if (m_nCurrAnimation != Animations::Win) m_nCurrAnimation = Animations::Win;
		break;
	case States::Defeat:
		if (m_nCurrAnimation != Animations::Defeat&&
			m_nCurrAnimation != Animations::Defeat2)
			m_nCurrAnimation = Animations::Defeat;

		if (m_nCurrAnimation == Animations::Defeat) {
			if (m_fFrameTime >= m_nAniLength[m_nAniIndex] - 1)
			{
				m_nCurrAnimation = Animations::Defeat2;
				m_fFrameTime = 0;
			}
		}
		break;
	case States::Remove:
		m_spawnCoolTime -= timeElapsed;
		if (m_spawnCoolTime < 0.0f)
		{
			Respawn();
		}
		break;
	default:
		break;
	}

	if (m_curState != States::Remove &&
		m_curState != States::Walk &&
		m_curState != States::Attack)
	{
		m_fPreFrameTime = m_fFrameTime;
		m_fFrameTime += ANIMATION_SPEED * timeElapsed;
	}

	m_StatusInfo.QSkillCoolTime = min(m_StatusInfo.QSkillCoolTime += timeElapsed * 0.1f, 1.f);
	m_StatusInfo.WSkillCoolTime = min(m_StatusInfo.WSkillCoolTime += timeElapsed * 0.1f, 1.f);
	m_StatusInfo.ESkillCoolTime = min(m_StatusInfo.ESkillCoolTime += timeElapsed * 0.1f, 1.f);
	m_StatusInfo.RSkillCoolTime = min(m_StatusInfo.RSkillCoolTime += timeElapsed * 0.1f, 1.f);


	AdjustAnimationIndex();

	if (m_fFrameTime > m_nAniLength[m_nAniIndex]) {
		while (m_fFrameTime > m_nAniLength[m_nAniIndex])
			m_fFrameTime -= m_nAniLength[m_nAniIndex];
	}

	if (MoveToDestination(timeElapsed) == States::Done) SetState(States::Idle);

	m_BattleDelay += timeElapsed;
	if (m_BattleDelay >= 10.0f&&m_StatusInfo.HP<m_StatusInfo.maxHP) {
		m_StatusInfo.HP = min(m_StatusInfo.HP + 50.0f * timeElapsed,m_StatusInfo.maxHP);
	}
	CAnimatedObject::Animate(timeElapsed);
}

void CPlayer::Render(CCamera * pCamera, UINT instanceCnt)
{
	OnPrepareRender();
	
	if (!IsVisible(pCamera) || !m_Detected) return;
	
	if (m_pMaterial)
	{
		m_pMaterial->Render(pCamera);
		m_pMaterial->UpdateShaderVariables();
	}
	
	if (m_cbvGPUDescriptorHandle.ptr)
		m_pCommandList->SetGraphicsRootDescriptorTable(6, m_cbvGPUDescriptorHandle);
	
	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(instanceCnt);
		}
	}
}

void CPlayer::LookAt(XMFLOAT3 objPosition)
{
	if (m_curState == States::Attack) return;
	CAnimatedObject::LookAt(objPosition);
}

void CPlayer::LookAt(XMFLOAT2 objPosition)
{
	if (m_curState == States::Attack) return;
	CAnimatedObject::LookAt(objPosition);
}

ProcessType CPlayer::MoveToDestination(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	if (m_curState != States::Walk) return States::Processing;
	if (!m_mainPath) return States::Done;

	if (NoneDestination() || IsArrive(m_speed * timeElapsed * m_StatusInfo.WalkSpeed))	//  ���� �� ���
	{
		if (m_mainPath->empty())
		{
			Safe_Delete(m_mainPath);
			ResetDestination();
			return States::Done;
		}
		else
		{
			m_destination = m_mainPath->front().To();
			m_mainPath->pop_front();
			LookAt(m_destination);
		}
	}
	else  // ���� �������� ���� ���
	{
		MoveForward(m_speed * timeElapsed * m_StatusInfo.WalkSpeed);
		XMFLOAT3 position = GetPosition();
		position.y = m_pTerrain->GetHeight(position.x, position.z);
		CBaseObject::SetPosition(position);
		CheckRightWay(PathType::Main, pWayFinder);
	}
	return States::Processing;
}

void CPlayer::MoveToSubDestination(float timeElapsed, shared_ptr<CWayFinder> pWayFinder)
{
	if (pWayFinder != NULL)
	{
		m_availableTime -= timeElapsed;
		if (m_availableTime <= 0.0f)
		{
			XMFLOAT3 myPos{ GetPosition() };
			XMFLOAT3 enemyPos{ m_pEnemy->GetPosition() };
			m_availableTime = TIME_AVAILABILITY_CHECK;
			ResetSubPath();
			m_subPath = pWayFinder->GetPathToPosition(
				myPos,
				enemyPos);
			m_subPath->push_back(CPathEdge(XMFLOAT2(enemyPos.x, enemyPos.z), Vector3::ToVector2(Vector3::Add(enemyPos, Vector3::Subtract(enemyPos, myPos)))));
		}
	}

	if (NoneDestination(PathType::Sub) || IsArrive(m_speed * timeElapsed * m_StatusInfo.WalkSpeed, PathType::Sub))	//  ���� �� ���
	{
		if (m_subPath == NULL || m_subPath->empty())
		{
			Safe_Delete(m_subPath);
			ResetDestination(PathType::Sub);
			LookAt(m_destination);
		}
		else
		{
			m_subDestination = m_subPath->front().To();
			m_subPath->pop_front();
			LookAt(m_subDestination);
		}
	}

	MoveForward(m_speed * timeElapsed * m_StatusInfo.WalkSpeed);
	XMFLOAT3 position = GetPosition();
	position.y = m_pTerrain->GetHeight(position.x, position.z);
	CBaseObject::SetPosition(position);
	CheckRightWay(PathType::Sub, pWayFinder);
}

void CPlayer::ActiveSkill(AnimationsType act)
{
	if (m_curState != States::Attack) {
		m_curState = States::Attack;
		m_nCurrAnimation = act;
		m_fFrameTime = 0;
		m_BattleDelay = 0;
	}
}

void CPlayer::SetState(StatesType newState)
{
	m_curState = newState;

	switch (newState)
	{
	case States::Idle:
		m_nCurrAnimation = Animations::Idle;
		break;
	case States::Walk:
		RegenerateLookAt();
		if (m_nCurrAnimation != Animations::StartWalk &&
			m_nCurrAnimation != Animations::Walking)
			m_nCurrAnimation = Animations::StartWalk;
		break;
	case States::Chase:
		m_nCurrAnimation = Animations::StartWalk;
		break;
	case States::Attack:
		m_fFrameTime = 0;
		break;
	case States::Die:
		m_nCurrAnimation = Animations::Die;
		m_fFrameTime = 0;
		m_spawnCoolTime = COOLTIME_SPAWN_PLAYER + static_cast<float>(m_StatusInfo.Level);
		SetPathToGo(NULL);
		break;
	case States::Remove:
		break;
	case States::Win:
		m_nCurrAnimation = Animations::Win;
		m_fFrameTime = 0;
		SetPathToGo(NULL);
		break;
	case States::Defeat:
		m_nCurrAnimation = Animations::Defeat;
		m_fFrameTime = 0;
		SetPathToGo(NULL);
		break;
	default:
		assert(!"Error:: There is No State");
	}
}

void CPlayer::ChangeSkillSet(CSkeleton ** ppskill)
{
	for (int j = 0; j < 8; j++)
	{
		m_nAniLength[j+4] = ppskill[j]->GetAnimationLength();
		m_pSkeleton[j+4] = *ppskill[j];
	}
}

void CPlayer::SaveCurrentState()
{
	m_xmf4x4SpawnWorld = m_xmf4x4World;
	m_spawnLocation = GetPosition();
}

void CPlayer::Respawn()
{
	SetState(StatesType::Idle);

	m_StatusInfo.HP = m_StatusInfo.maxHP;

	m_pColManager->AddCollider(this);

	m_xmf4x4World = m_xmf4x4SpawnWorld;
}



////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CPlayer::AdjustAnimationIndex()
{
	switch (m_nCurrAnimation)
	{
	case Animations::Win:
		m_nAniIndex = 0;
		break;
	case Animations::Defeat:
		m_nAniIndex = 1;
		break;
	case Animations::Defeat2:
		m_nAniIndex = 2;
		break;
	case Animations::Die:
		m_nAniIndex = 3;
		break;
	case Animations::Idle:
		m_nAniIndex = 4;
		break;
	case Animations::StartWalk:
		m_nAniIndex = 5;
		break;
	case Animations::Walking:
		m_nAniIndex = 6;
		break;
	case Animations::SkillQ:
		m_nAniIndex = 7;
		break;
	case Animations::SkillW:
		m_nAniIndex = 8;
		break;
	case Animations::SkillE:
		m_nAniIndex = 9;
		break;
	case Animations::SkillR:
		m_nAniIndex = 10;
		break;
	case Animations::Attack1:
		m_nAniIndex = 11;
		break;
	}
}