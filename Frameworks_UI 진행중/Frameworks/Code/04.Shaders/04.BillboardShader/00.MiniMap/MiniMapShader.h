#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CBillboardObject;
class CMinimap;
class CMaterial;

class CMiniMapShader : public CShader
{
public: // ������, �Ҹ���
	CMiniMapShader(CCreateMgr *pCreateMgr);
	CMiniMapShader(CCreateMgr *pCreateMgr, CCamera *pCamera);
	~CMiniMapShader();

public: // ���� �Լ�
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	virtual void OnProcessKeyUp(WPARAM wParam, LPARAM lParam, float timeElapsed);
	virtual void OnProcessKeyDown(WPARAM wParam, LPARAM lParam, float timeElapsed);

protected: // ���� �Լ�
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr);
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseShaderVariables();
	virtual void ReleaseObjects();

protected: // ����
	CBillboardObject *m_pMiniMap{ NULL };

#if USE_INSTANCING
	CB_GAMEOBJECT_INFO *m_pMappedObjects{ NULL };
	CMaterial						*m_pMaterial{ NULL };
#else
	CB_GAMEOBJECT_INFO m_pMappedMiniMap;
#endif
};

