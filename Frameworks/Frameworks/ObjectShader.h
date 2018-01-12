#pragma once
#include "Shader.h"

class CObjectShader : public CShader
{
public: // ������, �Ҹ���
	CObjectShader(CCreateMgr *pCreateMgr);
	~CObjectShader();

public: // ���� �Լ�
	virtual void ReleaseUploadBuffers();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

protected: // ���� �Լ�
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // ����
	CBaseObject **m_ppObjects = NULL;
	int m_nObjects = 0;
};

