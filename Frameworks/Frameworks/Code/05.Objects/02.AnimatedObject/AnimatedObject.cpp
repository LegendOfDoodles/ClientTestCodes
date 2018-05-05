#include "stdafx.h"
#include "AnimatedObject.h"
#include "06.Meshes/00.Vertex/Vertex.h"
/// <summary>
/// 목적: 움직이는 오브젝트 처리용 기본 클래스
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-05-04
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CAnimatedObject::CAnimatedObject(CCreateMgr * pCreateMgr, int nMeshes) : CBaseObject(pCreateMgr, nMeshes)
{
	m_fFrameTime = 0;
	m_nAniState = 0;
}

CAnimatedObject::~CAnimatedObject()
{
	if (m_pathToGo) Safe_Delete(m_pathToGo);
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CAnimatedObject::Animate(float timeElapsed)
{

	m_fFrameTime += 30 * timeElapsed;
	if (m_fFrameTime > m_nAniLength[m_nAniState]) {
		while (m_fFrameTime > m_nAniLength[m_nAniState])
			m_fFrameTime -= m_nAniLength[m_nAniState];
	}

	int Bcnt = m_pSkeleton[m_nAniState].GetBoneCount(); 

	for (int i = 0; i < Bcnt; ++i) {
		m_xmf4x4Frame[i] = m_pSkeleton[m_nAniState].GetBone(i).GetFrame((int)m_fFrameTime);
	}
}

void CAnimatedObject::Render(CCamera * pCamera, UINT instanceCnt)
{
	OnPrepareRender();

	if (m_pMaterial)
	{
		m_pMaterial->Render(pCamera);
		m_pMaterial->UpdateShaderVariables();
	}

	if (m_cbvGPUDescriptorHandle.ptr)
		m_pCommandList->SetGraphicsRootDescriptorTable(7, m_cbvGPUDescriptorHandle);

	if (m_pShader)
	{
		UpdateShaderVariables();
		m_pShader->Render(pCamera);
	}

	if (m_ppMeshes)
	{
		for (int i = 0; i < m_nMeshes; i++)
		{
			if (m_ppMeshes[i]) m_ppMeshes[i]->Render(instanceCnt);
		}
	}
}

void CAnimatedObject::SetSkeleton(CSkeleton *skeleton)
{
	m_nAniLength[m_nAniCnt] = skeleton->GetAnimationLength();
	m_pSkeleton[m_nAniCnt++] = *skeleton;
}

void CAnimatedObject::AniStateSet() 
{
	++m_nAniState;
	m_fFrameTime = 0;
	if (m_nAniState >= m_nAniCnt) { m_nAniState = 0; }
}

void CAnimatedObject::MinionTypeSet()
{
	++m_nAniState;
	m_fFrameTime = 0;
	if (m_nAniState >= m_nAniCnt) { m_nAniState = 0; }
}

void CAnimatedObject::MoveUp(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Up = GetUp();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Up, fDistance);
	CBaseObject::SetPosition(xmf3Position);
}

void CAnimatedObject::MoveForward(float fDistance)
{
	XMFLOAT3 xmf3Position = GetPosition();
	XMFLOAT3 xmf3Look = GetLook();
	xmf3Position = Vector3::Add(xmf3Position, xmf3Look, fDistance);
	CBaseObject::SetPosition(xmf3Position);
}

void CAnimatedObject::LookAt(XMFLOAT3 objPosition)
{
	XMFLOAT3 upVector{ 0.f, 1.f, 0.f };
	XMFLOAT3 playerLook = GetLook();
	XMFLOAT3 towardVector = Vector3::Normalize(Vector3::Subtract(objPosition, GetPosition()));

	float angle{ Vector3::DotProduct(towardVector, playerLook) };
	angle = XMConvertToDegrees(acos(angle));

	if (isnan(angle))
		return;

	float check{ Vector3::DotProduct(Vector3::CrossProduct(towardVector, playerLook), upVector) };

	// 캐릭터가 선택된 오브젝트 보다 오른쪽 보고 있는 경우
	if (check < 0.0f)
		Rotate(0.0f, 0.0f, -angle);
	else if (check > 0.0f)
		Rotate(0.0f, 0.0f, angle);
}

void CAnimatedObject::LookAt(XMFLOAT2 objPosition)
{
	LookAt(XMFLOAT3(objPosition.x, 0, objPosition.y));
}

XMFLOAT3 CAnimatedObject::GetLook()
{
	return(Vector3::ScalarProduct(XMFLOAT3(m_xmf4x4World._21, m_xmf4x4World._22, m_xmf4x4World._23), -1));
}

XMFLOAT3 CAnimatedObject::GetUp()
{
	return(Vector3::Normalize(XMFLOAT3(m_xmf4x4World._31, m_xmf4x4World._32, m_xmf4x4World._33)));
}

void CAnimatedObject::SetPathToGo(Path * path)
{
	if (m_pathToGo)
	{
		m_pathToGo->clear();
		Safe_Delete(m_pathToGo);
	}
	m_pathToGo = path;
	ResetDestination();
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
bool CAnimatedObject::IsArrive(XMFLOAT2 & nextPos, float dst)
{
	XMFLOAT2 curPos{ GetPosition().x, GetPosition().z };
	int distanceSqr = Vector2::DistanceSquare(curPos, nextPos);

	return distanceSqr < dst * dst;
}

void CAnimatedObject::MoveToDestination(float timeElapsed)
{
	if (!m_pathToGo) return;

	if (NoneDestination() || IsArrive(m_destination, m_speed * timeElapsed))	//  도착 한 경우
	{
		if (m_pathToGo->empty())
		{
			Safe_Delete(m_pathToGo);
			ResetDestination();
		}
		else
		{
			m_destination = m_pathToGo->front().To();
			m_pathToGo->pop_front();
			LookAt(m_destination);
		}
	}
	else  // 아직 도착하지 않은 경우
	{
		MoveForward(m_speed * timeElapsed);
		XMFLOAT3 position = GetPosition();
		position.y = m_pTerrain->GetHeight(position.x, position.z);
		CBaseObject::SetPosition(position);
		LookAt(m_destination);
	}
}
