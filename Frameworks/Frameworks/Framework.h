#pragma once
#include "CreateMgr.h"

class CFramework
{
public:	// 생성자, 소멸자
	CFramework();
	~CFramework();

public: // 공개 함수
	// Initialize and Release
	bool OnCreate(HINSTANCE hInstance, HWND hwnd);
	void OnDestroy();

	void FrameAdvance();

	// Message Process
	void OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, 
		WPARAM wParam, LPARAM lParam);
	void OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, 
		WPARAM wParam, LPARAM lParam);
	LRESULT CALLBACK OnProcessingWindowMessage(HWND hWnd, UINT nMessageID, 
		WPARAM wParam, LPARAM lParam);

private: // 내부 함수
	 // Create Functions
	void CreateDirect3dDevice();
	void CreateCommandQueueAndList();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDepthStencilView();
	void CreateRenderTargetView();

	void BuildObjects();
	void ReleaseObjects();

	void ProcessInput();
	void AnimateObjects();

	// Synchronize
	void WaitForGpuComplete();

private: // 변수
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

