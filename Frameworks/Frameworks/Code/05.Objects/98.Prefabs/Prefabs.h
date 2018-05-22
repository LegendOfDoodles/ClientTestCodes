#pragma once
#include "05.Objects/99.Material/Material.h"

class CCreateMgr;

namespace Materials
{
	// Terrain
	CMaterial* CreateTerrainMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// SkyBox
	CMaterial* CreateSkyBoxMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateFloorMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateCubeMapMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// Building
	CMaterial* CreateTresureBoxMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateShellMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateDuckMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateKeumOneBoMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateRoundSoapDispenserMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateSquareSoapDispenserMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// Environment
	CMaterial* CreateEraserMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateDiceMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateLongPencilMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateNailClipperMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreatePaperCupMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreatePenMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreatePencilCaseMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreatePenCoverMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateRoundHeadPhoneMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateShortPencilMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateSquareHeadPhoneMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateBook1Material(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateBook2Material(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateBook3Material(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateBook4Material(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// Minion
	CMaterial* CreateMinionMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// Player
	CMaterial* CreatePlayerMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);
	
	// UI----------------------------------------------------------
	CMaterial* CreateUIMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateGreyMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

		// HP
	CMaterial* CreateRedGaugeMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateBlueGaugeMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// Icon
		//Player
			//Blue
	CMaterial* CreateStickBlueIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateSwordBlueIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);
	
	CMaterial* CreateStaffBlueIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateBowBlueIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

			//Red
	CMaterial* CreateStickRedIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateSwordRedIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateStaffRedIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateBowRedIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

		// Minion
			//Blue
	CMaterial* CreateMinionBlueIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

			//Red
	CMaterial* CreateMinionRedIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

		// System
	CMaterial* CreateDeathIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateLevelUpMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

		// Common Monster
			//White Icon
	CMaterial* CreateCamBossIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateLoydeIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateGolemIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

			// Blue Icon
	CMaterial* CreateCamBossBlueIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateLoydeBlueIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateGolemBlueIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

			// Red Icon
	CMaterial* CreateCamBossRedIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateLoydeRedIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateGolemRedIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

		// Tower
	CMaterial* CreateRedTowerIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateBlueTowerIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

		// Nexus
	CMaterial* CreateBoxNexusIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateShellNexusIconMaterial(CCreateMgr *pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// SRV 생성 함수
	void GetShaderResourceViewDesc(
		D3D12_RESOURCE_DESC resourceDesc,
		UINT nTextureType,
		D3D12_SHADER_RESOURCE_VIEW_DESC *pShaderResourceViewDesc);
		
	void CreateShaderResourceViews(
		CCreateMgr *pCreateMgr, CTexture *pTexture,
		UINT nRootParameterStartIndex, bool bAutoIncrement,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);
}