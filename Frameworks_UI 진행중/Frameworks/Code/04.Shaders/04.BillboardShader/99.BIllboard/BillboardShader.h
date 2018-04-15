#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CBillboardObject;
class CHeightMapTerrain;

class CBillboardShader : public CShader
{
public:
	CBillboardShader(CCreateMgr *pCreateMgr);
	virtual ~CBillboardShader();

public:
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

protected:
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr);
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseShaderVariables();
	virtual void ReleaseObjects();
private:
	CBillboardObject * m_pBillboard{ NULL };
	CB_GAMEOBJECT_INFO m_pMappedBillboard;
};