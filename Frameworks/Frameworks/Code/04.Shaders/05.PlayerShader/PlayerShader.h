#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"

class CMaterial;
class CHeightMapTerrain;
class CCollisionManager;
class CSkeleton;
class CPlayerShader : public CShader
{
public:
	CPlayerShader(shared_ptr<CCreateMgr> pCreateMgr);
	~CPlayerShader();

public: // 공개 함수
	virtual void Initialize(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables(int opt = 0);
	virtual void UpdateBoundingBoxShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);
	virtual void RenderBoundingBox(CCamera *pCamera);
	virtual void RenderShadow(CCamera *pCamera);

	virtual CBaseObject *PickObjectByRayIntersection(
		XMFLOAT3& pickPosition, XMFLOAT4X4& xmf4x4View, float &nearHitDistance);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);

	void SetColManagerToObject(shared_ptr<CCollisionManager> manager);

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual D3D12_SHADER_BYTECODE CreateShadowVertexShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // 변수
	int m_nSword{ 0 };
	int m_nStaff{ 0 };
	int m_nBow{ 0 };

	//애니메이션 정보
	CSkeleton** m_ppSwordAni{ NULL };
	CSkeleton** m_ppStaffAni{ NULL };

	CSkeleton** m_ppBowAni{NULL};

	//메쉬정보
	CSkinnedMesh* m_pStick{ NULL };
	CSkinnedMesh** m_pSword{ NULL };
	CSkinnedMesh** m_pStaff{ NULL };
	CSkinnedMesh** m_pBow{ NULL };
	int m_nWeaponState{ 0 };

	CHeightMapTerrain * m_pTerrain{ NULL };
};
