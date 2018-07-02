#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CTextureToFullScreenShader : public CShader
{
public: // ������, �Ҹ���
	CTextureToFullScreenShader(CCreateMgr *pCreateMgr);
	virtual ~CTextureToFullScreenShader();

public: // ���� �Լ�
	virtual void CreateGraphicsRootSignature(CCreateMgr *pCreateMgr);

	void CreateShaderResourceViews(CCreateMgr *pCreateMgr, shared_ptr<CTexture> pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement, int index = 0);
	virtual void CreateShader(CCreateMgr *pCreateMgr, ID3D12RootSignature *pd3dGraphicsRootSignature, UINT nRenderTargets = 1);

	virtual void BuildObjects(CCreateMgr * pCreateMgr, shared_ptr<CTexture> pContext = NULL);

	virtual void Render(CCamera *pCamera);

	void SetGraphicsRootSignature() { m_pCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature); }

protected: // ���� �Լ�
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	virtual void ReleaseObjects();

protected: // ����
	shared_ptr<CTexture> m_pTexture;
};

