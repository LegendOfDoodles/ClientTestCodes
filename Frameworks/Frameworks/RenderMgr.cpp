#include "stdafx.h"
#include "RenderMgr.h"

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CRenderMgr::CRenderMgr()
{
	// Swap Chain
	m_pSwapChain = NULL;
	m_swapChainBufferIndex = 0;

	// Render Target View
	for (int i = 0; i < SWAP_CHAIN_BUFFER_CNT; i++)
	{
		m_ppRenderTargetBuffers[i] = NULL;
	}
	m_pRtvDescriptorHeap = NULL;
	m_rtvDescriptorIncrementSize = 0;

	// Depth Stencil View
	m_pDsvDescriptorHeap = NULL;

	// Command Queue
	m_pCommandAllocator = NULL;
	m_pCommandQueue = NULL;
	m_pCommandList = NULL;

	// Fence
	m_hFenceEvent = NULL;
	m_pFence = NULL;
	m_fenceValue = 0;
}

CRenderMgr::~CRenderMgr()
{
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CRenderMgr::Initialize(int width, int height)
{
	// Fence
	m_hFenceEvent = ::CreateEvent(NULL, FALSE, FALSE, NULL);
	m_fenceValue = 1;

	// Viewport
	m_viewport.TopLeftX = 0;
	m_viewport.TopLeftY = 0;
	m_viewport.Width = static_cast<float>(width);
	m_viewport.Height = static_cast<float>(height);
	m_viewport.MinDepth = 0.0f;
	m_viewport.MaxDepth = 1.0f;

	// Scissor Rect
	m_scissorRect = { 0, 0, width, height };
}

void CRenderMgr::Release()
{
	::CloseHandle(m_hFenceEvent);
}

void CRenderMgr::Render()
{
	// Reset Command List
	HRESULT hResult = m_pCommandAllocator->Reset();
	hResult = m_pCommandList->Reset(m_pCommandAllocator, NULL);

	// Set Barrier
	D3D12_RESOURCE_BARRIER resourceBarrier;
	::ZeroMemory(&resourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resourceBarrier.Transition.pResource =
		m_ppRenderTargetBuffers[m_swapChainBufferIndex];
	resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	m_pCommandList->ResourceBarrier(1, &resourceBarrier);

	// Set Viewport and Scissor Rect
	m_pCommandList->RSSetViewports(1, &m_viewport);
	m_pCommandList->RSSetScissorRects(1, &m_scissorRect);

	// Check Render Target
	D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUDescriptorHandle =
		m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	rtvCPUDescriptorHandle.ptr +=
		(m_swapChainBufferIndex * m_rtvDescriptorIncrementSize);

	// Clear Render Target View
	float pClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	m_pCommandList->ClearRenderTargetView(rtvCPUDescriptorHandle,
		pClearColor, 0, NULL);

	// Check Depth Stencil
	D3D12_CPU_DESCRIPTOR_HANDLE dsvCPUDescriptorHandle =
		m_pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// Clear Depth Stencil View
	m_pCommandList->ClearDepthStencilView(dsvCPUDescriptorHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0,
		0, NULL);

	// Set Render Target and Depth Stencil
	m_pCommandList->OMSetRenderTargets(1, &rtvCPUDescriptorHandle,
		TRUE, &dsvCPUDescriptorHandle);

	// Set Barrier
	resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	m_pCommandList->ResourceBarrier(1, &resourceBarrier);

	// Close Command List
	hResult = m_pCommandList->Close();

	// Excute Command List
	ID3D12CommandList *ppCommandLists[] = { m_pCommandList };
	m_pCommandQueue->ExecuteCommandLists(1, ppCommandLists);

	WaitForGpuComplete();

	// Present
	DXGI_PRESENT_PARAMETERS presentParameters;
	presentParameters.DirtyRectsCount = 0;
	presentParameters.pDirtyRects = NULL;
	presentParameters.pScrollRect = NULL;
	presentParameters.pScrollOffset = NULL;

	m_pSwapChain->Present1(1, 0, &presentParameters);

	// Get Next Back Buffer Index
	m_swapChainBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
}

void CRenderMgr::SetRenderTargetBuffers(ID3D12Resource *ppRenderTargetBuffers[])
{
	for (int i = 0; i < SWAP_CHAIN_BUFFER_CNT; ++i)
	{
		m_ppRenderTargetBuffers[i] = ppRenderTargetBuffers[i];
	}
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
void CRenderMgr::WaitForGpuComplete()
{
	m_fenceValue++;
	//CPU �潺�� ���� �����Ѵ�.
	const UINT64 nFence = m_fenceValue;
	HRESULT hResult = m_pCommandQueue->Signal(m_pFence, nFence);
	//GPU�� �潺�� ���� �����ϴ� ����� ��� ť�� �߰��Ѵ�.
	if (m_pFence->GetCompletedValue() < nFence)
	{
		//�潺�� ���� ���� ������ ������ ������ �潺�� ���� ���� ������ ���� �� ������ ��ٸ���.
		hResult = m_pFence->SetEventOnCompletion(nFence, m_hFenceEvent);
		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}