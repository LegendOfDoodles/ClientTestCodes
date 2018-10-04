#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CLoadingCardShader : public CShader
{
public:	// 생성자, 소멸자
	CLoadingCardShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CLoadingCardShader();

public: // 공개 함수
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables(int opt = 0);
	virtual void Render(CCamera *pCamera);

	virtual int GetPlayerId() { return m_myId; }

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);
	virtual void ReleaseObjects();

protected: // 변수
	int m_myId{ 0 };

	CBaseObject * m_LoadingCards[4];
};

