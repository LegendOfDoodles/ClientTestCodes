#pragma once
#include "RenderMgr.h"

class CCreateMgr
{
public:	// ������, �Ҹ���
	CCreateMgr();
	~CCreateMgr();

public:	// ���� �Լ�
	void Initialize(HINSTANCE hInstance, HWND hwnd);
	void Release();

	void Resize(int width, int height);

	void OnResizeBackBuffers();
	void ChangeScreenMode();

	CRenderMgr* GetRenderMgr() { return &m_renderMgr; }

private:	// ���� �Լ�
	void CreateDirect3dDevice();
	void CreateCommandQueueAndList();
	void CreateSwapChain();
	void CreateRtvAndDsvDescriptorHeaps();
	void CreateDepthStencilView();
	void CreateRenderTargetView();

public:	// ����
	HINSTANCE m_hInstance;
	HWND m_hwnd;

	// Window Size
	int m_wndClientWidth;
	int m_wndClientHeight;

	// Factory and Device
	IDXGIFactory4 *m_pFactory;
	ID3D12Device *m_pDevice;

	// MSAA Set
	bool m_msaa4xEnable = false;
	UINT m_msaa4xQualityLevels = 0;

	// Swap Chain
	IDXGISwapChain3 *m_pSwapChain;

	// Render Target View
	ID3D12Resource *m_ppRenderTargetBuffers[SWAP_CHAIN_BUFFER_CNT];
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

#ifdef _DEBUG
	ID3D12Debug *m_pDebugController;
#endif

	// Render Manager
	CRenderMgr m_renderMgr;
};

