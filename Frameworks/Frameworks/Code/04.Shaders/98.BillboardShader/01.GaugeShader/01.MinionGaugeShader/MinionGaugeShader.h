#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "05.Objects/96.Billboard/02.GaugeObject/GaugeObject.h"

typedef std::list<CCollisionObject*> CollisionObjectList;
typedef std::list<CGaugeObject*> HPGaugeObjectList;

class CMaterial;
class CUIObjectManager;

class CMinionHPGaugeShader : public CShader
{
public: // 생성자, 소멸자
	CMinionHPGaugeShader(CCreateMgr *pCreateMgr);
	virtual ~CMinionHPGaugeShader();

public: // 공개 함수
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	virtual void GetCamera(CCamera *pCamera);

	void SetUIObjectsManager(CUIObjectManager * pManger);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers = 1);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

	int GetPossibleIndex();
	void SpawnGauge();

	void SetPossibleIndex(int idx) { m_indexArr[idx] = true; }
	void ResetPossibleIndex(int idx) { m_indexArr[idx] = false; }

protected: // 변수
	CUIObjectManager * m_pGaugeManger{ NULL };

	CCamera *m_pCamera;

	CollisionObjectList *m_MinionObjectList;
	HPGaugeObjectList m_HPGaugeObjectList;

	bool m_indexArr[MAX_MINION]{ false };

	UINT8 *m_pMappedObjects{ NULL };

	CCreateMgr* m_pCreateMgr{ NULL };

};

