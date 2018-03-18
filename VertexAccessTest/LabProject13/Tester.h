#pragma once

class CMesh;

void UploadVertexBuffer(
	ID3D12GraphicsCommandList *pd3dCommandList,
	void *pData,
	UINT nBytes,
	CMesh *pMesh);