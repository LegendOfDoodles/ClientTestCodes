#pragma once
#include "05.Objects/99.Material/Material.h"

class CCreateMgr;

namespace Materials
{
	// Terrain
	CMaterial* CreateTerrainMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// SkyBox
	CMaterial* CreateSkyBoxMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateFloorMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateCubeMapMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// Building
	CMaterial* CreateTresureBoxMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateShellMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateDuckMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateKeumOneBoMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateRoundSoapDispenserMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateSquareSoapDispenserMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// Environment
	CMaterial* CreateEraserMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateDiceMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateLongPencilMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateNailClipperMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreatePaperCupMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreatePenMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreatePencilCaseMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreatePenCoverMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateRoundHeadPhoneMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateShortPencilMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateSquareHeadPhoneMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateBook1Material(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateBook2Material(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateBook3Material(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateBook4Material(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// Minion
	CMaterial* CreateMinionMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// Player
	CMaterial* CreatePlayerMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// Sketch Effect
	CMaterial* CreateSketchMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);
	
	// UI----------------------------------------------------------
	CMaterial* CreateUIMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateMinimapUIMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateGreyMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateNumberMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

		//Skill
	CMaterial* CreateStickGreySkillMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateStickColorSkillMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateSwordGreySkillMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateSwordColorSkillMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

		// HP
	CMaterial* CreateRedGaugeMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateBlueGaugeMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreatePlayerMPGagueMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);
	
	// Icon
		//Player
			//Blue
		// Texture Array 
	CMaterial* CreatePlayerBlueIconMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

			//Red
	CMaterial* CreatePlayerRedIconMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

		// Minion
	CMaterial* CreateMinionIconMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

		// System
	CMaterial* CreateDeathIconMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateLevelUpMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

		// Common Monster
	CMaterial* CreateCamBossIconMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateLoydeIconMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	CMaterial* CreateGolemIconMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

		// Tower
	CMaterial* CreateTowerIconMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

		// Nexus
	CMaterial* CreateNexusIconMaterial(shared_ptr<CCreateMgr> pCreateMgr,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);

	// SRV 생성 함수
	void GetShaderResourceViewDesc(
		D3D12_RESOURCE_DESC resourceDesc,
		UINT nTextureType,
		D3D12_SHADER_RESOURCE_VIEW_DESC *pShaderResourceViewDesc);
		
	void CreateShaderResourceViews(
		shared_ptr<CCreateMgr> pCreateMgr, CTexture *pTexture,
		UINT nRootParameterStartIndex, bool bAutoIncrement,
		D3D12_CPU_DESCRIPTOR_HANDLE *pSrvCPUDescriptorStartHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE *pSrvGPUDescriptorStartHandle);
}