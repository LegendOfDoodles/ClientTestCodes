#include "stdafx.h"
#include "Tester.h"
#include "Mesh.h"

void UploadVertexBuffer(
	ID3D12GraphicsCommandList *pd3dCommandList,
	void *pData,
	UINT nBytes,
	CMesh *pMesh)
{
	static bool isInitialized{ false };
	static D3D12_HEAP_PROPERTIES heapPropertiesDesc;

	ID3D12Resource *pVertexBuffer{ pMesh->GetVertexBuffer() };
	ID3D12Resource *pUploadBuffer{ pMesh->GetVertexUploadBuffer() };

	if (!isInitialized)
	{
		::ZeroMemory(&heapPropertiesDesc, sizeof(D3D12_HEAP_PROPERTIES));
		heapPropertiesDesc.Type = D3D12_HEAP_TYPE_DEFAULT;
		heapPropertiesDesc.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
		heapPropertiesDesc.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
		heapPropertiesDesc.CreationNodeMask = 1;
		heapPropertiesDesc.VisibleNodeMask = 1;

		isInitialized = true;
	}

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

	static D3D12_RANGE readRange{ 0, 0 };

	UINT8 *pBufferDataBegin{ NULL };

	pUploadBuffer->Map(0, &readRange, (void **)&pBufferDataBegin);
	memcpy(pBufferDataBegin, pData, nBytes);
	pUploadBuffer->Unmap(0, NULL);

	//업로드 버퍼의 내용을 디폴트 버퍼에 복사한다.
	pd3dCommandList->CopyResource(pVertexBuffer, pUploadBuffer);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = pVertexBuffer;
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	d3dResourceBarrier.Transition.Subresource =
		D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
}