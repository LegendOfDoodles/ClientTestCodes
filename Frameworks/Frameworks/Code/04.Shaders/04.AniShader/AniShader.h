#pragma once
#include "00.Global/01.Utility/04.WayFinder/01.Edge/Edge.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"

typedef std::list<CPathEdge> Path;
typedef std::list<CBaseObject*> ObjectList;

class CMaterial;
class CHeightMapTerrain;
class CCollisionManager;
class CAniShader : public CShader	// Warning! 얘는 오브젝트 쉐이더를 상속 받는 구조로 바꿀 필요 있음
{
public: // 생성자, 소멸자
	CAniShader(CCreateMgr *pCreateMgr);
	virtual ~CAniShader();

public: // 공개 함수
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();
	virtual void UpdateBoundingBoxShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);
	virtual void RenderBoundingBox(CCamera *pCamera);

	virtual CBaseObject *PickObjectByRayIntersection(
		XMFLOAT3& pickPosition, XMFLOAT4X4& xmf4x4View, float &nearHitDistance);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);

	virtual CBaseObject * * GetCollisionObjects() { return m_ppObjects; }
	int GetObjectCount() {  return m_nObjects; }

	void SetCollisionManager(CCollisionManager* manager);

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr);
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers = 1);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

	void CreatePathes();

	int GetPossibleIndex();
	void SpawnMinion(CCreateMgr *pCreateMgr, Minion_Species kind);

	void SetPossibleIndex(int idx) { m_indexArr[idx] = true; }
	void ResetPossibleIndex(int idx) { m_indexArr[idx] = false; }

protected: // 변수
	CBaseObject * *m_ppObjects{ NULL };
	int m_nObjects{ 0 };

	int m_kind{ 0 };

	ObjectList m_blueObjects;
	ObjectList m_redObjects;

	bool m_indexArr[MAX_MINION]{ false };

	Path m_pathes[4];

	CSkinnedMesh* m_pWeapons[3];
	int m_nWeaponState{ 0 };

#if USE_INSTANCING
	CB_GAMEOBJECT_INFO *m_pMappedObjects{ NULL };
	CMaterial						*m_pMaterial{ NULL };
#else
	UINT8 *m_pMappedObjects{ NULL };
#endif
	UINT8 *m_pMappedBoundingBoxes{ NULL };
	
	CCollisionManager* m_pColManager{NULL};
	
	CHeightMapTerrain * m_pTerrain{ NULL };

	CCreateMgr* m_pCreateMgr{ NULL };
};
