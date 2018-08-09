#include "stdafx.h"
#include "EffectObject.h"
#include "00.Global/01.Utility/05.CollisionManager/CollisionManager.h"

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CEffectObject::CEffectObject(shared_ptr<CCreateMgr> pCreateMgr, int nMeshes)
	: CCollisionObject(pCreateMgr)
{
	m_ObjectType = ObjectType::EffectObject;

	// HP게이지 Mesh
	CTexturedRectMesh *pRectMesh = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 12.8f, FRAME_BUFFER_HEIGHT / 7.2f, 0.f);
	SetMesh(0, pRectMesh);
}

CEffectObject::~CEffectObject()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CEffectObject::Animate(float timeElapsed)
{
	if (!m_Activated) return;

	// Billboard Update
	//XMFLOAT3 xmf3Up = m_pCamera->GetUpVector();
	//XMFLOAT3 xmf3Position(m_xmf4x4World._41, m_xmf4x4World._42, m_xmf4x4World._43);
	//XMFLOAT3 xmf3Look = Vector3::ScalarProduct(m_pCamera->GetLookVector(), -1);
	//XMFLOAT3 xmf3Right = Vector3::ScalarProduct(m_pCamera->GetRightVector(), -1);
	//
	//m_xmf4x4World._11 = xmf3Right.x;	m_xmf4x4World._12 = xmf3Right.y;	m_xmf4x4World._13 = xmf3Right.z;
	//m_xmf4x4World._21 = xmf3Up.x;		m_xmf4x4World._22 = xmf3Up.y;		m_xmf4x4World._23 = xmf3Up.z;
	//m_xmf4x4World._31 = xmf3Look.x;		m_xmf4x4World._32 = xmf3Look.y;
	
	// Effect Update
	if (m_EffectObjectType == EffectObjectType::Player_SwordSkill_Q)
	{
		m_AnimaitonTime += timeElapsed * 30;
		if (m_AnimaitonTime > m_maxAnimaitonTime / 2)
		{
			m_curState = StatesType::Remove;
		}
	}
	if (m_EffectObjectType == EffectObjectType::Player_SwordSkill_W)
	{
		m_AnimaitonTime += timeElapsed * 30;
		if (m_AnimaitonTime > m_maxAnimaitonTime / 2)
		{
			m_curState = StatesType::Remove;
		}
	}
	if (m_EffectObjectType == EffectObjectType::Minion_Magic_Ball)
	{
		m_AnimaitonTime += timeElapsed * 30;
		if (m_AnimaitonTime > m_maxAnimaitonTime / 2)
		{
			m_curState = StatesType::Remove;
		}
	}
	if (m_EffectObjectType == EffectObjectType::Player_BowSkill_Effect)
	{
		m_AnimaitonTime += timeElapsed * 30;
		if (m_AnimaitonTime > m_maxAnimaitonTime / 2)
		{
			m_curState = StatesType::Remove;
		}
	}

	// 오브젝트와 같이 이동해야하는 오브젝트
	//if ( type )		// type 주고
	//{
	//	m_distance += timeElapsed * m_speed;	// speed 정해주고
	//	MoveToDirection(timeElapsed * m_speed); // 이동 시켜준다.
	//	if (m_distance > m_maxAnimaitonTime)
	//	{
	//		m_curState = StatesType::Remove;
	//	}
	//}
}

void CEffectObject::Render(CCamera * pCamera, UINT istanceCnt)
{
	OnPrepareRender();
	//if (!IsVisible(pCamera) || !m_pMasterObject->GetDetected()) return;

	if (m_pMaterial)
	{
		m_pMaterial->Render(pCamera);
		m_pMaterial->UpdateShaderVariables();
	}

	if (m_cbvGPUDescriptorHandle.ptr)
		m_pCommandList->SetGraphicsRootDescriptorTable(10, m_cbvGPUDescriptorHandle);

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(istanceCnt);
		}
	}
}

void CEffectObject::SetDirection(const XMFLOAT3 & direction)
{
	m_direction = direction;
	LookAt(Vector3::Add(GetPosition(), Vector3::ScalarProduct(direction, 10000, false)));
}

void CEffectObject::SetEffectObjectsType(EffectObjectType type)
{
	m_EffectObjectType = type;
	m_curState = StatesType::Idle;
	
	m_AnimaitonTime = 0.0f;

	//if (type == EffectObjectType::Player_SwordSkill_Q)
	//{
	//	m_AnimaitonTime = 0.0f;
	//}
	//else if (type == EffectObjectType::Player_SwordSkill_W)
	//{
	//	m_AnimaitonTime = 0.0f;
	//}
	//else if (type == EffectObjectType::Minion_Magic_Ball) {
	//	m_AnimaitonTime = 0.0f;
	//}
	//else if (type == EffectObjectType::Player_BowSkill_Effect) {
	//	m_AnimaitonTime = 0.0f;
	//}

	// 필요하면 여기서 speed 추가해준다.
}

void CEffectObject::LookAt(XMFLOAT3 target)
{
	if (m_curState == States::Attack) return;
	if (m_curState == States::Win) return;
	if (m_curState == States::Defeat) return;

	XMFLOAT3 upVector{ 0.f, 1.f, 0.f };
	XMFLOAT3 objLookVector = GetLook();
	XMFLOAT3 objPos = GetPosition();

	target.y = objPos.y;

	XMFLOAT3 towardVector = Vector3::Subtract(GetPosition(), target, true);

	float angle{ Vector3::DotProduct(towardVector, objLookVector) };
	angle = XMConvertToDegrees(acos(angle));

	if (isnan(angle)) return;

	float check{ Vector3::DotProduct(Vector3::CrossProduct(towardVector, objLookVector), upVector) };

	// 회전 방향 결정
	if (check < 0.0f)
		Rotate(-90.0f, angle + 180.f, 0.0f);
	else if (check > 0.0f)
		Rotate(-90.0f, -angle - 180.f, 0.0f);
}

void CEffectObject::LookAt(XMFLOAT2 target)
{
	LookAt(XMFLOAT3(target.x, 0, target.y));
}

void CEffectObject::MoveToDirection(float dist)
{
	CBaseObject::SetPosition(Vector3::Add(GetPosition(), m_direction, dist));
}
