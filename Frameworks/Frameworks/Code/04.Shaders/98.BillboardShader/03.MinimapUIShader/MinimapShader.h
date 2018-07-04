#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CUIFrameObject;
class CMaterial;
class CPlayer;
class CWayFinder;

class CMinimapShader : public CShader
{
public: // ������, �Ҹ���
	CMinimapShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CMinimapShader();

public: // ���� �Լ�
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	virtual void SetCamera(CCamera *pCamera);
	virtual void SetPlayer(CBaseObject *pPlayer) { m_pPlayer = (CPlayer*)pPlayer; };
	virtual void SetWayFinder(shared_ptr<CWayFinder> pWayFinder) { m_pWayFinder = pWayFinder; };

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

protected: // ���� �Լ�
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // ����
	CCamera *m_pCamera;

	CPlayer *m_pPlayer;
	shared_ptr<CWayFinder> m_pWayFinder;

	bool OnOFF{ false };
};

