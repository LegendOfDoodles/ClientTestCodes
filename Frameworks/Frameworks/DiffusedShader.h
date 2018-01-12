#pragma once
#include "Shader.h"

class CDiffusedShader : public CShader
{
public: // ������, �Ҹ���
	CDiffusedShader(CCreateMgr *pCreateMgr);
	~CDiffusedShader();

public: // ���� �Լ�
	virtual void Initialize(CCreateMgr *pCreateMgr, void *pContext = NULL);

protected: // ���� �Լ�
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr);

private:	 // ����
};

