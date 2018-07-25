#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CMaterial;
class CHeightMapTerrain;
class CCollisionManager;

struct FlyingObjectIndices
{
	int m_begIndex;
	int m_endIndex;
};

class CFlyingShader : public CShader
{
public: // 생성자, 소멸자
	CFlyingShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CFlyingShader();

public: // 공개 함수
	virtual void UpdateShaderVariables(int opt = 0);

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	int GetNexusCount() { return m_nNexus; }
	int GetTowerCount() { return m_nTower; }

	void SetColManagerToObject(shared_ptr<CCollisionManager> manager);

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	int GetPossibleIndex(FlyingObjectType type);

protected: // 변수
	int m_nNexus{ 0 };
	int m_nTower{ 0 };

	int m_meshCounts[4];

	std::unordered_map<FlyingObjectType, UINT> m_objectsMaxCount;
	std::unordered_map<FlyingObjectType, FlyingObjectIndices> m_objectsIndices;
	std::unique_ptr<bool[]> m_objectsPossibleIndices;

	CHeightMapTerrain * m_pTerrain{ NULL };
};