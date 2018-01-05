#pragma once
class CCreateMgr
{
public:	// 생성자, 소멸자
	CCreateMgr();
	~CCreateMgr();

public:	// 공개 함수
	void Initialize(HINSTANCE hInstance, HWND hwnd);
	void Release();

	void Resize(int width, int height);

private:	// 내부 함수
	void CreateDirect3dDevice();
	void CreateCommandQueueAndList();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDepthStencilView();
	void CreateRenderTargetView();

public:	// 변수
	HINSTANCE m_hInstance;
	HWND m_hwnd;

	// Window Size
	int m_wndClientWidth;
	int m_wndClientHeight;

	// Factory and Device
	IDXGIFactory4 *m_pFactory;
	IDXGISwapChain3 *m_pSwapChain;
	ID3D12Device *m_pDevice;

	// MSAA Set
	bool m_msaa4xEnable = false;
	UINT m_msaa4xQualityLevels = 0;

	// Swap Chain
	static const UINT m_swapChainBuffers = 2;
	UINT m_swapChainBufferIndex;

	// Render Target View
	ID3D12Resource *m_ppRenderTargetBuffers[m_swapChainBuffers];
	ID3D12DescriptorHeap *m_pRtvDescriptorHeap;
	UINT m_rtvDescriptorIncrementSize;

	// Depth Stencil View
	ID3D12Resource *m_pDepthStencilBuffer;
	ID3D12DescriptorHeap *m_pDsvDescriptorHeap;

	// Command Queue
	ID3D12CommandQueue *m_pCommandQueue;
	ID3D12CommandAllocator *m_pCommandAllocator;
	ID3D12GraphicsCommandList *m_pCommandList;

	// Fence
	ID3D12Fence *m_pFence;
	UINT64 m_fenceValue;
	HANDLE m_hFenceEvent;

#ifdef _DEBUG
	ID3D12Debug *m_pDebugController;
#endif

	// Viewport and ScissorRect
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;
};

