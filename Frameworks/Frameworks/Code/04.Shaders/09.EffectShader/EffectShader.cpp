#include "stdafx.h"
#include "EffectShader.h"
#include "05.Objects\96.Billboard\06.EffectObject\EffectObject.h"
#include "02.Framework/01.CreateMgr/CreateMgr.h"
#include "05.Objects/99.Material/Material.h"

/// <summary>
/// ����: Effect �׸��� ���̴�
/// ���� ������:  �̿뼱
/// ������ ���:  �̿뼱
/// ���� ���� ��¥: 2018-08-06
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CEffectShader::CEffectShader(shared_ptr<CCreateMgr> pCreateMgr)
	:CShader(pCreateMgr)
{
}

CEffectShader::~CEffectShader()
{
}

void CEffectShader::ReleaseUploadBuffers()
{
	//if (m_ppObjects)
	//{
	//	for (int j = 0; j < m_nObjects; j++)
	//	{
	//		m_ppObjects[j]->ReleaseUploadBuffers();
	//	}
	//}

	//if (m_ppMaterials)
	//{
	//	for (int i = 0; i<m_nMaterials; ++i)
	//		m_ppMaterials[i]->ReleaseUploadBuffers();
	//}

	//for (int j = 0; j < m_nMesh; j++)
	//{
	//	m_ppMesh[j]->ReleaseUploadBuffers();
	//}
}

void CEffectShader::UpdateShaderVariables(int opt)
{
	static UINT elementBytes = ((sizeof(CB_EFFECTOBJECT_INFO) + 255) & ~255);
	int beg{ 0 }, end{ 0 };

	switch (opt)
	{
	case 0:
		beg = 0;
		end = UseMatrialNumToObjectCnt[0];
		break;
	case 1:
		beg = UseMatrialNumToObjectCnt[0];
		end = UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1];
		break;
	case 2:
		beg = UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1];
		end = UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2];
		break;
	case 3:
		beg = UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2];
		end = UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2] + UseMatrialNumToObjectCnt[3];
		break;
	}

	for (int i = beg; i < end; ++i)
	{
		CB_EFFECTOBJECT_INFO *pMappedObject = (CB_EFFECTOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		// Animation Time
		pMappedObject->m_fAnimationTime = ((CEffectObject*)m_ppObjects[i])->GetAnimationTime();
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
}

void CEffectShader::AnimateObjects(float timeElapsed)
{
	// ����Ʈ���� ������ ���� �Լ�
	static auto removeFunc = [this](CCollisionObject* obj) {
		if (obj->GetState() == StatesType::Remove)
		{
			ResetPossibleIndex(obj->GetIndex());
			obj->Deactivate();
			return true;
		}
		return false;
	};

	if (m_Paused) return;
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);
	}

	// �� �̻� ������Ʈ �ϸ� �ȵǴ� ������Ʈ ����Ʈ���� ����
	m_PlayerSwordSkill_Q_EffectList.remove_if(removeFunc);
	m_PlayerSwordSkill_W_EffectList.remove_if(removeFunc);
	m_PlayerSwordSkill_E_EffectList.remove_if(removeFunc);
	m_PlayerSwordSkill_R_EffectList.remove_if(removeFunc);

	m_PlayerStaffSkill_W_EffectList.remove_if(removeFunc);
	m_PlayerStaffSkill_E_EffectList.remove_if(removeFunc);
	
	m_PlayerArrowAttack_EffectList.remove_if(removeFunc);
	m_PlayerStaffAttack_EffectList.remove_if(removeFunc);
	m_PlayerStaffQSkill_EffectList.remove_if(removeFunc);
	m_PlayerStaffESkill_EffectList.remove_if(removeFunc);

	m_MinionArrowAttack_EffectList.remove_if(removeFunc);
	m_MinionStaffAttack_EffectList.remove_if(removeFunc);

	m_MinionArrow_EffectList.remove_if(removeFunc);
	m_PlayerArrow_EffectList.remove_if(removeFunc);
	m_PlayerFireBall_EffectList.remove_if(removeFunc);
	m_PlayerArrowAndFireBall_HitPosition_EffectList.remove_if(removeFunc);
}

void CEffectShader::Render(CCamera * pCamera)
{
	CShader::Render(pCamera, 0);
	
	if (!m_PlayerSwordSkill_Q_EffectList.empty())
	{
		m_ppMaterials[0]->UpdateShaderVariable(0);
		for (auto iter = m_PlayerSwordSkill_Q_EffectList.begin(); iter != m_PlayerSwordSkill_Q_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_PlayerSwordSkill_W_EffectList.empty())
	{
		m_ppMaterials[0]->UpdateShaderVariable(1);
		for (auto iter = m_PlayerSwordSkill_W_EffectList.begin(); iter != m_PlayerSwordSkill_W_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_PlayerSwordSkill_E_EffectList.empty())
	{
		m_ppMaterials[0]->UpdateShaderVariable(2);
		for (auto iter = m_PlayerSwordSkill_E_EffectList.begin(); iter != m_PlayerSwordSkill_E_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_PlayerSwordSkill_R_EffectList.empty())
	{
		m_ppMaterials[0]->UpdateShaderVariable(3);
		for (auto iter = m_PlayerSwordSkill_R_EffectList.begin(); iter != m_PlayerSwordSkill_R_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	
	if (!m_PlayerStaffSkill_W_EffectList.empty())
	{
		m_ppMaterials[0]->UpdateShaderVariable(4);
		for (auto iter = m_PlayerStaffSkill_W_EffectList.begin(); iter != m_PlayerStaffSkill_W_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_PlayerStaffSkill_E_EffectList.empty())
	{
		m_ppMaterials[0]->UpdateShaderVariable(5);
		for (auto iter = m_PlayerStaffSkill_E_EffectList.begin(); iter != m_PlayerStaffSkill_E_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}

	CShader::Render(pCamera, 1);

	if (!m_PlayerArrowAttack_EffectList.empty())
	{
		m_ppMaterials[1]->UpdateShaderVariable(0);
		for (auto iter = m_PlayerArrowAttack_EffectList.begin(); iter != m_PlayerArrowAttack_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_PlayerStaffAttack_EffectList.empty())
	{
		m_ppMaterials[1]->UpdateShaderVariable(1);
		for (auto iter = m_PlayerStaffAttack_EffectList.begin(); iter != m_PlayerStaffAttack_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_PlayerStaffQSkill_EffectList.empty())
	{
		m_ppMaterials[1]->UpdateShaderVariable(2);
		for (auto iter = m_PlayerStaffQSkill_EffectList.begin(); iter != m_PlayerStaffQSkill_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_PlayerStaffESkill_EffectList.empty())
	{
		m_ppMaterials[1]->UpdateShaderVariable(3);
		for (auto iter = m_PlayerStaffESkill_EffectList.begin(); iter != m_PlayerStaffESkill_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}

	CShader::Render(pCamera, 2);
	if (!m_MinionArrowAttack_EffectList.empty())
	{
		m_ppMaterials[2]->UpdateShaderVariable(0);
		for (auto iter = m_MinionArrowAttack_EffectList.begin(); iter != m_MinionArrowAttack_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_MinionStaffAttack_EffectList.empty())
	{
		m_ppMaterials[2]->UpdateShaderVariable(1);
		for (auto iter = m_MinionStaffAttack_EffectList.begin(); iter != m_MinionStaffAttack_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}

	CShader::Render(pCamera, 3);
	if (!m_MinionArrow_EffectList.empty())
	{
		m_ppMaterials[3]->UpdateShaderVariable(0);
		for (auto iter = m_MinionArrow_EffectList.begin(); iter != m_MinionArrow_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_PlayerArrow_EffectList.empty())
	{
		m_ppMaterials[3]->UpdateShaderVariable(1);
		for (auto iter = m_PlayerArrow_EffectList.begin(); iter != m_PlayerArrow_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_PlayerFireBall_EffectList.empty())
	{
		m_ppMaterials[3]->UpdateShaderVariable(2);
		for (auto iter = m_PlayerFireBall_EffectList.begin(); iter != m_PlayerFireBall_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}
	if (!m_PlayerArrowAndFireBall_HitPosition_EffectList.empty())
	{
		m_ppMaterials[3]->UpdateShaderVariable(3);
		for (auto iter = m_PlayerArrowAndFireBall_HitPosition_EffectList.begin(); iter != m_PlayerArrowAndFireBall_HitPosition_EffectList.end(); ++iter)
		{
			(*iter)->Render(pCamera);
		}
	}


}

void CEffectShader::SpawnEffectObject(const XMFLOAT3 & position, const XMFLOAT3 & direction, int aniLength, EffectObjectType objectType)
{
	int idx{ GetPossibleIndex(objectType) };

	if (idx != NONE)
	{
		m_ppObjects[idx]->ResetWorldMatrix();
		m_ppObjects[idx]->SaveIndex(idx);

		m_ppObjects[idx]->SetDirection(direction);
		m_ppObjects[idx]->SetEffectObjectsType(objectType);
		m_ppObjects[idx]->SetAnimationLength(aniLength);

		m_ppObjects[idx]->Activate();
		if (objectType == EffectObjectType::Player_SwordSkill_Q_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[3]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x + (CONVERT_PaperUnit_to_InG(8) * direction.x), position.y + CONVERT_PaperUnit_to_InG(2), position.z + (CONVERT_PaperUnit_to_InG(8) * direction.z)));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (m_srvIncrementSize * idx));
			m_PlayerSwordSkill_Q_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Player_SwordSkill_W_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[3]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(4), position.z ));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (m_srvIncrementSize * idx));
			m_PlayerSwordSkill_W_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Player_SwordSkill_E_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[3]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x + (CONVERT_PaperUnit_to_InG(10) * direction.x), position.y + 50.f, position.z + (CONVERT_PaperUnit_to_InG(10) * direction.z)));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (m_srvIncrementSize * idx));
			m_PlayerSwordSkill_E_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Player_SwordSkill_R_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[3]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + CONVERT_PaperUnit_to_InG(2), position.z));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (m_srvIncrementSize * idx));
			m_PlayerSwordSkill_R_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Player_StaffSkill_W_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[3]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x +(CONVERT_PaperUnit_to_InG(8) * direction.x), position.y + 50.f, position.z + (CONVERT_PaperUnit_to_InG(8) * direction.z)));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (m_srvIncrementSize * idx));
			m_PlayerStaffSkill_W_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Player_StaffSkill_E_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[2]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x + (CONVERT_PaperUnit_to_InG(15) * direction.x), position.y + 50.f, position.z + (CONVERT_PaperUnit_to_InG(15) * direction.z)));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[0].ptr + (m_srvIncrementSize * idx));
			m_PlayerStaffSkill_E_EffectList.emplace_back(m_ppObjects[idx]);
		}

		// Player Attack Motion Effect
		else if (objectType == EffectObjectType::Player_ArrowAttack_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[2]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + 55.f, position.z ));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[1].ptr + (m_srvIncrementSize * (idx - UseMatrialNumToObjectCnt[0])));
			m_PlayerArrowAttack_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Player_StaffAttack_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[3]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + 55.f, position.z ));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[1].ptr + (m_srvIncrementSize * (idx - UseMatrialNumToObjectCnt[0])));
			m_PlayerStaffAttack_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Player_StaffQSkill_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[1]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + 50.f, position.z ));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[1].ptr + (m_srvIncrementSize *  (idx - UseMatrialNumToObjectCnt[0])));
			m_PlayerStaffQSkill_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Player_StaffESkill_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[1]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + 55.f, position.z ));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[1].ptr + (m_srvIncrementSize * (idx - UseMatrialNumToObjectCnt[0])));
			m_PlayerStaffESkill_EffectList.emplace_back(m_ppObjects[idx]);
		}
		

		// Minion Attack Motion Effect
		else if (objectType == EffectObjectType::Minion_ArrowAttack_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[1]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + 40.f, position.z - CONVERT_PaperUnit_to_InG(1)));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[2].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1]))));
			m_MinionArrowAttack_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Minion_StaffAttack_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[0]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x + (CONVERT_PaperUnit_to_InG(2) * direction.x), position.y + CONVERT_PaperUnit_to_InG(4), position.z + (CONVERT_PaperUnit_to_InG(4)  * direction.z)));

			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[2].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1]))));
			m_MinionStaffAttack_EffectList.emplace_back(m_ppObjects[idx]);
		}

		// FollowingToFlyingObejct Effect
		else if (objectType == EffectObjectType::Flying_MinionArrow_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[0]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + 4.f, position.z));
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[3].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2]))));
			m_MinionArrow_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Flying_PlayerArrow_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[0]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + 4.f, position.z));
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[3].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2]))));
			m_PlayerArrow_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Flying_PlayerFireBall_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[0]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + 4.f, position.z));
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[3].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2]))));
			m_PlayerFireBall_EffectList.emplace_back(m_ppObjects[idx]);
		}
		else if (objectType == EffectObjectType::Player_ArrowAndFireBall_HitPosition_Effect)
		{
			m_ppObjects[idx]->SetMesh(0, m_ppMesh[1]);
			m_ppObjects[idx]->SetPosition(XMFLOAT3(position.x, position.y + 50.f, position.z - CONVERT_PaperUnit_to_InG(2)));
			m_ppObjects[idx]->SetCbvGPUDescriptorHandlePtr(m_pcbvGPUDescriptorStartHandle[3].ptr + (m_srvIncrementSize * (idx - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2]))));
			m_PlayerArrowAndFireBall_HitPosition_EffectList.emplace_back(m_ppObjects[idx]);
		}
	}
}

D3D12_INPUT_LAYOUT_DESC CEffectShader::CreateInputLayout()
{
	UINT nInputElementDescs = 2;
	D3D12_INPUT_ELEMENT_DESC *pInputElementDescs = new D3D12_INPUT_ELEMENT_DESC[nInputElementDescs];

	pInputElementDescs[0] = {
		"POSITION",
		0,
		DXGI_FORMAT_R32G32B32_FLOAT,
		0,
		0,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };
	pInputElementDescs[1] = {
		"TEXCOORD",
		0,
		DXGI_FORMAT_R32G32_FLOAT,
		0,
		12,
		D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
		0 };

	D3D12_INPUT_LAYOUT_DESC d3dInputLayoutDesc;
	d3dInputLayoutDesc.pInputElementDescs = pInputElementDescs;
	d3dInputLayoutDesc.NumElements = nInputElementDescs;

	return(d3dInputLayoutDesc);
}

D3D12_BLEND_DESC CEffectShader::CreateBlendState()
{
	D3D12_BLEND_DESC blendDesc;
	::ZeroMemory(&blendDesc, sizeof(D3D12_BLEND_DESC));

	blendDesc.AlphaToCoverageEnable = TRUE;
	blendDesc.IndependentBlendEnable = FALSE;
	blendDesc.RenderTarget[0].BlendEnable = FALSE;
	blendDesc.RenderTarget[0].LogicOpEnable = FALSE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].LogicOp = D3D12_LOGIC_OP_NOOP;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;

	return(blendDesc);
}

D3D12_SHADER_BYTECODE CEffectShader::CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	//./Code/04.Shaders/99.GraphicsShader/
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"VSTexturedEffect",
		"vs_5_1",
		pShaderBlob));
}

D3D12_SHADER_BYTECODE CEffectShader::CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob)
{
	return(CShader::CompileShaderFromFile(
		L"./code/04.Shaders/99.GraphicsShader/Shaders.hlsl",
		"PSTexturedEffect",
		"ps_5_1",
		pShaderBlob));
}

void CEffectShader::CreateShader(shared_ptr<CCreateMgr> pCreateMgr, UINT nRenderTargets, bool isRenderBB, bool isRenderShadow)
{
	m_nPipelineStates = 1;

	m_nHeaps = m_nMesh;
	CreateDescriptorHeaps();

	CShader::CreateShader(pCreateMgr, nRenderTargets, isRenderBB, isRenderShadow);
}

void CEffectShader::BuildObjects(shared_ptr<CCreateMgr> pCreateMgr, void * pContext)
{
	m_pCamera = (CCamera*)pContext;

	// ������Ʈ ���� ����
	EffectObjectType objectOrder[]{
		EffectObjectType::Player_SwordSkill_Q_Effect,
		EffectObjectType::Player_SwordSkill_W_Effect,
		EffectObjectType::Player_SwordSkill_E_Effect,
		EffectObjectType::Player_SwordSkill_R_Effect,

		EffectObjectType::Player_StaffSkill_W_Effect,
		EffectObjectType::Player_StaffSkill_E_Effect,

		EffectObjectType::Player_ArrowAttack_Effect,
		EffectObjectType::Player_StaffAttack_Effect,
		EffectObjectType::Player_StaffQSkill_Effect,
		EffectObjectType::Player_StaffESkill_Effect,

		EffectObjectType::Minion_ArrowAttack_Effect,
		EffectObjectType::Minion_StaffAttack_Effect,

		EffectObjectType::Flying_MinionArrow_Effect,
		EffectObjectType::Flying_PlayerArrow_Effect,
		EffectObjectType::Flying_PlayerFireBall_Effect,
		EffectObjectType::Player_ArrowAndFireBall_HitPosition_Effect,
	};

	// �� ������Ʈ�� �ִ� ���� ����
	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_SwordSkill_Q_Effect] = MAX_SKILL;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_SwordSkill_W_Effect] = MAX_SKILL;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_SwordSkill_E_Effect] = MAX_SKILL;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_SwordSkill_R_Effect] = MAX_SKILL;

	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_StaffSkill_W_Effect] = MAX_SKILL;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_StaffSkill_E_Effect] = MAX_SKILL;
	// 0�� Matrial�� ����ϴ� Obejct ����
	UseMatrialNumToObjectCnt[0] = m_nObjects;

	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_ArrowAttack_Effect] = MAX_SKILL;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_StaffAttack_Effect] = MAX_SKILL;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_StaffQSkill_Effect] = MAX_SKILL;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_StaffESkill_Effect] = MAX_SKILL;
	// 1�� Matrial�� ����ϴ� Obejct ����
	UseMatrialNumToObjectCnt[1] = (m_nObjects - UseMatrialNumToObjectCnt[0]);

	m_nObjects += m_objectsMaxCount[EffectObjectType::Minion_ArrowAttack_Effect] = MAX_ARROW;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Minion_StaffAttack_Effect] = MAX_MAGIC;
	// 2�� Matrial�� ����ϴ� Obejct ����
	UseMatrialNumToObjectCnt[2] = (m_nObjects - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1]));

	m_nObjects += m_objectsMaxCount[EffectObjectType::Flying_MinionArrow_Effect] = MAX_ARROW;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Flying_PlayerArrow_Effect] = MAX_ARROW;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Flying_PlayerFireBall_Effect] = MAX_MAGIC;
	m_nObjects += m_objectsMaxCount[EffectObjectType::Player_ArrowAndFireBall_HitPosition_Effect] = MAX_HIT;
	// 3�� Matrial�� ����ϴ� Obejct ����
	UseMatrialNumToObjectCnt[3] = (m_nObjects - (UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2]));

	// �� ������Ʈ ���� ��ŭ Possible Index ����
	m_objectsPossibleIndices = std::unique_ptr<bool[]>(new bool[m_nObjects]);

	// ������ Possible Indices�� 0(false)�� �ʱ�ȭ
	memset(m_objectsPossibleIndices.get(), false, m_nObjects * sizeof(bool));

	m_ppObjects = new CCollisionObject*[m_nObjects];

	UINT ncbElementBytes = ((sizeof(CB_EFFECTOBJECT_INFO) + 255) & ~255);
	int accCnt{ 0 };

	CreateShaderVariables(pCreateMgr, ncbElementBytes, m_nObjects);

	// Matrial ���� �� ����
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, UseMatrialNumToObjectCnt[0], 6, 0);
	CreateConstantBufferViews(pCreateMgr, UseMatrialNumToObjectCnt[0], m_pConstBuffer.Get(), ncbElementBytes, 0, 0);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, UseMatrialNumToObjectCnt[1], 4, 1);
	CreateConstantBufferViews(pCreateMgr, UseMatrialNumToObjectCnt[1], m_pConstBuffer.Get(), ncbElementBytes, UseMatrialNumToObjectCnt[0], 1);
	
	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, UseMatrialNumToObjectCnt[2], 2, 2);
	CreateConstantBufferViews(pCreateMgr, UseMatrialNumToObjectCnt[2], m_pConstBuffer.Get(), ncbElementBytes, UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1], 2);

	CreateCbvAndSrvDescriptorHeaps(pCreateMgr, UseMatrialNumToObjectCnt[3], 4, 3);
	CreateConstantBufferViews(pCreateMgr, UseMatrialNumToObjectCnt[3], m_pConstBuffer.Get(), ncbElementBytes, UseMatrialNumToObjectCnt[0] + UseMatrialNumToObjectCnt[1] + UseMatrialNumToObjectCnt[2], 3);

	// ������Ʈ Index
	for (int i = 0; i < EffectObjectTime_Max_COUNT; ++i) {
		m_objectsIndices[objectOrder[i]] = EffectObjectIndices();
		m_objectsIndices[objectOrder[i]].m_begIndex = accCnt;
		accCnt += m_objectsMaxCount[objectOrder[i]];
		m_objectsIndices[objectOrder[i]].m_endIndex = accCnt;
	}
	
	m_nMaterials = m_nHeaps;
	m_ppMaterials = new CMaterial*[m_nMaterials];
	// Effect Image
	/*
		�̹��� ����
		0. 
		1. 
		2. Minion Magic Effect
	*/
	m_ppMaterials[0] = Materials::CreatePlayerSkillEffectMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[0], &m_psrvGPUDescriptorStartHandle[0]);
	m_ppMaterials[1] = Materials::CreatePlayerAttackEffectMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[1], &m_psrvGPUDescriptorStartHandle[1]);
	m_ppMaterials[2] = Materials::CreateMinionAttackEffectMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[2], &m_psrvGPUDescriptorStartHandle[2]);
	m_ppMaterials[3] = Materials::CreateFlyingObjectEffectMaterial(pCreateMgr, &m_psrvCPUDescriptorStartHandle[3], &m_psrvGPUDescriptorStartHandle[3]);

	m_ppMesh[0] = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 25.6f, FRAME_BUFFER_HEIGHT / 14.4f, 0.f);		// 50 x 50
	m_ppMesh[1] = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 12.8f, FRAME_BUFFER_HEIGHT / 7.2f, 0.f);		// 100 x 100
	m_ppMesh[2] = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 8.53f, FRAME_BUFFER_HEIGHT / 4.8f, 0.f);		// 150 x 150
	m_ppMesh[3] = new CTexturedRectMesh(pCreateMgr, FRAME_BUFFER_WIDTH / 6.4f, FRAME_BUFFER_HEIGHT / 3.6f, 0.f);		// 200 x 200

	for (int i = 0; i < m_nMesh; ++i)
	{
		m_ppMesh[i]->AddRef();
	}

	m_srvIncrementSize = pCreateMgr->GetCbvSrvDescriptorIncrementSize();

	// ������Ʈ ����
	CEffectObject *pObject{ NULL };
	for (int j = 0; j < m_nObjects; ++j) {
		pObject = new CEffectObject(pCreateMgr);

		pObject->SetStatic(StaticType::Static);
		pObject->SetCamera(m_pCamera);

		m_ppObjects[j] = pObject;
	}
}

void CEffectShader::ReleaseObjects()
{
	if(!m_PlayerSwordSkill_Q_EffectList.empty()) m_PlayerSwordSkill_Q_EffectList.clear();
	if(!m_PlayerSwordSkill_W_EffectList.empty()) m_PlayerSwordSkill_W_EffectList.clear();
	if (!m_PlayerSwordSkill_E_EffectList.empty()) m_PlayerSwordSkill_E_EffectList.clear();
	if (!m_PlayerSwordSkill_R_EffectList.empty()) m_PlayerSwordSkill_R_EffectList.clear();

	if(!m_PlayerStaffSkill_W_EffectList.empty()) m_PlayerStaffSkill_W_EffectList.clear();
	if(!m_PlayerStaffSkill_E_EffectList.empty()) m_PlayerStaffSkill_E_EffectList.clear();

	if(!m_PlayerArrowAttack_EffectList.empty()) m_PlayerArrowAttack_EffectList.clear();
	if(!m_PlayerStaffAttack_EffectList.empty()) m_PlayerStaffAttack_EffectList.clear();
	if(!m_PlayerStaffESkill_EffectList.empty()) m_PlayerStaffESkill_EffectList.clear();
	if(!m_PlayerStaffQSkill_EffectList.empty()) m_PlayerStaffQSkill_EffectList.clear();

	if(!m_MinionArrowAttack_EffectList.empty()) m_MinionArrowAttack_EffectList.clear();
	if(!m_MinionStaffAttack_EffectList.empty()) m_MinionStaffAttack_EffectList.clear();

	if(!m_MinionArrow_EffectList.empty()) m_MinionArrow_EffectList.clear();
	if(!m_PlayerArrow_EffectList.empty()) m_PlayerArrow_EffectList.clear();
	if(!m_PlayerFireBall_EffectList.empty()) m_PlayerFireBall_EffectList.clear();
	if(!m_PlayerArrowAndFireBall_HitPosition_EffectList.empty()) m_PlayerArrowAndFireBall_HitPosition_EffectList.clear();

	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			delete m_ppObjects[j];
		}
		Safe_Delete_Array(m_ppObjects);
	}

	if (m_ppMaterials)
	{
		for (int i = 0; i < m_nMaterials; ++i)
		{
			Safe_Delete(m_ppMaterials[i]);
		}
		Safe_Delete_Array(m_ppMaterials);
	}

	for (int j = 0; j < m_nMesh; j++)
	{
		m_ppMesh[j]->ReleaseUploadBuffers();
	}
}

int CEffectShader::GetPossibleIndex(EffectObjectType type)
{
	for (int idx = m_objectsIndices[type].m_begIndex; idx < m_objectsIndices[type].m_endIndex; ++idx)
	{
		if (!m_objectsPossibleIndices[idx])
		{
			m_objectsPossibleIndices[idx] = true;
			return idx;
		}
	}
	return NONE;
}
