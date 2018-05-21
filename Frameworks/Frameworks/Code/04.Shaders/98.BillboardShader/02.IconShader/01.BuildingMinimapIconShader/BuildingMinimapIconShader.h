#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "05.Objects/96.Billboard/Billboard.h"

class CMaterial;

class CBuildingMinimapIconShader : public CShader
{
public:
	CBuildingMinimapIconShader(CCreateMgr *pCreateMgr);
	virtual ~CBuildingMinimapIconShader();

public: // 공개 함수
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	virtual void GetCamera(CCamera *pCamera);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

	virtual void SetNexusAndTower(CBaseObject **ppObjects) { m_ppNexusAndTower = (CCollisionObject**)ppObjects; };
	virtual void SetNexusAndTowerCnt(int cnt) { m_nNexusAndTower = cnt; };

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr, UINT nRenderTargets = 1);
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers = 1);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // 변수
		   // Players Icon & Static Objects
	CBaseObject **m_ppObjects{ NULL };
	int m_nObjects = 0;

	// Materials
	CMaterial	**m_ppMaterials{ NULL };
	int m_nMaterials = 0;

	// 카메라
	CCamera *m_pCamera;

	// Tower And Nexus
	CCollisionObject **m_ppNexusAndTower{ NULL };
	int m_nNexusAndTower = 0;

#if USE_INSTANCING
	CB_GAMEOBJECT_INFO *m_pMappedObjects{ NULL };
	CMaterial		   *m_pMaterial{ NULL };
#else
	UINT8 *m_pMappedObjects{ NULL };
#endif

};