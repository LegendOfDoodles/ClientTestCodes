#include "stdafx.h"
#include "MinimapIconShader.h"

CMinimapIconShader::CMinimapIconShader(CCreateMgr *pCreateMgr)
	: CShader(pCreateMgr)
{

}

CMinimapIconShader::~CMinimapIconShader()
{
}

void CMinimapIconShader::ReleaseUploadBuffers()
{
	if (m_ppObjects)
	{
		for (int j = 0; j < m_nObjects; j++)
		{
			m_ppObjects[j]->ReleaseUploadBuffers();
		}
	}

#if USE_BATCH_MATERIAL
	if (m_ppMaterials)
	{
		for (int i = 0; i<m_nMaterials; ++i)
			m_ppMaterials[i]->ReleaseUploadBuffers();
	}
#endif
}

void CMinimapIconShader::UpdateShaderVariables()
{
#if USE_INSTANCING
	m_pCommandList->SetGraphicsRootShaderResourceView(2,
		m_pInstanceBuffer->GetGPUVirtualAddress());

	for (int i = 0; i < m_nObjects; i++)
	{
		XMStoreFloat4x4(&m_pMappedObjects[i].m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
#else
	static UINT elementBytes = ((sizeof(CB_GAMEOBJECT_INFO) + 255) & ~255);

	for (int i = 0; i < m_nObjects; i++)
	{
		CB_GAMEOBJECT_INFO *pMappedObject = (CB_GAMEOBJECT_INFO *)(m_pMappedObjects + (i * elementBytes));
		XMStoreFloat4x4(&pMappedObject->m_xmf4x4World,
			XMMatrixTranspose(XMLoadFloat4x4(m_ppObjects[i]->GetWorldMatrix())));
	}
#endif
}

void CMinimapIconShader::AnimateObjects(float timeElapsed)
{
	for (int j = 0; j < m_nObjects; j++)
	{
		m_ppObjects[j]->Animate(timeElapsed);

		XMFLOAT3 HPGaugePosition = m_pPlayer[j]->GetPosition();
		HPGaugePosition.y += 110.f;
		dynamic_cast<CHPGaugeObjects*>(m_ppObjects[j])->SetPosition(HPGaugePosition);
	}
}

void CMinimapIconShader::Render(CCamera * pCamera)
{
}

void CMinimapIconShader::GetCamera(CCamera * pCamera)
{
}

bool CMinimapIconShader::OnProcessKeyInput(UCHAR * pKeyBuffer)
{
	return false;
}

bool CMinimapIconShader::OnProcessMouseInput(WPARAM pKeyBuffer)
{
	return false;
}

D3D12_INPUT_LAYOUT_DESC CMinimapIconShader::CreateInputLayout()
{
	return D3D12_INPUT_LAYOUT_DESC();
}

D3D12_BLEND_DESC CMinimapIconShader::CreateBlendState()
{
	return D3D12_BLEND_DESC();
}

D3D12_SHADER_BYTECODE CMinimapIconShader::CreateVertexShader(ID3DBlob ** ppShaderBlob)
{
	return D3D12_SHADER_BYTECODE();
}

D3D12_SHADER_BYTECODE CMinimapIconShader::CreatePixelShader(ID3DBlob ** ppShaderBlob)
{
	return D3D12_SHADER_BYTECODE();
}

void CMinimapIconShader::CreateShader(CCreateMgr * pCreateMgr)
{
}

void CMinimapIconShader::CreateShaderVariables(CCreateMgr * pCreateMgr, int nBuffers)
{
}

void CMinimapIconShader::BuildObjects(CCreateMgr * pCreateMgr, void * pContext)
{
}

void CMinimapIconShader::ReleaseShaderVariables()
{
}

void CMinimapIconShader::ReleaseObjects()
{
}
