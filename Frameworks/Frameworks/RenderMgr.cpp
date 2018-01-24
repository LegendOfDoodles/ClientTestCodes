#include "stdafx.h"
#include "RenderMgr.h"
#include "Scene.h"

/// <summary>
/// 목적: 렌더링 관련 함수를 모아 두어 다른 변경사항 없이 그릴 수 있도록 하기 위함
/// 최종 수정자:  김나단
/// 수정자 목록:  김나단
/// 최종 수정 날짜: 2018-01-24
/// </summary>

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CRenderMgr::CRenderMgr()
{
}

CRenderMgr::~CRenderMgr()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
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
	m_pSwapChain->Present(0, 0);

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
	UINT64 fenceValue = ++m_fenceValues[m_swapChainBufferIndex];
	HRESULT hResult = m_pCommandQueue->Signal(m_pFence, fenceValue);
	//GPU가 펜스의 값을 설정하는 명령을 명령 큐에 추가한다.
	if (m_pFence->GetCompletedValue() < fenceValue)
	{
		//펜스의 현재 값이 설정한 값보다 작으면 펜스의 현재 값이 설정한 값이 될 때까지 기다린다.
		hResult = m_pFence->SetEventOnCompletion(fenceValue, m_hFenceEvent);
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
	m_pCommandList->Close();
	ID3D12CommandList *ppCommandLists[] = { m_pCommandList };
	m_pCommandQueue->ExecuteCommandLists(1, ppCommandLists);

	WaitForGpuComplete();
}

////////////////////////////////////////////////////////////////////////
// 내부 함수