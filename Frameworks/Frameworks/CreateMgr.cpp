#include "stdafx.h"
#include "CreateMgr.h"

/// <summary>
/// 목적: 생성 관련 함수를 모아 두어 헷갈리는 일 없이 생성 가능하도록 함
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-01-12
/// </summary>


////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CCreateMgr::CCreateMgr()
{
}

CCreateMgr::~CCreateMgr()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CCreateMgr::Initialize(HINSTANCE hInstance, HWND hWnd)
{
	m_hInstance = hInstance;
	m_hWnd = hWnd;

	CreateDirect3dDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	CreateGraphicsRootSignature();

	m_renderMgr.Initialize(m_wndClientWidth, m_wndClientHeight);
}

void CCreateMgr::Release()
{
#ifdef _DEBUG
	Safe_Release(m_pDebugController);
#endif

	for (int i = 0; i < SWAP_CHAIN_BUFFER_CNT; i++)
	{
		Safe_Release(m_ppRenderTargetBuffers[i]);
	}
	Safe_Release(m_pRtvDescriptorHeap);

	Safe_Release(m_pDepthStencilBuffer);
	Safe_Release(m_pDsvDescriptorHeap);

	Safe_Release(m_pCommandAllocator);
	Safe_Release(m_pCommandQueue);
	Safe_Release(m_pCommandList);

	Safe_Release(m_pFence);

	m_pSwapChain->SetFullscreenState(FALSE, NULL);
	Safe_Release(m_pSwapChain);
	Safe_Release(m_pDevice);
	Safe_Release(m_pFactory);

	Safe_Release(m_pGraphicsRootSignature);

	m_renderMgr.Release();
}

void CCreateMgr::Resize(int width, int height)
{
	m_wndClientWidth = width;
	m_wndClientHeight = height;
}

void CCreateMgr::OnResizeBackBuffers()
{
	m_pCommandList->Reset(m_pCommandAllocator, NULL);

	for (int i = 0; i < SWAP_CHAIN_BUFFER_CNT; i++)
	{
		if (m_ppRenderTargetBuffers[i])
		{
			m_ppRenderTargetBuffers[i]->Release();
		}
	}
	if (m_pDepthStencilBuffer) { m_pDepthStencilBuffer->Release(); }

#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	DXGI_SWAP_CHAIN_DESC dxgiSwapChainDesc;
	m_pdxgiSwapChain->GetDesc(&dxgiSwapChainDesc);
	m_pdxgiSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	m_nSwapChainBufferIndex = 0;
#else
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	m_pSwapChain->GetDesc(&swapChainDesc);
	m_pSwapChain->ResizeBuffers(SWAP_CHAIN_BUFFER_CNT, m_wndClientWidth,
		m_wndClientHeight, swapChainDesc.BufferDesc.Format, swapChainDesc.Flags);
	m_renderMgr.SetSwapChainBufferIndex(m_pSwapChain->GetCurrentBackBufferIndex());
#endif

	CreateRenderTargetView();
	CreateDepthStencilView();

	m_pCommandList->Close();

	ID3D12CommandList *ppCommandLists[] = { m_pCommandList };
	m_pCommandQueue->ExecuteCommandLists(1, ppCommandLists);
}

void CCreateMgr::ChangeScreenMode()
{
	BOOL fullScreenState = FALSE;
	m_pSwapChain->GetFullscreenState(&fullScreenState, NULL);
	if (!fullScreenState)
	{
		DXGI_MODE_DESC targetParameters;
		targetParameters.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		targetParameters.Width = m_wndClientWidth;
		targetParameters.Height = m_wndClientHeight;
		targetParameters.RefreshRate.Numerator = 60;
		targetParameters.RefreshRate.Denominator = 1;
		targetParameters.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		targetParameters.ScanlineOrdering =
			DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
		m_pSwapChain->ResizeTarget(&targetParameters);
	}
	m_pSwapChain->SetFullscreenState(!fullScreenState, NULL);
}

ID3D12Resource* CCreateMgr::CreateBufferResource(
	void *pData, UINT nBytes, D3D12_HEAP_TYPE heapType,
	D3D12_RESOURCE_STATES resourceStates, ID3D12Resource **ppUploadBuffer)
{
	ID3D12Resource *pBuffer = NULL;

	D3D12_HEAP_PROPERTIES heapPropertiesDesc;
	::ZeroMemory(&heapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
	heapPropertiesDesc.Type = heapType;
	heapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	heapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	heapPropertiesDesc.CreationNodeMask = 1;
	heapPropertiesDesc.VisibleNodeMask = 1;

	D3D12_RESOURCE_DESC resourceDesc;
	::ZeroMemory(&resourceDesc, sizeof(D3D12_RESOURCE_DESC));
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Alignment = 0;
	resourceDesc.Width = nBytes;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.SampleDesc.Quality = 0;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

	D3D12_RESOURCE_STATES resourceInitialStates = D3D12_RESOURCE_STATE_COPY_DEST;
	if (heapType == D3D12_HEAP_TYPE_UPLOAD)
		resourceInitialStates =	D3D12_RESOURCE_STATE_GENERIC_READ;
	else if (heapType == D3D12_HEAP_TYPE_READBACK)
		resourceInitialStates =	D3D12_RESOURCE_STATE_COPY_DEST;

	HRESULT hResult = m_pDevice->CreateCommittedResource(
		&heapPropertiesDesc,
		D3D12_HEAP_FLAG_NONE, 
		&resourceDesc,
		resourceInitialStates, 
		NULL,
		__uuidof(ID3D12Resource), 
		(void **)&pBuffer);

	if (pData)
	{
		switch (heapType)
		{
		case D3D12_HEAP_TYPE_DEFAULT:
		{
			if (ppUploadBuffer)
			{
				//업로드 버퍼를 생성한다.
				heapPropertiesDesc.Type = D3D12_HEAP_TYPE_UPLOAD;
				m_pDevice->CreateCommittedResource(&heapPropertiesDesc,
					D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, NULL,
					__uuidof(ID3D12Resource), (void **)ppUploadBuffer);

				//업로드 버퍼를 매핑하여 초기화 데이터를 업로드 버퍼에 복사한다.
				D3D12_RANGE readRange = { 0, 0 };
				UINT8 *pBufferDataBegin = NULL;
				(*ppUploadBuffer)->Map(0, &readRange, (void **)&pBufferDataBegin);
				memcpy(pBufferDataBegin, pData, nBytes);
				(*ppUploadBuffer)->Unmap(0, NULL);

				//업로드 버퍼의 내용을 디폴트 버퍼에 복사한다.
				m_pCommandList->CopyResource(pBuffer, *ppUploadBuffer);

				D3D12_RESOURCE_BARRIER resourceBarrier;
				::ZeroMemory(&resourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
				resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
				resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
				resourceBarrier.Transition.pResource = pBuffer;
				resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
				resourceBarrier.Transition.StateAfter = resourceStates;
				resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
				m_pCommandList->ResourceBarrier(1, &resourceBarrier);
			}
			break;
		}
		case D3D12_HEAP_TYPE_UPLOAD:
		{
			D3D12_RANGE readRange = { 0, 0 };
			UINT8 *pBufferDataBegin = NULL;
			pBuffer->Map(0, &readRange, (void **)&pBufferDataBegin);
			memcpy(pBufferDataBegin, pData, nBytes);
			pBuffer->Unmap(0, NULL);
			break;
		}
		case D3D12_HEAP_TYPE_READBACK:
			break;
		}
	}
	return(pBuffer);
}


////////////////////////////////////////////////////////////////////////
// 내부 함수
void CCreateMgr::CreateDirect3dDevice()
{
#if defined(_DEBUG)
	D3D12GetDebugInterface(__uuidof(ID3D12Debug), (void **)&m_pDebugController);
	m_pDebugController->EnableDebugLayer();
#endif

	::CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void **)&m_pFactory);

	//모든 하드웨어 어댑터 대하여 특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성한다.
	IDXGIAdapter1 *pAdapter = NULL;
	for (UINT i = 0;
		DXGI_ERROR_NOT_FOUND != m_pFactory->EnumAdapters1(i, &pAdapter);
		i++)
	{
		DXGI_ADAPTER_DESC1 adapterDesc;
		pAdapter->GetDesc1(&adapterDesc);
		if (adapterDesc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE) continue;
		if (SUCCEEDED(D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_12_0,
			_uuidof(ID3D12Device), (void **)&m_pDevice))) break;
	}

	//특성 레벨 12.0을 지원하는 하드웨어 디바이스를 생성할 수 없으면 WARP 디바이스를 생성한다.
	if (!pAdapter)
	{
		m_pFactory->EnumWarpAdapter(_uuidof(IDXGIFactory4), (void **)&pAdapter);
		D3D12CreateDevice(pAdapter, D3D_FEATURE_LEVEL_11_0,
			_uuidof(ID3D12Device), (void**)&m_pDevice);
	}

	// MSAA Check
	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS d3dMsaaQualityLevels;
	d3dMsaaQualityLevels.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	d3dMsaaQualityLevels.SampleCount = 4; //Msaa4x 다중 샘플링
	d3dMsaaQualityLevels.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE;
	d3dMsaaQualityLevels.NumQualityLevels = 0;
	m_pDevice->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS,
		&d3dMsaaQualityLevels, sizeof(D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS));
	m_msaa4xQualityLevels = d3dMsaaQualityLevels.NumQualityLevels;

	// MSAA 
	m_msaa4xEnable = (m_msaa4xQualityLevels > 1) ? true : false;

	// Fence
	m_pDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE,
		__uuidof(ID3D12Fence), (void**)&m_pFence);
	m_renderMgr.SetFence(m_pFence);

	if (pAdapter) pAdapter->Release();
}

void CCreateMgr::CreateCommandQueueAndList()
{
	// Create Queue
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
	::ZeroMemory(&commandQueueDesc, sizeof(D3D12_COMMAND_QUEUE_DESC));
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

	HRESULT hResult = m_pDevice->CreateCommandQueue(&commandQueueDesc,
		_uuidof(ID3D12CommandQueue), (void **)&m_pCommandQueue);
	m_renderMgr.SetCommandQueue(m_pCommandQueue);

	// Create Allocator
	hResult = m_pDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
		__uuidof(ID3D12CommandAllocator), (void **)&m_pCommandAllocator);
	m_renderMgr.SetCommandAllocator(m_pCommandAllocator);

	// Create Command List
	hResult = m_pDevice->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_DIRECT,
		m_pCommandAllocator, NULL,
		__uuidof(ID3D12GraphicsCommandList), (void**)&m_pCommandList);
	m_renderMgr.SetCommandList(m_pCommandList);

	hResult = m_pCommandList->Close();
}

void CCreateMgr::CreateSwapChain()
{
	RECT rcClient;
	::GetClientRect(m_hWnd, &rcClient);
	m_wndClientWidth = rcClient.right - rcClient.left;
	m_wndClientHeight = rcClient.bottom - rcClient.top;

	// Create Swap Chain
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	::ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));
	swapChainDesc.BufferDesc.Width = m_wndClientWidth;
	swapChainDesc.BufferDesc.Height = m_wndClientHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = SWAP_CHAIN_BUFFER_CNT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.OutputWindow = m_hWnd;
	swapChainDesc.SampleDesc.Count = (m_msaa4xEnable) ? 4 : 1;
	swapChainDesc.SampleDesc.Quality = (m_msaa4xEnable) ? (m_msaa4xQualityLevels - 1) : 0;
	swapChainDesc.Windowed = TRUE;

#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	swapChainDesc.Flags = 0;
#else
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#endif

	HRESULT hResult = m_pFactory->CreateSwapChain(m_pCommandQueue,
		&swapChainDesc, (IDXGISwapChain **)&m_pSwapChain);
	m_renderMgr.SetSwapChain(m_pSwapChain);

	//“Alt+Enter” 키의 동작을 비활성화한다.
	m_pFactory->MakeWindowAssociation(m_hWnd, DXGI_MWA_NO_ALT_ENTER);

	//스왑체인의 현재 후면버퍼 인덱스를 저장한다.
	m_renderMgr.SetSwapChainBufferIndex(m_pSwapChain->GetCurrentBackBufferIndex());
}

void CCreateMgr::CreateRtvAndDsvDescriptorHeaps()
{
	// Create Render Target View Descriptor Heap
	D3D12_DESCRIPTOR_HEAP_DESC d3dDescriptorHeapDesc;
	::ZeroMemory(&d3dDescriptorHeapDesc, sizeof(D3D12_DESCRIPTOR_HEAP_DESC));
	d3dDescriptorHeapDesc.NumDescriptors = SWAP_CHAIN_BUFFER_CNT;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	d3dDescriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	d3dDescriptorHeapDesc.NodeMask = 0;

	HRESULT hResult = m_pDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		__uuidof(ID3D12DescriptorHeap), (void **)&m_pRtvDescriptorHeap);
	m_renderMgr.SetRtvDescriptorHeap(m_pRtvDescriptorHeap);

	m_rtvDescriptorIncrementSize =
		m_pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	m_renderMgr.SetRtvDescriptorIncrementSize(m_rtvDescriptorIncrementSize);

	// Create Depth Stencil View Descriptor Heap
	d3dDescriptorHeapDesc.NumDescriptors = 1;
	d3dDescriptorHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	hResult = m_pDevice->CreateDescriptorHeap(&d3dDescriptorHeapDesc,
		__uuidof(ID3D12DescriptorHeap), (void **)&m_pDsvDescriptorHeap);
	m_renderMgr.SetDsvDescriptorHeap(m_pDsvDescriptorHeap);
}

void CCreateMgr::CreateDepthStencilView()
{
	// Create Depth Stencil Buffer
	D3D12_RESOURCE_DESC d3dResourceDesc;
	d3dResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	d3dResourceDesc.Alignment = 0;
	d3dResourceDesc.Width = m_wndClientWidth;
	d3dResourceDesc.Height = m_wndClientHeight;
	d3dResourceDesc.DepthOrArraySize = 1;
	d3dResourceDesc.MipLevels = 1;
	d3dResourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dResourceDesc.SampleDesc.Count = (m_msaa4xEnable) ? 4 : 1;
	d3dResourceDesc.SampleDesc.Quality =
		(m_msaa4xEnable) ? (m_msaa4xQualityLevels - 1) : 0;
	d3dResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	d3dResourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_HEAP_PROPERTIES d3dHeapProperties;
	::ZeroMemory(&d3dHeapProperties, sizeof(D3D12_HEAP_PROPERTIES));
	d3dHeapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;
	d3dHeapProperties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	d3dHeapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	d3dHeapProperties.CreationNodeMask = 1;
	d3dHeapProperties.VisibleNodeMask = 1;

	D3D12_CLEAR_VALUE d3dClearValue;
	d3dClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	d3dClearValue.DepthStencil.Depth = 1.0f;
	d3dClearValue.DepthStencil.Stencil = 0;

	m_pDevice->CreateCommittedResource(&d3dHeapProperties, D3D12_HEAP_FLAG_NONE,
		&d3dResourceDesc, D3D12_RESOURCE_STATE_DEPTH_WRITE, &d3dClearValue,
		__uuidof(ID3D12Resource), (void **)&m_pDepthStencilBuffer);

	// Create Depth Stencil View
	D3D12_CPU_DESCRIPTOR_HANDLE d3dDsvCPUDescriptorHandle =
		m_pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	m_pDevice->CreateDepthStencilView(m_pDepthStencilBuffer, NULL,
		d3dDsvCPUDescriptorHandle);
}

void CCreateMgr::CreateRenderTargetView()
{
	HRESULT hResult;

	D3D12_CPU_DESCRIPTOR_HANDLE d3dRtvCPUDescriptorHandle =
		m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	for (UINT i = 0; i <SWAP_CHAIN_BUFFER_CNT; i++)
	{
		hResult = m_pSwapChain->GetBuffer(i, __uuidof(ID3D12Resource),
			(void**)&m_ppRenderTargetBuffers[i]);
		m_pDevice->CreateRenderTargetView(m_ppRenderTargetBuffers[i], NULL,
			d3dRtvCPUDescriptorHandle);
		d3dRtvCPUDescriptorHandle.ptr += m_rtvDescriptorIncrementSize;
	}
	m_renderMgr.SetRenderTargetBuffers(m_ppRenderTargetBuffers);
}

void CCreateMgr::CreateGraphicsRootSignature()
{
	D3D12_ROOT_PARAMETER pRootParameters[3];
	pRootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pRootParameters[0].Descriptor.ShaderRegister = 0; //Player
	pRootParameters[0].Descriptor.RegisterSpace = 0;
	pRootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

	pRootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pRootParameters[1].Descriptor.ShaderRegister = 1; //Camera
	pRootParameters[1].Descriptor.RegisterSpace = 0;
	pRootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

#if USE_INSTANCING
	pRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_SRV;
	pRootParameters[2].Descriptor.ShaderRegister = 0; // t0 : Objects
	pRootParameters[2].Descriptor.RegisterSpace = 0;
	pRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
#else
	pRootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
	pRootParameters[2].Descriptor.ShaderRegister = 2; // Objects
	pRootParameters[2].Descriptor.RegisterSpace = 0;
	pRootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;
#endif

	D3D12_ROOT_SIGNATURE_FLAGS rootSignatureFlags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;

	D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	::ZeroMemory(&rootSignatureDesc, sizeof(D3D12_ROOT_SIGNATURE_DESC));

	rootSignatureDesc.NumParameters = _countof(pRootParameters);
	rootSignatureDesc.pParameters = pRootParameters;
	rootSignatureDesc.NumStaticSamplers = 0;
	rootSignatureDesc.pStaticSamplers = NULL;
	rootSignatureDesc.Flags = rootSignatureFlags;

	ID3DBlob *pSignatureBlob = NULL;		ID3DBlob *pErrorBlob = NULL;
	::D3D12SerializeRootSignature(
		&rootSignatureDesc,
		D3D_ROOT_SIGNATURE_VERSION_1,
		&pSignatureBlob,
		&pErrorBlob);

	m_pDevice->CreateRootSignature(
		0,
		pSignatureBlob->GetBufferPointer(),
		pSignatureBlob->GetBufferSize(),
		__uuidof(ID3D12RootSignature), 
		(void**)&m_pGraphicsRootSignature);

	if (pSignatureBlob) pSignatureBlob->Release();
	if (pErrorBlob) pErrorBlob->Release();

	m_renderMgr.SetGraphicsRootSignature(m_pGraphicsRootSignature);
}