#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/03.Terrain/HeightMapTerrain.h"

class CTerrainShader : public CShader
{
public:	// ������, �Ҹ���
	CTerrainShader(CCreateMgr *pCreateMgr);
	virtual ~CTerrainShader();

public: // ���� �Լ�
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();

	virtual void Render(CCamera *pCamera);

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
	CHeightMapTerrain *m_pTerrain{ NULL };
	CB_GAMEOBJECT_INFO *m_pMappedTerrain{ NULL };
};