#pragma once
#include "00.Global/01.Utility/04.WayFinder/01.Edge/Edge.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"

class CMaterial;
class CHeightMapTerrain;
class CCollisionManager;
class CUIObjectManager;
class CFSMMgr;

class CNeutralityShader : public CShader
{
public: // ������, �Ҹ���
	CNeutralityShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CNeutralityShader();

public: // ���� �Լ�
	virtual void Initialize(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables(int opt = 0);
	virtual void UpdateBoundingBoxShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);
	virtual void RenderBoundingBox(CCamera *pCamera);
	virtual void RenderShadow(CCamera *pCamera);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);

	virtual CBaseObject * * GetCollisionObjects() { return m_ppObjects; }
	int GetObjectCount() {  return m_nObjects; }

	void SetCollisionManager(shared_ptr<CCollisionManager> pManger) { m_pColManager = pManger; }
	void SetGaugeManger(shared_ptr<CUIObjectManager> pManger) { m_pGaugeManger = pManger; }
	void SetFSMManager(shared_ptr<CFSMMgr> pManger) { m_pFSMMgr = pManger; }

protected: // ���� �Լ�
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual D3D12_SHADER_BYTECODE CreateShadowVertexShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // ����
	shared_ptr<CCollisionManager> m_pColManager;
	shared_ptr<CUIObjectManager> m_pGaugeManger;
	CHeightMapTerrain * m_pTerrain{ NULL };

	shared_ptr<CCreateMgr> m_pCreateMgr{ NULL };
	shared_ptr<CFSMMgr> m_pFSMMgr;
};
