#pragma once
#include "00.Global/01.Utility/04.WayFinder/01.Edge/Edge.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"

typedef std::list<CPathEdge> Path;
typedef std::list<CCollisionObject*> CollisionObjectList;

class CMaterial;
class CHeightMapTerrain;
class CCollisionManager;
class CThrowingMgr;
class CUIObjectManager;
class CFSMMgr;

class CMinionShader : public CShader
{
public: // 생성자, 소멸자
	CMinionShader(shared_ptr<CCreateMgr> pCreateMgr);
	virtual ~CMinionShader();

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

	void SetCollisionManager(shared_ptr<CCollisionManager> pManger) { m_pColManager = pManger; }
	void SetThrowingManager(shared_ptr<CThrowingMgr> pManger) { m_pThrowingMgr = pManger; }
	void SetGaugeManger(shared_ptr<CUIObjectManager> pManger) { m_pGaugeManger = pManger; }
	void SetFSMManager(shared_ptr<CFSMMgr> pManger) { m_pFSMMgr = pManger; }
	void SetEffectManager(shared_ptr<CEffectMgr> pManger) { m_pEffectMgr = pManger; }

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual D3D12_SHADER_BYTECODE CreateShadowVertexShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);

	virtual void BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

	void CreatePathes();

	int GetPossibleIndex(ObjectType type);
	void SetPossibleIndex(int idx, ObjectType type);
	void ResetPossibleIndex(int idx, ObjectType type);

	void SpawnMinion();

protected: // 변수
	ObjectType m_kind{ ObjectType::SwordMinion };

	static const int m_nMeshes{ 3 };
	CSkinnedMesh *m_ppMinionMeshes[m_nMeshes]{};

	CollisionObjectList m_blueSwordMinions;
	CollisionObjectList m_blueStaffMinions;
	CollisionObjectList m_blueBowMinions;
	CollisionObjectList m_redSwordMinions;
	CollisionObjectList m_redStaffMinions;
	CollisionObjectList m_redBowMinions;

	std::unordered_map<ObjectType, bool*> m_possibleIndicies;

	Path m_pathes[4];
	
	shared_ptr<CCollisionManager> m_pColManager;
	shared_ptr<CThrowingMgr> m_pThrowingMgr;
	shared_ptr<CUIObjectManager> m_pGaugeManger;
	shared_ptr<CEffectMgr> m_pEffectMgr;

	CHeightMapTerrain * m_pTerrain{ NULL };

	shared_ptr<CCreateMgr> m_pCreateMgr{ NULL };
	shared_ptr<CFSMMgr> m_pFSMMgr;

	float m_spawnTime{ 10.1f };
	float m_preSpawnTime{ 0.0f };

	int m_curSpawnCount{ 0 };

	bool m_bDataPrepared{ false };
};
