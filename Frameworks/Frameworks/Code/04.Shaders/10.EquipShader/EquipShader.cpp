#include "stdafx.h"
#include "EquipShader.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"
#include "05.Objects/12.Equipment/Equipment.h"
#include "05.Objects/08.Player/Player.h"


/// <summary>
/// 목적: 플레이어 관리 및 렌더링 용도
/// 최종 수정자:  김나단
/// 수정자 목록:  정휘현, 김나단
/// 최종 수정 날짜: 2018-07-03
/// </summary>

//////////////////////////////////////////////////////////////////////////////////////////////////
CEquipShader::CEquipShader(shared_ptr<CCreateMgr> pCreateMgr) : CShader(pCreateMgr)
{
}

CEquipShader::~CEquipShader()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수

void CEquipShader::UpdateShaderVariables(int opt)
{
	UNREFERENCED_PARAMETER(opt);
	static UINT elementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < 4; i++)
	{
		CPlayer* pPlayer = (dynamic_cast<CEquipment*>(m_ppObjects[i*m_nMaxEquip]))->GetMaster();
		CB_ANIOBJECT_INFO *pMappedObject = (CB_ANIOBJECT_INFO *)(m_pMappedObjects + ((/*matIndex*/0 * m_nPlayer + i/*selectIndex*/) * elementBytes));
		
		memcpy(pMappedObject->m_xmf4x4Frame, pPlayer->GetFrameMatrix(), sizeof(XMFLOAT4X4) * 128);
		
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World0,
			XMMatrixTranspose(XMLoadFloat4x4(pPlayer->GetWorldMatrix())));
	}
}

void CEquipShader::AnimateObjects(float timeElapsed)
{
	UNREFERENCED_PARAMETER(timeElapsed);

	//for (int i = 0; i < m_nPlayer; ++i) {
	//	if (m_pPlayer[i]->GetWeaponType() != m_nPlayerWeaponType[i]) {
	//		m_nPlayerWeaponType[i] = m_pPlayer[i]->GetWeaponType();
	//		SetEquipMesh(i, 0, 0, m_nPlayerWeaponType[i]);
	//	}
	//	else if (m_pPlayer[i]->GetWeaponNum() != m_arrEquipIndex[i][0]) {
	//		m_arrEquipIndex[i][0] = m_pPlayer[i]->GetWeaponNum();
	//		SetEquipMesh(i, 0, m_arrEquipIndex[i][0], m_nPlayerWeaponType[i]);
	//	}
	//}
}

void CEquipShader::SetEquipMesh(int playerindex, int equiptype, int equipindex, int weaponnum)
{
	CSkinnedMesh* pMesh{ NULL };
	int meshindex = 0;
	if (equiptype == 0) {
		switch (weaponnum)
		{
		case 0:
			pMesh = m_pStick;
			meshindex = 0;
			break;
		case 1:
			pMesh = m_pSword[equipindex];
			meshindex = equipindex+1;
			break;
		case 2:
			pMesh = m_pStaff[equipindex];
			break;
		case 3:
			pMesh = m_pBow[equipindex];
			break;
		default:
			pMesh = m_pStick;
			break;
		}
	}
	m_nMeshIndex[meshindex]++;
	m_arrEquipIndex[playerindex][equiptype] = meshindex;
	m_ppObjects[playerindex * m_nMaxEquip + equiptype]->SetMesh(0, pMesh);
	//m_ppObjects[playerindex * m_nMaxEquip + equiptype]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[meshindex].ptr + (incrementSize * playerindex));
}

void CEquipShader::Render(CCamera *pCamera)
{
	CShader::Render(pCamera, 0);
	
	for (int i = 0; i < m_nPlayer; ++i) {
		for (int j = 0; j < m_nPlayerEquipments[i]; j++)
		{
			m_ppMaterials[1]->UpdateShaderVariables();
			m_ppObjects[i * m_nMaxEquip + j]->Render(pCamera);
		}
	}
}



////////////////////////////////////////////////////////////////////////
// 내부 함수
D3D12_INPUT_LAYOUT_DESC CEquipShader::CreateInputLayout()
{
	UINT nInputElementDescs = 6;
	D3D12_INPUT_ELEMENT_DESC *pInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];
	pInputElementDescs[0] = {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[1] = {
		"NORMAL",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[2] = {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	pInputElementDescs[3] = {
		"WEIGHTS",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[4] = {
		"BONEINDICES",
		0,
		DXGI_FORMAT_R8G8B8A8_UINT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	pInputElementDescs[5] = {
		"TANGENT",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		D3D12_APPEND_ALIGNED_ELEMENT,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_SHADER_BYTECODE CEquipShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSBone",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CEquipShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSBone",
		"ps_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CEquipShader::CreateShadowVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/ShadowShader.hlsl",
		"VSBone",
		"vs_5_1",
		pShaderBlob));
}

void CEquipShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 3;

	m_nHeaps = 6;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CEquipShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext)
{
	UNREFERENCED_PARAMETER(pContext);
	m_nObjects = m_nPlayer * m_nMaxEquip;
	m_ppObjects = new CCollisionObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);
	
	int accCnt{ 0 };

	CreateShaderVariables(pCreateMgr, ncbElementBytes, m_nPlayer * m_nHeaps, true, ncbElementBytes, m_nObjects);
	for (int i = 0; i < m_nHeaps; ++i)
	{
		CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nPlayer, 1, i);
		CreateConstantBufferViews(pCreateMgr, m_nPlayer, m_pConstBuffer.Get(), ncbElementBytes, accCnt, i);
		accCnt += m_nPlayer;
	}

#if USE_BATCH_MATERIAL

#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif
	m_nMaterials = m_nHeaps;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreateLolipopMaterial(pCreateMgr,	&m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[1] = Materials::CreateSwordMaterial(pCreateMgr,	&m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);
	m_ppMaterials[2] = Materials::CreateMaceMaterial(pCreateMgr,	&m_psrvCPUDescriptorStartHandle[2], &m_psrvGPUDescriptorStartHandle[2]);
	m_ppMaterials[3] = Materials::CreateLolipopMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[3], &m_psrvGPUDescriptorStartHandle[3]);
	m_ppMaterials[4] = Materials::CreateSawMaterial(pCreateMgr,		&m_psrvCPUDescriptorStartHandle[4], &m_psrvGPUDescriptorStartHandle[4]);
	m_ppMaterials[5] = Materials::CreateSabreMaterial(pCreateMgr,	&m_psrvCPUDescriptorStartHandle[5], &m_psrvGPUDescriptorStartHandle[5]);

	m_pStick = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Player_Stick.meshinfo");

	
	m_nSword = 5;

	m_pSword = new CSkinnedMesh*[m_nSword];
	m_pSword[0] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Sword//Player_Sword_Basic.meshinfo");
	m_pSword[1] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Sword//Mace.meshinfo");
	m_pSword[2] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Sword//BFSword.meshinfo");
	m_pSword[3] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Sword//Saw.meshinfo");
	m_pSword[4] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Sword//Sabre.meshinfo");


	m_nBow = 3;

	m_pBow = new CSkinnedMesh*[m_nBow];
	m_pBow[0] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Bow//Player_Bow_Basic.meshinfo");
	m_pBow[1] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Bow//Player_Bow_Flight.meshinfo");
	m_pBow[2] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Bow//Player_Bow_Battle.meshinfo");

	m_nStaff = 3;

	m_pStaff = new CSkinnedMesh*[m_nStaff];
	m_pStaff[0] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Staff//Player_Staff_Basic.meshinfo");
	m_pStaff[1] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Staff//Player_Staff_Lolipop.meshinfo");
	m_pStaff[2] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Staff//Player_Staff_Watch.meshinfo");

	m_nArmor = 4;
	m_pArmor = new CSkinnedMesh*[m_nArmor];
	m_pArmor[0] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Armor//extra//shoulderpack.meshinfo");
	m_pArmor[1] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Armor//cloth//BulletJaket.meshinfo");
	m_pArmor[2] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Armor//Shose//Nike.meshinfo");
	m_pArmor[3] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Armor//Familiar//MedicBox.meshinfo");
	
	m_nSpecial = 4;
	m_pSpecial = new CSkinnedMesh*[m_nSpecial];
	m_pSpecial[0] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Armor//hat//Glasses.meshinfo");
	m_pSpecial[1] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Armor//hat//Sunglass.meshinfo");
	m_pSpecial[2] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Armor//extra//Winder.meshinfo");
	m_pSpecial[3] = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Armor//hat//Icecream.meshinfo");

	
	m_pStick->AddRef();
	for (UINT j = 0; j < m_nSword; ++j) {
		m_pSword[j]->AddRef();
	}
	for (UINT j = 0; j < m_nStaff; ++j) {
		m_pStaff[j]->AddRef();
	}
	for (UINT j = 0; j < m_nBow; ++j) {
		m_pBow[j]->AddRef();
	}
	for (UINT j = 0; j < m_nArmor; ++j) {
		m_pArmor[j]->AddRef();
	}
	for (UINT j = 0; j < m_nSpecial; ++j) {
		m_pSpecial[j]->AddRef();
	}
	m_nMeshCnt = 1 + m_nSword + m_nBow + m_nStaff + m_nArmor+ m_nSpecial;
	m_nMeshIndex = new int[m_nMeshCnt];

	int i = 0;
	incrementSize = pCreateMgr->GetCbvSrvDescriptorIncrementSize();
	CEquipment *pEquip = { NULL };

	for (int x = 0; x < m_nPlayer; ++x) {
		for (int y = 0; y < m_nMaxEquip; ++y) {
			pEquip = new CEquipment(pCreateMgr, 1);
			pEquip->SetMaster(m_pPlayer[x]);
			
			m_ppObjects[i++] = pEquip;
		}
		m_nPlayerEquipments[x] = 1;
	}

	m_nMeshIndex[0] = m_nPlayer; //Stick;
	for (int x = 0; x < m_nPlayer; ++x) {
		m_nPlayerWeaponType[x] =0;
		m_arrEquipIndex[x][0] = 0;
		m_ppObjects[x*m_nMaxEquip]->SetMesh(0, m_pSword[0]);
		m_ppObjects[x*m_nMaxEquip]->SetMaterial(m_ppMaterials[1]);
		m_ppObjects[x*m_nMaxEquip]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[1].ptr + (incrementSize * x));
	}
}

void CEquipShader::ReleaseObjects()
{
	for (UINT j = 0; j < m_nSword; j++)
	{
		delete m_pSword[j];
	}
	Safe_Delete_Array(m_pSword);


	for (UINT j = 0; j < m_nStaff; j++)
	{
		delete m_pStaff[j];
	}
	Safe_Delete_Array(m_pStaff);


	for (UINT j = 0; j < m_nBow; j++)
	{
		delete m_pBow[j];
	}
	Safe_Delete_Array(m_pBow);


	for (UINT j = 0; j < m_nArmor; j++)
	{
		delete m_pArmor[j];
	}
	Safe_Delete_Array(m_pArmor);

	///////////////////////////////////////
	if (m_ppMaterials)
	{
		for (int i = 0; i < m_nMaterials; ++i)
		{
			if (m_ppMaterials[i]) Safe_Delete(m_ppMaterials[i]);
		}
		Safe_Delete_Array(m_ppMaterials);
	}
}

