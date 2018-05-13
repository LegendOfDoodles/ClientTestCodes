#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CMaterial;
class CCollisionObject;

class CNexusAndTowerHPGaugeShader : public CShader
{
public:
	CNexusAndTowerHPGaugeShader(CCreateMgr *pCreateMgr);
	virtual ~CNexusAndTowerHPGaugeShader();

public: // 공개 함수
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	virtual void GetCamera(CCamera *pCamera);

	void SetNexusAndTower(CBaseObject **ppObjects) { m_ppNexusAndTower = (CCollisionObject**)ppObjects; };
	void SetNexusAndTowerCnt(int cnt) { m_nNexusAndTower = cnt; };

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr);
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers = 1);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // 변수
	CBaseObject * *m_ppObjects{ NULL };
	int m_nObjects = 0;

	CMaterial	**m_ppMaterials{ NULL };
	int m_nMaterials = 0;

	CCamera *m_pCamera;
	CCreateMgr* m_pCreateMgr{ NULL };

	CCollisionObject **m_ppNexusAndTower{ NULL };
	int m_nNexusAndTower = 0;

#if USE_INSTANCING
	CB_GAMEOBJECT_INFO *m_pMappedObjects{ NULL };
	CMaterial		   *m_pMaterial{ NULL };
#else
	UINT8 *m_pMappedObjects{ NULL };
#endif

};