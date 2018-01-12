#pragma once
#include "Mesh.h"

class CShader;
class CCamera;

class CBaseObject
{
public:	// ������, �Ҹ���
	CBaseObject();
	~CBaseObject();

public: // ���� �Լ�
	void ReleaseUploadBuffers();

	virtual void SetMesh(CMesh *pMesh);
	virtual void SetShader(CShader *pShader);

	virtual void Animate(float timeElapsed);

	virtual void OnPrepareRender();
	virtual void Render(CCamera *pCamera);

	void Rotate(XMFLOAT3 *pxmf3Axis, float fAngle);

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected: // ���� �Լ�

protected: // ����
	int m_nReferences = 0;

	XMFLOAT4X4 m_xmf4x4World;
	CMesh *m_pMesh = NULL;

	CShader *m_pShader = NULL;
};

