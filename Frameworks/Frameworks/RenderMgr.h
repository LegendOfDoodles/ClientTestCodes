#pragma once
class CRenderMgr
{
public:	// 생성자, 소멸자
	CRenderMgr();
	~CRenderMgr();

public:	// 공개 함수
	void Initialize(int width, int height);
	void Release();

	void Render();

	// Set Swap Chain
	void SetSwapChain(IDXGISwapChain3 *pSwapChain) { m_pSwapChain = pSwapChain; }
	void SetSwapChainBufferIndex(UINT swapChainBufferIndex) { m_swapChainBufferIndex = swapChainBufferIndex; }

	// Set Render Target View
	void SetRenderTargetBuffers(ID3D12Resource *ppRenderTargetBuffers[]);
	void SetRtvDescriptorHeap(ID3D12DescriptorHeap *pRtvDescriptorHeap) { m_pRtvDescriptorHeap = pRtvDescriptorHeap; }
	void SetRtvDescriptorIncrementSize(UINT rtvDescriptorIncrementSize) { m_rtvDescriptorIncrementSize = rtvDescriptorIncrementSize;  }

	// Set Depth Stencil View
	void SetDsvDescriptorHeap(ID3D12DescriptorHeap *pDsvDescriptorHeap) { m_pDsvDescriptorHeap = pDsvDescriptorHeap; }

	// Set Command Queue
	void SetCommandQueue(ID3D12CommandQueue *pCommandQueue) { m_pCommandQueue = pCommandQueue; }
	void SetCommandAllocator(ID3D12CommandAllocator *pCommandAllocator) { m_pCommandAllocator = pCommandAllocator; }
	void SetCommandList(ID3D12GraphicsCommandList *pCommandList) { m_pCommandList = pCommandList; }

	// Set Fence
	void SetFence(ID3D12Fence *pFence) { m_pFence = pFence; }

private:	// 내부 함수
	// Synchronize
	void WaitForGpuComplete();

private:	// 변수
	// Swap Chain
	IDXGISwapChain3 *m_pSwapChain;
	UINT m_swapChainBufferIndex;

	// Render Target View
	ID3D12Resource *m_ppRenderTargetBuffers[SWAP_CHAIN_BUFFER_CNT];
	ID3D12DescriptorHeap *m_pRtvDescriptorHeap;
	UINT m_rtvDescriptorIncrementSize;

	// Depth Stencil View
	ID3D12DescriptorHeap *m_pDsvDescriptorHeap;

	// Command Queue
	ID3D12CommandQueue *m_pCommandQueue;
	ID3D12CommandAllocator *m_pCommandAllocator;
	ID3D12GraphicsCommandList *m_pCommandList;

	// Fence
	ID3D12Fence *m_pFence;
	UINT64 m_fenceValue;
	HANDLE m_hFenceEvent;

	// Viewport and ScissorRect
	D3D12_VIEWPORT m_viewport;
	D3D12_RECT m_scissorRect;
};

