#include "stdafx.h"
#include "RenderMgr.h"
#include "../../03.Scenes/00.BaseScene/Scene.h"

/// <summary>
/// ����: ������ ���� �Լ��� ��� �ξ� �ٸ� ������� ���� �׸� �� �ֵ��� �ϱ� ����
/// ���� ������:  �質��
/// ������ ���:  �質��
/// ���� ���� ��¥: 2018-01-27
/// </summary>

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
CRenderMgr::CRenderMgr()
{
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
	m_fenceValues[0] = 1;
}

void CRenderMgr::Release()
{
	::CloseHandle(m_hFenceEvent);
}

void CRenderMgr::Render(CScene* pScene)
{
	HRESULT hResult;
	// Reset Command List
	hResult = m_pCommandAllocator->Reset();
	assert(SUCCEEDED(hResult) && "CommandAllocator->Reset Failed");

	hResult = m_pCommandList->Reset(m_pCommandAllocator, NULL);
	assert(SUCCEEDED(hResult) && "CommandList->Reset Failed");

	// Set Barrier
	m_pCommandList->ResourceBarrier(	1, 
		&CreateResourceBarrier(m_ppRenderTargetBuffers[m_swapChainBufferIndex], 
			D3D12_RESOURCE_STATE_PRESENT,
			D3D12_RESOURCE_STATE_RENDER_TARGET));

	// Set Viewport and Scissor Rect
	pScene->SetViewportsAndScissorRects();

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

	// Set Graphics Root Signature
	m_pCommandList->SetGraphicsRootSignature(m_pGraphicsRootSignature);
	
	// Update Camera
	pScene->UpdateCamera();

	// Render Scene
	pScene->Render();

	// Set Barrier
	m_pCommandList->ResourceBarrier(1, 
		&CreateResourceBarrier(m_ppRenderTargetBuffers[m_swapChainBufferIndex],
			D3D12_RESOURCE_STATE_RENDER_TARGET,
			D3D12_RESOURCE_STATE_PRESENT));

	// Close Command List
	hResult = m_pCommandList->Close();
	assert(SUCCEEDED(hResult) && "CommandList->Close Failed");

	// Excute Command List
	ID3D12CommandList *ppCommandLists[] = { m_pCommandList };
	m_pCommandQueue->ExecuteCommandLists(1, ppCommandLists);

	WaitForGpuComplete();

	// Present
	hResult = m_pSwapChain->Present(0, 0);
	assert(SUCCEEDED(hResult) && "SwapChain->Present Failed");

	MoveToNextFrame();
}

void CRenderMgr::SetRenderTargetBuffers(ID3D12Resource *ppRenderTargetBuffers[])
{
	for (int i = 0; i < SWAP_CHAIN_BUFFER_CNT; ++i)
	{
		m_ppRenderTargetBuffers[i] = ppRenderTargetBuffers[i];
	}
}

void CRenderMgr::WaitForGpuComplete()
{
	HRESULT hResult;
	UINT64 fenceValue = ++m_fenceValues[m_swapChainBufferIndex];
	
	hResult = m_pCommandQueue->Signal(m_pFence, fenceValue);
	assert(SUCCEEDED(hResult) && "CommandQueue->Signal Failed");

	//GPU�� �潺�� ���� �����ϴ� ������ ���� ť�� �߰��Ѵ�.
	if (m_pFence->GetCompletedValue() < fenceValue)
	{
		//�潺�� ���� ���� ������ ������ ������ �潺�� ���� ���� ������ ���� �� ������ ��ٸ���.
		hResult = m_pFence->SetEventOnCompletion(fenceValue, m_hFenceEvent);
		assert(SUCCEEDED(hResult) && "SetEventOnCompletion Failed");

		::WaitForSingleObject(m_hFenceEvent, INFINITE);
	}
}

void CRenderMgr::MoveToNextFrame()
{
	// Get Next Back Buffer Index
	m_swapChainBufferIndex = m_pSwapChain->GetCurrentBackBufferIndex();
	WaitForGpuComplete();
}

void CRenderMgr::ResetCommandList()
{
	m_pCommandList->Reset(m_pCommandAllocator, NULL);
}

void CRenderMgr::ExecuteCommandList()
{
	HRESULT hResult;

	hResult = m_pCommandList->Close();
	assert(SUCCEEDED(hResult) && "CommandList->Close Failed");

	ID3D12CommandList *ppCommandLists[] = { m_pCommandList };
	m_pCommandQueue->ExecuteCommandLists(1, ppCommandLists);

	WaitForGpuComplete();
}

////////////////////////////////////////////////////////////////////////
// ���� �Լ�
//D3D12_RESOURCE_BARRIER CRenderMgr::CreateResourceBarrier()
//{
//	D3D12_RESOURCE_BARRIER resourceBarrier;
//
//	::ZeroMemory(&resourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
//	resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
//	resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
//	resourceBarrier.Transition.pResource =
//		m_ppRenderTargetBuffers[m_swapChainBufferIndex];
//	resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
//	resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
//	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
//
//	return(resourceBarrier);
//}
//
//void CRenderMgr::SetResourceBarrier(D3D12_RESOURCE_BARRIER &resourceBarrier,
//	D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after)
//{
//	resourceBarrier.Transition.StateBefore = before;
//	resourceBarrier.Transition.StateAfter = after;
//}