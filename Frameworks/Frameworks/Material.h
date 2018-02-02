#pragma once
#include "Texture.h"
#include "Shader.h"

class CMaterial
{
public:	// ������, �Ҹ���
	CMaterial();
	virtual ~CMaterial();

public: // ���� �Լ�
	void Finalize();

	void ReleaseUploadBuffers();

	void UpdateShaderVariables();

	void SetTexture(CTexture *pTexture);
	void SetShader(CShader *pShader);

	void SetAlbedo(XMFLOAT4 xmf4Albedo) { m_xmf4Albedo = xmf4Albedo; }
	void SetReflection(UINT nReflection) { m_nReflection = nReflection; }

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected: // ���� �Լ�

protected: // ����
	int	m_nReferences{ 0 };

	XMFLOAT4						m_xmf4Albedo{ XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f) };
	UINT							m_nReflection{ 0 };
	CTexture						*m_pTexture{ NULL };
	CShader							*m_pShader{ NULL };
};

