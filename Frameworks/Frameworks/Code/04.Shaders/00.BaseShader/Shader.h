#pragma once
#include "05.Objects/00.BaseObject/BaseObject.h"

#define BOUNDING_BOX 1

class CCreateMgr;
class CCamera;
class CTexture;

class CShader	// Warning!  Ani Shader가 오브젝트 쉐이더 상속받으면 적당히 기능 나눌 필요 있음
{
public:	// 생성자, 소멸자
	CShader(CCreateMgr *pCreateMgr);
	virtual ~CShader();

public: // 공개 함수
	virtual void Initialize(CCreateMgr *pCreateMgr, void *pContext = NULL);
	virtual void Finalize();

	virtual void ReleaseUploadBuffers();

	virtual void UpdateShaderVariables();
	virtual void UpdateShaderVariable(XMFLOAT4X4 *pxmf4x4World);

	virtual void UpdateBoundingBoxShaderVariables();

	virtual void AnimateObjects(float timeElapsed);

	virtual void Render(CCamera *pCamera);
	void Render(CCamera *pCamera, int opt);
	void Render(CCamera *pCamera, int opt, int index);
	virtual void RenderBoundingBox(CCamera *pCamera);
	virtual void RenderShadow(CCamera *pCamera);

	virtual CBaseObject *PickObjectByRayIntersection(
		XMFLOAT3& pickPosition, XMFLOAT4X4& xmf4x4View, float &nearHitDistance);

	virtual bool OnProcessKeyInput(UCHAR* pKeyBuffer);
	virtual bool OnProcessMouseInput(WPARAM pKeyBuffer);

	bool HasBoundingBox() { return m_isRenderBB; }

	virtual void OnStatus(int ObjectType);
	virtual void OffStatus();

	virtual void CreateGraphicsRootSignature(CCreateMgr *pCreateMgr) { pCreateMgr; }

	virtual CBaseObject * * GetCollisionObjects() { return nullptr; }

	void SaveBoundingBoxHeapNumber(int n) { m_boundingBoxHeapNumber = n; }
	ComPtr<ID3D12RootSignature> GetGraphicsRootSignature() { return(m_pGraphicsRootSignature); }

	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected: // 내부 함수
	virtual D3D12_INPUT_LAYOUT_DESC CreateInputLayout();
	virtual D3D12_RASTERIZER_DESC CreateRasterizerState();
	virtual D3D12_RASTERIZER_DESC CreateShadowRasterizerState();
	virtual D3D12_BLEND_DESC CreateBlendState();
	virtual D3D12_DEPTH_STENCIL_DESC CreateDepthStencilState();

	D3D12_INPUT_LAYOUT_DESC CreateBoundingBoxInputLayout();
	D3D12_RASTERIZER_DESC CreateBoundingBoxRasterizerState();

	void CreateDescriptorHeaps();
	void CreateCbvAndSrvDescriptorHeaps(CCreateMgr *pCreateMgr, int nConstantBufferViews, int nShaderResourceViews, int index = 0);
	void CreateConstantBufferViews(CCreateMgr *pCreateMgr, int nConstantBufferViews, ID3D12Resource *pConstantBuffers, UINT nStride, int index = 0);
	void GetShaderResourceViewDesc(D3D12_RESOURCE_DESC resourceDesc, UINT nTextureType, D3D12_SHADER_RESOURCE_VIEW_DESC *pShaderResourceViewDesc);
	void CreateShaderResourceViews(CCreateMgr *pCreateMgr, CTexture *pTexture, UINT nRootParameterStartIndex, bool bAutoIncrement, int index = 0);

	virtual D3D12_SHADER_BYTECODE CreateVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateHullShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateDomainShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreatePixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	D3D12_SHADER_BYTECODE CreateBoundingBoxVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	D3D12_SHADER_BYTECODE CreateBoundingBoxPixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual D3D12_SHADER_BYTECODE CreateShadowVertexShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateShadowHullShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateShadowDomainShader(ComPtr<ID3DBlob>& pShaderBlob);
	virtual D3D12_SHADER_BYTECODE CreateShadowPixelShader(ComPtr<ID3DBlob>& pShaderBlob);

	virtual void CreateShaderWithTess(CCreateMgr *pCreateMgr, UINT nRenderTargets = 1, bool isRenderShadow = false);
	virtual void CreateShader(CCreateMgr *pCreateMgr, UINT nRenderTargets = 1, bool isRenderBB = false, bool isRenderShadow = false);
	virtual void CreateShader(CCreateMgr *pCreateMgr, ComPtr<ID3D12RootSignature> pGraphicsRootSignature, UINT nRenderTargets = 1);

	virtual void CreateShaderVariables(CCreateMgr *pCreateMgr, int nBuffers = 1);

	virtual void BuildObjects(CCreateMgr *pCreateMgr, void *pContext = NULL);

	virtual void ReleaseShaderVariables();
	virtual void ReleaseObjects();

	virtual void OnPrepareRender(int opt = 0, int index = 0);
	void OnPrepareRenderForBB();

	D3D12_SHADER_BYTECODE CompileShaderFromFile(WCHAR *pszFileName, LPCSTR pszShaderName,
		LPCSTR pszShaderProfile, ComPtr<ID3DBlob>& pShaderBlob);

protected: // 변수
	int m_nReferences{ 0 };

	bool m_isRenderBB{ false };

	ID3D12PipelineState **m_ppPipelineStates{ NULL };
	int m_nPipelineStates{ 0 };

	ID3D12Resource *m_pInstanceBuffer{ NULL };
	ID3D12Resource *m_pConstBuffer{ NULL };
	ID3D12Resource *m_pBoundingBoxBuffer{ NULL };

#if USE_BATCH_MATERIAL
	CMaterial			**m_ppMaterials{ NULL };
	int					m_nMaterials{ 0 };
#endif

	ID3D12DescriptorHeap			**m_ppCbvSrvDescriptorHeaps{ NULL };
	int m_nHeaps{ 0 };
	int m_boundingBoxHeapNumber{ 0 };

	D3D12_CPU_DESCRIPTOR_HANDLE		*m_pcbvCPUDescriptorStartHandle{ NULL };
	D3D12_GPU_DESCRIPTOR_HANDLE		*m_pcbvGPUDescriptorStartHandle{ NULL };
	D3D12_CPU_DESCRIPTOR_HANDLE		*m_psrvCPUDescriptorStartHandle{ NULL };
	D3D12_GPU_DESCRIPTOR_HANDLE		*m_psrvGPUDescriptorStartHandle{ NULL };

	ComPtr<ID3D12GraphicsCommandList> m_pCommandList;

	ComPtr<ID3D12RootSignature>		m_pGraphicsRootSignature;
};

