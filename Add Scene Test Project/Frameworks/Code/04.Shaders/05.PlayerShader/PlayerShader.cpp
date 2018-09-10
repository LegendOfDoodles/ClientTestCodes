#include "stdafx.h"
#include "PlayerShader.h"
#include "05.Objects/03.AnimatedObject/AnimatedObject.h"
#include "05.Objects/06.Minion/Minion.h"
#include "05.Objects/08.Player/Player.h"
#include "05.Objects/08.Player/01.PlayerAI/PlayerAI.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"
#include "05.Objects/04.Terrain/HeightMapTerrain.h"
#include "06.Meshes/01.Mesh/MeshImporter.h"

/// <summary>
/// ����: �÷��̾� ���� �� ������ �뵵
/// ���� ������:  �質��
/// ������ ���:  ������, �質��
/// ���� ���� ��¥: 2018-08-22
/// </summary>

//////////////////////////////////////////////////////////////////////////////////////////////////
CPlayerShader::CPlayerShader(shared_ptr<CCreateMgr> pCreateMgr) : CShader(pCreateMgr)
{
}

CPlayerShader::~CPlayerShader()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CPlayerShader::Initialize(shared_ptr<CCreateMgr> pCreateMgr, void *pContext)
{
	CreateShader(pCreateMgr, RENDER_TARGET_BUFFER_CNT, true, true);
	BuildObjects(pCreateMgr, pContext);
}

void CPlayerShader::UpdateShaderVariables(int opt)
{
	UNREFERENCED_PARAMETER(opt);
	static UINT elementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_ANIOBJECT_INFO *pMappedObject = (CB_ANIOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMFLOAT4X4 tmp[31];
		memcpy(tmp, m_ppObjects[i]->GetFrameMatrix(), sizeof(XMFLOAT4X4) * 31);
		memcpy(pMappedObject->m_xmf4x4Frame, tmp, sizeof(XMFLOAT4X4) * 31);

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World0,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CPlayerShader::UpdateBoundingBoxShaderVariables()
{
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedBoundingBoxes + (i * boundingBoxElementBytes));

		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CPlayerShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		if (static_cast<CPlayer*>(m_ppObjects[j])->GetWeaponChangeTriger() == true)
		{
			switch (m_ppObjects[j]->GetPlayerStatus()->Weapon)
			{
			case 1:
				m_ppObjects[0]->SetType((ObjectType)m_nWeaponState);
				m_ppObjects[0]->SetType(ObjectType::SwordPlayer);
				dynamic_cast<CPlayer*>(m_ppObjects[0])->ChangeSkillSet(m_ppSwordAni);
				dynamic_cast<CPlayer*>(m_ppObjects[0])->SetWeaponData(ObjectType::SwordPlayer, 0);

				m_ChangeWeapon = true;
				static_cast<CPlayer*>(m_ppObjects[j])->SetWeaponChangeTriger(false);
				break;
			case 2:
				m_ppObjects[0]->SetType((ObjectType)m_nWeaponState);
				m_ppObjects[0]->SetType(ObjectType::StaffPlayer);
				dynamic_cast<CPlayer*>(m_ppObjects[0])->ChangeSkillSet(m_ppStaffAni);
				dynamic_cast<CPlayer*>(m_ppObjects[0])->SetWeaponData(ObjectType::StaffPlayer, 0);

				m_ChangeWeapon = true;
				static_cast<CPlayer*>(m_ppObjects[j])->SetWeaponChangeTriger(false);
				break;
			case 3:
				m_ppObjects[0]->SetType((ObjectType)m_nWeaponState);
				m_ppObjects[0]->SetType(ObjectType::BowPlayer);
				dynamic_cast<CPlayer*>(m_ppObjects[0])->ChangeSkillSet(m_ppBowAni);
				dynamic_cast<CPlayer*>(m_ppObjects[0])->SetWeaponData(ObjectType::BowPlayer, 0);

				m_ChangeWeapon = true;
				static_cast<CPlayer*>(m_ppObjects[j])->SetWeaponChangeTriger(false);
				break;
			}
		}

		m_ppObjects[j]->Animate(timeElapsed);
	}
}

void CPlayerShader::Render(CCamera *pCamera)
{
	CShader::Render(pCamera, 0);
	if (m_ppMaterials) m_ppMaterials[0]->UpdateShaderVariables();

	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Render(pCamera);

	}
}

void CPlayerShader::RenderBoundingBox(CCamera * pCamera)
{
	CShader::RenderBoundingBox(pCamera);

	for (int j = 0; j < m_nObjects; j++)
	{
		if (m_ppObjects[j]) m_ppObjects[j]->RenderBoundingBox(pCamera);
	}
}

void CPlayerShader::RenderShadow(CCamera * pCamera)
{
	CShader::Render(pCamera, 0, 2);

	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Render(pCamera);
	}
}

CBaseObject *CPlayerShader::PickObjectByRayIntersection(
	XMFLOAT3& pickPosition, XMFLOAT4X4& xmf4x4View, float &nearHitDistance)
{
	bool intersected = 0;

	nearHitDistance = FLT_MAX;
	float hitDistance = FLT_MAX;
	CBaseObject *pSelectedObject{ NULL };

	for (int j = 0; j < m_nObjects; j++)
	{
		intersected = m_ppObjects[j]->PickObjectByRayIntersection(pickPosition, xmf4x4View, hitDistance);
		if (intersected && (hitDistance < nearHitDistance))
		{
			nearHitDistance = hitDistance;
			pSelectedObject = m_ppObjects[j];
		}
	}

	return(pSelectedObject);
}

bool CPlayerShader::OnProcessKeyInput(UCHAR* pKeyBuffer)
{
	if (m_ppObjects[0]->GetState() == States::Die || m_ppObjects[0]->GetState() == States::Remove) return true;

	UNREFERENCED_PARAMETER(pKeyBuffer);

	static float R = 0.0f;
	static float M = 0.0f;


	if (GetAsyncKeyState('A') & 0x0001)
	{
		m_ppObjects[0]->ActiveSkill(AnimationsType::Attack1);
	}
	else if (GetAsyncKeyState('Q') & 0x0001)
	{
		if (m_ppObjects[0]->GetPlayerStatus()->QSkillCoolTime >= 1.f)
		{
			m_ppObjects[0]->ActiveSkill(AnimationsType::SkillQ);
		}
	}
	else if (GetAsyncKeyState('W') & 0x0001)
	{
		if (m_ppObjects[0]->GetPlayerStatus()->WSkillCoolTime >= 1.f)
		{
			m_ppObjects[0]->ActiveSkill(AnimationsType::SkillW);
		}
	}
	else if (GetAsyncKeyState('E') & 0x0001)
	{
		if (m_ppObjects[0]->GetPlayerStatus()->ESkillCoolTime >= 1.f)
		{
			m_ppObjects[0]->ActiveSkill(AnimationsType::SkillE);
		}
	}
	else if (GetAsyncKeyState('R') & 0x0001)
	{
		if (m_ppObjects[0]->GetPlayerStatus()->RSkillCoolTime >= 1.f)
		{
			m_ppObjects[0]->ActiveSkill(AnimationsType::SkillR);
		}
	}

	else if (GetAsyncKeyState('1') & 0x0001)
	{
	}

	return true;
}

void CPlayerShader::SetColManagerToObject(shared_ptr<CCollisionManager> manager)
{
	for (int i = 0; i < 4; ++i) {
		m_ppObjects[i]->SetCollisionManager(manager);
	}
}

void CPlayerShader::SetThrowingManagerToObject(shared_ptr<CThrowingMgr> manager)
{
	for (int i = 0; i < m_nObjects; ++i)
	{
		m_ppObjects[i]->SetThrowingManager(manager);
	}
}

void CPlayerShader::SetEffectManagerToObject(shared_ptr<CEffectMgr> manager)
{
	for (int i = 0; i < 4; ++i) {
		m_ppObjects[i]->SetEffectManager(manager);
	}
}
void CPlayerShader::SetSoundManagerToObject(shared_ptr<CSoundManager> manager)
{
	for (int i = 0; i < 4; ++i) {
		m_ppObjects[i]->SetSoundManager(manager);
	}
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
D3D12_INPUT_LAYOUT_DESC CPlayerShader::CreateInputLayout()
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

D3D12_SHADER_BYTECODE CPlayerShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/BoneShader.hlsl",
		"VSBone",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CPlayerShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/BoneShader.hlsl",
		"PSBone",
		"ps_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CPlayerShader::CreateShadowVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/ShadowShader.hlsl",
		"VSBone",
		"vs_5_1",
		pShaderBlob));
}

void CPlayerShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 3;

	m_nHeaps = 2;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CPlayerShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void *pContext)
{
	if (pContext) m_pTerrain = (CHeightMapTerrain*)pContext;

	m_nObjects = 4;
	m_ppObjects = new CCollisionObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_ANIOBJECT_INFO) + 255) & ~255);
	UINT boundingBoxElementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);


	int accCnt{ 0 };

	CreateShaderVariables(pCreateMgr, ncbElementBytes, m_nObjects * 2, true, ncbElementBytes, m_nObjects);
	for (int i = 0; i < m_nHeaps - 1; ++i)
	{
		CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 1, i);
		CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pConstBuffer.Get(), ncbElementBytes, accCnt, i);
		accCnt += m_nObjects;
	}
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, m_nObjects, 0, m_nHeaps - 1);
	CreateConstantBufferViews(pCreateMgr, m_nObjects, m_pBoundingBoxBuffer.Get(), boundingBoxElementBytes, 0, m_nHeaps - 1);

	SaveBoundingBoxHeapNumber(m_nHeaps - 1);


#if USE_BATCH_MATERIAL

#else
	CMaterial *pCubeMaterial = Materials::CreateBrickMaterial(pCreateMgr, &m_srvCPUDescriptorStartHandle, &m_srvGPUDescriptorStartHandle);
#endif
	m_nMaterials = m_nHeaps - 1;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	m_ppMaterials[0] = Materials::CreatePlayerMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	CSkinnedMesh *pPlayerMesh = new CSkinnedMesh(pCreateMgr, "Resource//3D//Player//Mesh//Player.meshinfo");

	CCubeMesh *pBoundingBoxMesh = new CCubeMesh(pCreateMgr,
		CONVERT_PaperUnit_to_InG(2.0f), CONVERT_PaperUnit_to_InG(1.0f), CONVERT_PaperUnit_to_InG(10.0f),
		0, 0, -CONVERT_PaperUnit_to_InG(6.5f));

	CSkeleton *pWin = new CSkeleton("Resource//3D//Player//Animation//Player_Win.aniinfo");
	CSkeleton *pDefeat = new CSkeleton("Resource//3D//Player//Animation//Player_Defeat.aniinfo");
	CSkeleton *pDefeat2 = new CSkeleton("Resource//3D//Player//Animation//Player_Defeat2.aniinfo");
	CSkeleton *pDie = new CSkeleton("Resource//3D//Player//Animation//Player_Die.aniinfo");
	//new CSkeleton("Resource//3D//Player//Animation//Player_Die.aniinfo"); �÷��̾� ����
	//new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Attack.aniinfo"); Į ����

	m_ppSwordAni = new CSkeleton*[8];

	m_ppSwordAni[0] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Idle.aniinfo");
	m_ppSwordAni[1] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Start_Walk.aniinfo");
	m_ppSwordAni[2] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Walk.aniinfo");

	m_ppSwordAni[3] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Smash.aniinfo");
	m_ppSwordAni[4] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Slash.aniinfo");
	m_ppSwordAni[5] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Dash.aniinfo");
	m_ppSwordAni[6] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Dispute.aniinfo");
	m_ppSwordAni[7] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Attack.aniinfo");

	m_ppStaffAni = new CSkeleton*[8];

	m_ppStaffAni[0] = new CSkeleton("Resource//3D//Player//Animation//Staff//Player_Staff_Idle.aniinfo");
	//�ӽ�
	m_ppStaffAni[1] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Start_Walk.aniinfo");
	m_ppStaffAni[2] = new CSkeleton("Resource//3D//Player//Animation//Sword//Player_Sword_Walk.aniinfo");
	///////////////////////////////////////////////////////

	//new CSkeleton("Resource//3D//Player//Animation//Staff//Player_Staff_Attack.aniinfo");���� ����
	m_ppStaffAni[3] = new CSkeleton("Resource//3D//Player//Animation//Staff//Player_Staff_SkillA.aniinfo");
	m_ppStaffAni[4] = new CSkeleton("Resource//3D//Player//Animation//Staff//Player_Staff_SkillB.aniinfo");
	m_ppStaffAni[5] = new CSkeleton("Resource//3D//Player//Animation//Staff//Player_Staff_SkillC.aniinfo");
	m_ppStaffAni[6] = new CSkeleton("Resource//3D//Player//Animation//Staff//Player_Staff_SkillD.aniinfo");
	m_ppStaffAni[7] = new CSkeleton("Resource//3D//Player//Animation//Staff//Player_Staff_Attack.aniinfo");

	m_ppBowAni = new CSkeleton*[8];

	m_ppBowAni[0] = new CSkeleton("Resource//3D//Player//Animation//Bow//Player_Bow_Idle.aniinfo");
	m_ppBowAni[1] = new CSkeleton("Resource//3D//Player//Animation//Bow//Player_Bow_Start_Walk.aniinfo");
	m_ppBowAni[2] = new CSkeleton("Resource//3D//Player//Animation//Bow//Player_Bow_Walk.aniinfo");
	m_ppBowAni[3] = new CSkeleton("Resource//3D//Player//Animation//Bow//Player_Bow_Attack.aniinfo");
	for (int j = 4; j < 8; ++j) {
		m_ppBowAni[j] = m_ppBowAni[3];
	}

	pPlayerMesh->SetBoundingBox(
		XMFLOAT3(0.0f, 0.0f, -CONVERT_PaperUnit_to_InG(6.5f)),
		XMFLOAT3(CONVERT_PaperUnit_to_InG(1.0f), CONVERT_PaperUnit_to_InG(1.0f), CONVERT_PaperUnit_to_InG(5.0f)));

	int i = 0;
	UINT incrementSize{ pCreateMgr->GetCbvSrvDescriptorIncrementSize() };
	CPlayer *pPlayer = NULL;



	for (int x = 0; x < m_nObjects / 2; ++x) {
		for (int z = 0; z < m_nObjects / 2; ++z) {
			if (z == 0 && x == 1) {
				pPlayer = new CPlayerAI(pCreateMgr, 1);
			}
			else {
				pPlayer = new CPlayer(pCreateMgr, 1);
			}
			pPlayer->SetMesh(0, pPlayerMesh);

			pPlayer->SetType(ObjectType::StickPlayer);
#if !USE_BATCH_MATERIAL
			pRotatingObject->SetMaterial(pCubeMaterial);
#endif
			pPlayer->SetBoundingMesh(pBoundingBoxMesh);
			pPlayer->SetCollisionSize(CONVERT_PaperUnit_to_InG(3));

			pPlayer->tag = i;
			pPlayer->CBaseObject::SetPosition(500.0f + (z * 9000.0f), 0.0f, 2000.0f + (x * 1000.0f));
			if (z == 1) {
				pPlayer->SetTeam(TeamType::Red);
			}
			else
				pPlayer->SetTeam(TeamType::Blue);


			pPlayer->SetSkeleton(pWin);
			pPlayer->SetSkeleton(pDefeat);
			pPlayer->SetSkeleton(pDefeat2);
			pPlayer->SetSkeleton(pDie);

			pPlayer->SetSkeleton(m_ppSwordAni[0]);
			pPlayer->SetSkeleton(m_ppSwordAni[1]);
			pPlayer->SetSkeleton(m_ppSwordAni[2]);

			pPlayer->SetSkeleton(m_ppSwordAni[3]);
			pPlayer->SetSkeleton(m_ppSwordAni[4]);
			pPlayer->SetSkeleton(m_ppSwordAni[5]);
			pPlayer->SetSkeleton(m_ppSwordAni[6]);
			pPlayer->SetSkeleton(m_ppSwordAni[7]);


			pPlayer->SetTerrain(m_pTerrain);

			pPlayer->Rotate(90, 0, 0);
			pPlayer->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (incrementSize * i));
			pPlayer->SetCbvGPUDescriptorHandlePtrForBB(m_pcbvGPUDescriptorStartHandle[m_nHeaps - 1].ptr + (incrementSize * i));

			pPlayer->SaveCurrentState();
			m_ppObjects[i++] = pPlayer;
		}
	}

}

void CPlayerShader::ReleaseObjects()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			delete m_ppObjects[j];
		}
		Safe_Delete_Array(m_ppObjects);
	}
	//�ִϸ��̼� 
	for (UINT j = 0; j < 8; j++)
	{
		delete m_ppSwordAni[j];
	}
	Safe_Delete_Array(m_ppSwordAni);

	for (UINT j = 0; j < 8; j++)
	{
		delete m_ppStaffAni[j];
	}
	Safe_Delete_Array(m_ppStaffAni);

	for (UINT j = 0; j < 4; j++)
	{
		delete m_ppBowAni[j];
	}
	Safe_Delete_Array(m_ppBowAni);
	//////////////////////////////////////
	//�޽�


	///////////////////////////////////////
#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i < m_nMaterials; ++i)
		{
			if (m_ppMaterials[i]) Safe_Delete(m_ppMaterials[i]);
		}
		Safe_Delete_Array(m_ppMaterials);
	}
#endif
}