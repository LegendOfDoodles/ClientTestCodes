#pragma once
#include "Texture.h"
#include "Shader.h"

class CMaterial
{
public:	// 생성자, 소멸자
	CMaterial();
	virtual ~CMaterial();

public: // 공개 함수
	void Finalize();

	void ReleaseUploadBuffers();

	void UpdateShaderVariables();

	void Render(CCamera *pCamera);

	void SetTexture(CTexture *pTexture);
	void SetShader(CShader *pShader);

	void SetAlbedo(XMFLOAT4 xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; }

	UINT GetReflectionNum() { return m_nReflection; }

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	bool HaveShader() { return m_pShader != NULL; }

protected: // 내부 함수

protected: // 변수
	int	m_nReferences{ 0 };

	XMFLOAT4						m_xmf4Albedo{ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) };
	UINT							m_nReflection{ 0 };
	CTexture						*m_pTexture{ NULL };
	CShader							*m_pShader{ NULL };
};

