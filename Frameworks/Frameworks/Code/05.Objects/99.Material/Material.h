#pragma once
#include "05.Objects/99.Material/00.Texture/Texture.h"
#include "04.Shaders/00.BaseShader/Shader.h"

class CCreateMgr;

struct COLOR
{
	XMFLOAT4						m_xmf4Diffuse{ 1.0f, 1.0f, 1.0f, 1.0f };
	float									m_cSpecularPower{ 1.0f };
	float									m_cRoughness{ 1.0f };
};

class CMaterial
{
public:	// ������, �Ҹ���
	CMaterial(CCreateMgr *pCreateMgr);
	virtual ~CMaterial();

public: // ���� �Լ�
	void Initialize(CCreateMgr *pCreateMgr);
	void Finalize();

	void ReleaseUploadBuffers();

	void UpdateShaderVariables();

	void Render(CCamera *pCamera);

	void SetTexture(CTexture *pTexture);
	void SetShader(CShader *pShader);

	void SetAlbedo(XMFLOAT4 xmf4Albedo) { m_color.m_xmf4Diffuse = xmf4Albedo; }
	void SetSmoothness(float SP) { m_color.m_cSpecularPower = SP; }
	void SetRoughness(float R) { m_color.m_cRoughness = CLAMP(R, 0, 1); }
	
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

	bool HaveShader() { return m_pShader != NULL; }

protected: // ���� �Լ�
	void CreateShaderVariables(CCreateMgr *pCreateMgr);
	void ReleaseShaderVariables();
	void UpdateShaderVariable();

protected: // ����
	int	m_nReferences{ 0 };

	ID3D12Resource				*m_pcbColor{ NULL };
	COLOR							*m_pcbMappedColor{ NULL };

	COLOR							m_color;

	CTexture						*m_pTexture{ NULL };

	CShader							*m_pShader{ NULL };

	ID3D12GraphicsCommandList *m_pCommandList{ NULL };
};
