#pragma once
#include "Shader.h"

class CDiffusedShader : public CShader
{
public: // 생성자, 소멸자
	CDiffusedShader(CCreateMgr *pCreateMgr);
	~CDiffusedShader();

public: // 공개 함수
	virtual void Initialize(CCreateMgr *pCreateMgr, void *pContext = NULL);

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr);

private:	 // 변수
};

