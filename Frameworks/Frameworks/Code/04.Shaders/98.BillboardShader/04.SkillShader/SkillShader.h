#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CBillboardObject;
class CSkillObject;
class CMaterial;
class CPlayer;

class CSkillShader : public CShader
{
public: // ������, �Ҹ���
	CSkillShader(CCreateMgr *pCreateMgr);
	virtual ~CSkillShader();

public: // ���� �Լ�
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	virtual void GetCamera(CCamera *pCamera);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

protected: // ���� �Լ�
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false);
	virtual void CreateShaderVariables(CCreateMgr * pCreateMgr, int nBuffers);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // ����
	CBaseObject * *m_ppObjects{ NULL };
	int m_nObjects = 0;

	CMaterial	**m_ppMaterials{ NULL };

	CCamera *m_pCamera;

	CPlayer *m_pPlayer;

	UINT8 *m_pMappedObjects{ NULL };
};

