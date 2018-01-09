#pragma once
#include "BaseObject.h"

class CCreateMgr;

class CShader
{
public:	// 생성자, 소멸자
	CShader(CCreateMgr *pCreateMgr);
	~CShader();

public: // 공개 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppd3dShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppd3dShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr);

	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr) { }
	virtual void UpdateShaderVariables() { }
	virtual void ReleaseShaderVariables() { }

	virtual void ReleaseUploadBuffers();

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);
	virtual void ReleaseObjects();

	virtual void AnimateObjects(float timeElapsed);

	virtual void OnPrepareRender();
	virtual void Render();

	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName,
		LPCSTR pszShaderProfile, ID3DBlob **ppShaderBlob);

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected: // 내부 함수

protected: // 변수
	int m_nReferences = 0;

	CBaseObject **m_ppObjects = NULL;
	int m_nObjects = 0;

	ID3D12PipelineState **m_ppPipelineStates = NULL;
	int m_nPipelineStates = 0;

	ID3D12GraphicsCommandList *m_pCommandList;
};

