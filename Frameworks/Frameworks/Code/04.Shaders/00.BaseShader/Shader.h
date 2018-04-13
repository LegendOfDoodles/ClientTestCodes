#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"

#define BOUNDING_BOX 1

class CCreateMgr;
class CCamera;
class CTexture;

class CShader
{
public:	// 생성자, 소멸자
	CShader(CCreateMgr *pCreateMgr);
	~CShader();

public: // 공개 함수
	virtual void Initialize(CCreateMgr *pCreateMgr, void *pContext = NULL);
	virtual void Finalize();

	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();
	virtual void UpdateShaderVariable(XMFLOAT4X4 *pxmf4x4World);

	virtual void UpdateBoundingBoxShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);
	virtual void RenderBoundingBox(CCamera *pCamera);

	virtual CBaseObject *PickObjectByRayIntersection(
		XMFLOAT3& pickPosition, XMFLOAT4X4& xmf4x4View, float &nearHitDistance);

	virtual void OnProcessKeyUp(WPARAM wParam, LPARAM lParam);
	virtual void OnProcessKeyDown(WPARAM wParam, LPARAM lParam);

	bool HasBoundingBox() { return m_nPipelineStates > 1; }

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	D3D12_INPUT_LAYOUT_DESC CreateBoundingBoxInputLayout();
	D3D12_RASTERIZER_DESC CreateBoundingBoxRasterizerState();

	void CreateDescriptorHeaps();
	void CreateCbvAndSrvDescriptorHeaps(CCreateMgr *pCreateMgr, int nConstantBufferViews, int nShaderResourceViews, int index = 0);
	void CreateConstantBufferViews(CCreateMgr *pCreateMgr, int nConstantBufferViews, ID3D12Resource *pConstantBuffers, UINT nStride, int index = 0);
	void CreateShaderResourceViews(CCreateMgr *pCreateMgr, CTexture *pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ID3DBlob **ppShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ID3DBlob **ppShaderBlob);

	D3D12_SHADER_BYTECODE CreateBoundingBoxVertexShader(ID3DBlob **ppShaderBlob);
	D3D12_SHADER_BYTECODE CreateBoundingBoxPixelShader(ID3DBlob **ppShaderBlob);

	virtual void CreateShader(CCreateMgr *pCreateMgr);
	void CreateBoundingBoxShader(CCreateMgr *pCreateMgr);
	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseShaderVariables();
	virtual void ReleaseObjects();

	virtual void OnPrepareRender(int opt = 0);

	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName,
		LPCSTR pszShaderProfile, ID3DBlob **ppShaderBlob);

protected: // 변수
	int m_nReferences{ 0 };

	ID3D12PipelineState **m_ppPipelineStates{ NULL };
	int m_nPipelineStates{ 0 };

	ID3D12Resource *m_pInstanceBuffer{ NULL };
	ID3D12Resource *m_pConstBuffer{ NULL };
	ID3D12Resource *m_pBoundingBoxBuffer{ NULL };

#if USE_BATCH_MATERIAL
	CMaterial						*m_pMaterial{ NULL };
#endif

	ID3D12DescriptorHeap			**m_ppCbvSrvDescriptorHeaps{ NULL };

	D3D12_CPU_DESCRIPTOR_HANDLE		*m_pcbvCPUDescriptorStartHandle{ NULL };
	D3D12_GPU_DESCRIPTOR_HANDLE		*m_pcbvGPUDescriptorStartHandle{ NULL };
	D3D12_CPU_DESCRIPTOR_HANDLE		m_srvCPUDescriptorStartHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE		m_srvGPUDescriptorStartHandle;

	ID3D12GraphicsCommandList *m_pCommandList{ NULL };
};

