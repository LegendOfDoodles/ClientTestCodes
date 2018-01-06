#include "stdafx.h"
#include "CreateMgr.h"

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CCreateMgr::CCreateMgr()
{
	// Window Size
	m_wndClientWidth = FRAME_BUFFER_WIDTH;
	m_wndClientHeight = FRAME_BUFFER_HEIGHT;

	// Factory and Device
	m_pFactory = NULL;
	m_pDevice = NULL;

	// Swap Chain
	m_pSwapChain = NULL;

	// Render Target View
	for (int i = 0; i < SWAP_CHAIN_BUFFER_CNT; i++)
	{
		m_ppRenderTargetBuffers[i] = NULL;
	}
	m_pRtvDescriptorHeap = NULL;
	m_rtvDescriptorIncrementSize = 0;

	// Depth Stencil View
	m_pDepthStencilBuffer = NULL;
	m_pDsvDescriptorHeap = NULL;

	// Command Queue
	m_pCommandAllocator = NULL;
	m_pCommandQueue = NULL;
	m_pCommandList = NULL;

	// Fence
	m_pFence = NULL;
}

CCreateMgr::~CCreateMgr()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
void CCreateMgr::Initialize(HINSTANCE hInstance, HWND hwnd)
{
	m_hInstance = hInstance;
	m_hwnd = hwnd;

	CreateDirect3dDevice();
	CreateCommandQueueAndList();
	CreateSwapChain();
	CreateRtvAndDsvDescriptorHeaps();
	//CreateRenderTargetView();
	//CreateDepthStencilView();

	m_renderMgr.Initialize(m_wndClientWidth, m_wndClientHeight);
}

void CCreateMgr::Release()
{
#ifdef _DEBUG
	if (m_pDebugController) { m_pDebugController->Release(); }
#endif

	for (int i = 0; i < SWAP_CHAIN_BUFFER_CNT; i++)
	{
		if (m_ppRenderTargetBuffers[i])
		{
			m_ppRenderTargetBuffers[i]->Release();
		}
	}
	if (m_pRtvDescriptorHeap) { m_pRtvDescriptorHeap->Release(); }

	if (m_pDepthStencilBuffer) { m_pDepthStencilBuffer->Release(); }
	if (m_pDsvDescriptorHeap) { m_pDsvDescriptorHeap->Release(); }

	if (m_pCommandAllocator) { m_pCommandAllocator->Release(); }
	if (m_pCommandQueue) { m_pCommandQueue->Release(); }
	if (m_pCommandList) { m_pCommandList->Release(); }

	if (m_pFence) { m_pFence->Release(); }

	m_pSwapChain->SetFullscreenState(FALSE, NULL);
	if (m_pSwapChain) { m_pSwapChain->Release(); }
	if (m_pDevice) { m_pDevice->Release(); }
	if (m_pFactory) { m_pFactory->Release(); }

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
	::GetClientRect(m_hwnd, &rcClient);
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
	swapChainDesc.OutputWindow = m_hwnd;
	swapChainDesc.SampleDesc.Count = (m_msaa4xEnable) ? 4 : 1;
	swapChainDesc.SampleDesc.Quality = (m_msaa4xEnable) ? (m_msaa4xQualityLevels - 1) : 0;
	swapChainDesc.Windowed = TRUE;

#ifdef _WITH_ONLY_RESIZE_BACKBUFFERS
	swapChainDesc.Flags = 0;
#else
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
#endif

	//DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFullScreenDesc;
	//::ZeroMemory(&swapChainFullScreenDesc, sizeof(DXGI_SWAP_CHAIN_FULLSCREEN_DESC));
	//swapChainFullScreenDesc.RefreshRate.Numerator = 60;
	//swapChainFullScreenDesc.RefreshRate.Denominator = 1;
	//swapChainFullScreenDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	//swapChainFullScreenDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	//swapChainFullScreenDesc.Windowed = TRUE;

	HRESULT hResult = m_pFactory->CreateSwapChain(m_pCommandQueue,
		&swapChainDesc, (IDXGISwapChain **)&m_pSwapChain);
	m_renderMgr.SetSwapChain(m_pSwapChain);

	//“Alt+Enter” 키의 동작을 비활성화한다.
	m_pFactory->MakeWindowAssociation(m_hwnd, DXGI_MWA_NO_ALT_ENTER);

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