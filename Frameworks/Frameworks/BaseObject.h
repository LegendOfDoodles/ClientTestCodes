#pragma once
#include "Mesh.h"

class CShader;

class CBaseObject
{
public:	// 생성자, 소멸자
	CBaseObject();
	~CBaseObject();

public: // 공개 함수
	void ReleaseUploadBuffers();

	virtual void SetMesh(CMesh *pMesh);
	virtual void SetShader(CShader *pShader);

	virtual void Animate(float fTimeElapsed);

	virtual void OnPrepareRender();
	virtual void Render();

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected: // 내부 함수

protected: // 변수
	int m_nReferences = 0;

	XMFLOAT4X4 m_xmf4x4World;
	CMesh *m_pMesh = NULL;

	CShader *m_pShader = NULL;
};

