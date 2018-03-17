#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CTerrainShader : public CShader
{
public:	// 생성자, 소멸자
	CTerrainShader(CCreateMgr *pCreateMgr);
	virtual ~CTerrainShader();

public: // 공개 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr);
};