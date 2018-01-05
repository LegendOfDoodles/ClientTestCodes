#include "stdafx.h"
#include "Framework.h"

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CFramework::CFramework()
{
}

CFramework::~CFramework()
{
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
bool CFramework::OnCreate(HINSTANCE hInstance, HWND hwnd)
{
	m_createMgr.Initialize(hInstance, hwnd);
	BuildObjects();

	return(true);
}

void CFramework::OnDestroy()
{
	ReleaseObjects();
	m_createMgr.Release();
}

void CFramework::FrameAdvance()
{
	ProcessInput();
	AnimateObjects();
	RenderObjects();
}

void CFramework::OnProcessingMouseMessage(HWND hWnd, UINT nMessageID, 
	WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
		break;
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
		break;
	case WM_MOUSEMOVE:
		break;
	default:
		break;
	}
}

void CFramework::OnProcessingKeyboardMessage(HWND hWnd, UINT nMessageID, 
	WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_KEYUP:
		switch (wParam)
		{
		case VK_ESCAPE:
			::PostQuitMessage(0);
			break;
		case VK_RETURN:
			break;
		case VK_F8:
			break;
		case VK_F9:
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}
}

LRESULT CALLBACK CFramework::OnProcessingWindowMessage(HWND hWnd, UINT nMessageID,
	WPARAM wParam, LPARAM lParam)
{
	switch (nMessageID)
	{
	case WM_SIZE:
	{
		m_createMgr.Resize(LOWORD(lParam), HIWORD(lParam));
		break;
	}
	case WM_LBUTTONDOWN:
	case WM_RBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		OnProcessingMouseMessage(hWnd, nMessageID, wParam, lParam);
		break;
	case WM_KEYDOWN:
	case WM_KEYUP:
		OnProcessingKeyboardMessage(hWnd, nMessageID, wParam, lParam);
		break;
	}
	return(0);
}

////////////////////////////////////////////////////////////////////////
// 내부 함수
void CFramework::BuildObjects()
{
}

void CFramework::ReleaseObjects()
{
}

void CFramework::ProcessInput()
{
}

void CFramework::AnimateObjects()
{
}

void CFramework::RenderObjects()
{
	// Reset Command List
	HRESULT hResult = m_createMgr.m_pCommandAllocator->Reset();
	hResult = m_createMgr.m_pCommandList->Reset(m_createMgr.m_pCommandAllocator, NULL);
	
	// Set Barrier
	D3D12_RESOURCE_BARRIER resourceBarrier;
	::ZeroMemory(&resourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	resourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	resourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	resourceBarrier.Transition.pResource =
		m_createMgr.m_ppRenderTargetBuffers[m_createMgr.m_swapChainBufferIndex];
	resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	m_createMgr.m_pCommandList->ResourceBarrier(1, &resourceBarrier);

	// Set Viewport and Scissor Rect
	m_createMgr.m_pCommandList->RSSetViewports(1, &m_createMgr.m_viewport);
	m_createMgr.m_pCommandList->RSSetScissorRects(1, &m_createMgr.m_scissorRect);

	// Check Render Target
	D3D12_CPU_DESCRIPTOR_HANDLE rtvCPUDescriptorHandle =
		m_createMgr.m_pRtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();
	rtvCPUDescriptorHandle.ptr +=
		(m_createMgr.m_swapChainBufferIndex * m_createMgr.m_rtvDescriptorIncrementSize);

	// Clear Render Target View
	float pClearColor[4] = { 0.0f, 0.125f, 0.3f, 1.0f };
	m_createMgr.m_pCommandList->ClearRenderTargetView(rtvCPUDescriptorHandle,
		pClearColor, 0, NULL);

	// Check Depth Stencil
	D3D12_CPU_DESCRIPTOR_HANDLE dsvCPUDescriptorHandle =
		m_createMgr.m_pDsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart();

	// Clear Depth Stencil View
	m_createMgr.m_pCommandList->ClearDepthStencilView(dsvCPUDescriptorHandle,
		D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL,
		1.0f, 0,
		0, NULL);

	// Set Render Target and Depth Stencil
	m_createMgr.m_pCommandList->OMSetRenderTargets(1, &rtvCPUDescriptorHandle,
		TRUE, &dsvCPUDescriptorHandle);

	// Set Barrier
	resourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	resourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	resourceBarrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	m_createMgr.m_pCommandList->ResourceBarrier(1, &resourceBarrier);

	// Close Command List
	hResult = m_createMgr.m_pCommandList->Close();

	// Excute Command List
	ID3D12CommandList *ppCommandLists[] = { m_createMgr.m_pCommandList };
	m_createMgr.m_pCommandQueue->ExecuteCommandLists(1, ppCommandLists);

	WaitForGpuComplete();

	// Present
	DXGI_PRESENT_PARAMETERS presentParameters;
	presentParameters.DirtyRectsCount = 0;
	presentParameters.pDirtyRects = NULL;
	presentParameters.pScrollRect = NULL;
	presentParameters.pScrollOffset = NULL;

	m_createMgr.m_pSwapChain->Present1(1, 0, &presentParameters);

	// Get Next Back Buffer Index
	m_createMgr.m_swapChainBufferIndex = m_createMgr.m_pSwapChain->GetCurrentBackBufferIndex();
}

void CFramework::WaitForGpuComplete()
{
	m_createMgr.m_fenceValue++;
	//CPU 펜스의 값을 증가한다.
	const UINT64 nFence = m_createMgr.m_fenceValue;
	HRESULT hResult = m_createMgr.m_pCommandQueue->Signal(m_createMgr.m_pFence, nFence);
	//GPU가 펜스의 값을 설정하는 명령을 명령 큐에 추가한다.
	if (m_createMgr.m_pFence->GetCompletedValue() < nFence)
	{
		//펜스의 현재 값이 설정한 값보다 작으면 펜스의 현재 값이 설정한 값이 될 때까지 기다린다.
		hResult = m_createMgr.m_pFence->SetEventOnCompletion(nFence, m_createMgr.m_hFenceEvent);
		::WaitForSingleObject(m_createMgr.m_hFenceEvent, INFINITE);
	}
}