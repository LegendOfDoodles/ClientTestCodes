#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CUIFrameObject;
class CMaterial;
class CPlayer;
class CWayFinder;

class CMinimapShader : public CShader
{
public: // 생성자, 소멸자
	CMinimapShader(CCreateMgr *pCreateMgr);
	virtual ~CMinimapShader();

public: // 공개 함수
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	virtual void GetCamera(CCamera *pCamera);
	virtual void SetPlayer(CBaseObject *pPlayer) { m_pPlayer = (CPlayer*)pPlayer; };
	virtual void SetWayFinder(shared_ptr<CWayFinder> pWayFinder) { m_pWayFinder = pWayFinder; };

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);
	virtual void CreateShaderVariables(CCreateMgr * pCreateMgr, int nBuffers);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // 변수
	CBaseObject * *m_ppObjects{ NULL };
	int m_nObjects = 0;

	CMaterial	**m_ppMaterials{ NULL };

	CCamera *m_pCamera;

	CPlayer *m_pPlayer;
	shared_ptr<CWayFinder> m_pWayFinder;

	bool OnOFF = false;

	UINT8 *m_pMappedObjects{ NULL };
};

