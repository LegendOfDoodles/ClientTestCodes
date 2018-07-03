#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CMaterial;
class CCollisionObject;

class CNexusAndTowerHPGaugeShader : public CShader
{
public:
	CNexusAndTowerHPGaugeShader(CCreateMgr *pCreateMgr);
	virtual ~CNexusAndTowerHPGaugeShader();

public: // ���� �Լ�
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	virtual void GetCamera(CCamera *pCamera);

	void SetNexusAndTower(CBaseObject **ppObjects) { m_ppNexusAndTower = (CCollisionObject**)ppObjects; };
	void SetNexusAndTowerCnt(int cnt) { m_nNexusAndTower = cnt; };

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

protected: // ���� �Լ�
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers = 1);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // ����
	CBaseObject * *m_ppObjects{ NULL };
	int m_nObjects = 0;

	CMaterial	**m_ppMaterials{ NULL };
	int m_nMaterials = 0;

	CCamera *m_pCamera;
	CCreateMgr* m_pCreateMgr{ NULL };

	CCollisionObject **m_ppNexusAndTower{ NULL };
	int m_nNexusAndTower = 0;

	UINT8 *m_pMappedObjects{ NULL };
};