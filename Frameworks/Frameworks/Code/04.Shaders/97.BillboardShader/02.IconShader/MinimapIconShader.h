#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"
#include "05.Objects/95.Billboard/Billboard.h"

typedef std::list<CCollisionObject*> CollisionObjectList;
typedef std::list<CMinimapIconObjects*> MinionIconObjectList;

class CMaterial;
class CPlayer;
class CUIObjectManager;

class CMinimapIconShader : public CShader
{
public:
	CMinimapIconShader(CCreateMgr *pCreateMgr);
	virtual ~CMinimapIconShader();

public: // 공개 함수
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	virtual void GetCamera(CCamera *pCamera);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);
	
	void SetUIObjectsManager(CUIObjectManager * pManger);

	void SetPlayer(CBaseObject **pPlayer) { m_pPlayer = (CCollisionObject**)pPlayer; };
	void SetPlayerCnt(int cnt) { m_nPlayer = cnt; };

	void SetNexusAndTower(CBaseObject **ppObjects) { m_ppNexusAndTower = (CCollisionObject**)ppObjects; };
	void SetNexusAndTowerCnt(int cnt) { m_nNexusAndTower = cnt; };

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr);
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers = 1);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

	int GetPossibleIndex();
	void SpawnMinionIcon();

	void SetPossibleIndex(int idx) { m_indexArr[idx] = true; }
	void ResetPossibleIndex(int idx) { m_indexArr[idx] = false; }

protected: // 변수
	// Players Icon & Static Objects
	CBaseObject **m_ppObjects{ NULL };
	int m_nObjects = 0;

	// Materials
	CMaterial	**m_ppMaterials{ NULL };
	int m_nMaterials = 0;

	// 카메라
	CCamera *m_pCamera;

	// Players
	CCollisionObject **m_pPlayer;
	int m_nPlayer = 0;

	// Tower And Nexus
	CCollisionObject **m_ppNexusAndTower{NULL};
	int m_nNexusAndTower = 0;

	//동적생성
	bool m_indexArr[MAX_MINION]{ false };

	CollisionObjectList *m_MinionObjectList;
	MinionIconObjectList m_MinionIconObjectList;

	CUIObjectManager * m_pIconManger{ NULL };
	CCreateMgr* m_pCreateMgr{ NULL };

#if USE_INSTANCING
	CB_GAMEOBJECT_INFO *m_pMappedObjects{ NULL };
	CMaterial		   *m_pMaterial{ NULL };
#else
	UINT8 *m_pMappedObjects{ NULL };
#endif

};