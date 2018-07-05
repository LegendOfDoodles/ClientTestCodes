#pragma once
#include "04.Shaders/00.BaseShader/Shader.h"
#include "05.Objects/02.CollisionObject/CollisionObject.h"

class CMaterial;
class CPlayer;

enum TeamKILL {
	BlueTeam,
	RedTeam,
	EnumCnt
};

class CNumberShader : public CShader
{
public: // ������, �Ҹ���
	CNumberShader(CCreateMgr *pCreateMgr);
	virtual ~CNumberShader();

public: // ���� �Լ�
	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);

	virtual void GetCamera(CCamera *pCamera);

	virtual void SetPlayer(CBaseObject **ppPlayer) { m_ppPlayers = (CPlayer**)ppPlayer; };
	virtual void SetPlayerCnt(int cnt) { m_nPlayer = cnt; };
	
	void PositionalNumber(int inputNum, int PositionalNumber);
	void SignificantDigit(int InputNum, int PositionalNumber, int *Num);

protected: // ���� �Լ�
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_BLEND_DESC CreateBlendState();

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);
	virtual void CreateShaderVariables(CCreateMgr * pCreateMgr, int nBuffers);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseObjects();

protected: // ����
	CBaseObject * *m_ppObjects{ NULL };
	int m_nObjects = 0;

	// Player's
	CPlayer **m_ppPlayers{NULL};
	int m_nPlayer = 0;

	// Team Kill Sum Value
	/* 0 - Blue 1 - Red  */
	int m_iTeamKill[TeamKILL::EnumCnt] = {0};
	int m_iTeamKillPositionalNum[2] = { 0 };
	int *m_iTeamKillSignificantNum[2] = { 0 };

	// Personal KDA
	/* KILL Death Assist */
	int m_iPlayerKDA[3] = { 0 };
	int m_iKDAPositionalNum[3] = { 0 };
	int *m_iKDASignificantNum[3] = { 0 };
	
	// Timer
	/* Min Sec */
	int m_iTimer[2] = { 0 };
	int m_iTimerPositionalNum[2] = { 0 };
	int *m_iTimerSignificnatNum[2] = { 0 };

	CMaterial	**m_ppMaterials{ NULL };
	CCamera *m_pCamera;

	UINT8 *m_pMappedObjects{ NULL };
};

