#include "stdafx.h"
#include "Tester.h"
#include "Mesh.h"

void UploadVertexBuffer(
	ID3D12GraphicsCommandList *pd3dCommandList,
	void *pData,
	UINT nBytes,
	CMesh *pMesh)
{
	ID3D12Resource *pVertexBuffer{ pMesh->GetVertexBuffer() };
	ID3D12Resource *pUploadBuffer{ pMesh->GetVertexUploadBuffer() };

	D3D12_RANGE readRange{ 0, 0 };
	UINT8 *pBufferDataBegin{ NULL };

	pUploadBuffer->Map(0, &readRange, (void **)&pBufferDataBegin);
	memcpy(pBufferDataBegin, pData, nBytes);
	pUploadBuffer->Unmap(0, NULL);

	D3D12_RESOURCE_BARRIER d3dResourceBarrier;
	::ZeroMemory(&d3dResourceBarrier, sizeof(D3D12_RESOURCE_BARRIER));
	d3dResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	d3dResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	d3dResourceBarrier.Transition.pResource = pVertexBuffer;
	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
	d3dResourceBarrier.Transition.Subresource =
		D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

	pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);

	//업로드 버퍼의 내용을 디폴트 버퍼에 복사한다.
	pd3dCommandList->CopyResource(pVertexBuffer, pUploadBuffer);

	d3dResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	d3dResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;

	pd3dCommandList->ResourceBarrier(1, &d3dResourceBarrier);
}