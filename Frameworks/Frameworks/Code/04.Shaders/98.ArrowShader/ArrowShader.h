#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/100.Arrow/Arrow.h"

class CArrowShader : public CShader
{
public:	// 생성자, 소멸자
	CArrowShader(CCreateMgr* pCreateMgr);
	virtual ~CArrowShader();

public: // 공개 함수
	virtual void ReleaseUploadBuffers();
	virtual void UpdateShaderVariables();

	virtual void Render(CCamera *pCamera);

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr);
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // 변수
	CArrow * m_pArrow{ NULL };
	CB_GAMEOBJECT_INFO *m_pMappedArrow{ NULL };
};

