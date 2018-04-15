#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"
#include "05.Objects/97.Skeleton/Skeleton.h"
class CRotatingObject : public CBaseObject
{
public: // ������, �Ҹ���
	CRotatingObject(CCreateMgr *pCreateMgr);
	virtual ~CRotatingObject();

public: // ���� �Լ�
	virtual void Animate(float timeElapsed);

	void SetRotationSpeed(float fRotationSpeed) { m_rotationSpeed = fRotationSpeed; }
	void SetRotationAxis(XMFLOAT3 xmf3RotationAxis) { m_rotationAxis = xmf3RotationAxis; }

protected: // ���� �Լ�

private:	 // ����
	XMFLOAT3 m_rotationAxis;
	float m_rotationSpeed;
};



class CAnimatedObject : public CBaseObject 
{
public:
	CAnimatedObject(CCreateMgr *pCreateMgr);
	CAnimatedObject(CCreateMgr *pCreateMgr, int nMeshes);
	virtual ~CAnimatedObject();

public:
	virtual void Animate(float timeElapsed);
	virtual void Render(CCamera *pCamera, UINT instanceCnt = 1);

	void SetSkeleton(CSkeleton *skeleton) {
		m_pSkeleton = skeleton;
		//m_pSkeleton->GerAnimationLength;
	}
	void SetSkeleton1(CSkeleton *skeleton) { m_pSkeleton1 = skeleton; }
	void SetSkeleton2(CSkeleton *skeleton) { m_pSkeleton2 = skeleton; }


	void AniStateSet() { 
		++aniState;
		m_fFrameTime = 0;
		if (aniState >= 3) { aniState = 0; }
	}

protected:
	CSkeleton*	m_pSkeleton{ NULL };
	CSkeleton*	m_pSkeleton1{ NULL };
	CSkeleton*	m_pSkeleton2{ NULL };

	int aniState{ 0 };
	float m_fFrameTime{ 0 };
};

