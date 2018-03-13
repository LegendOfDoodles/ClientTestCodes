#pragma once
#include "stdafx.h"
#include <Vector>
struct MyVertex {
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	XMFLOAT4 skinweight;
	XMFLOAT4 skinindex;
	XMFLOAT2 uv;
	~MyVertex() {}
};

class CMeshImporter
{
public:
	int m_iTriCnt;
	std::vector<XMFLOAT3> m_xmTriIndex;
	int m_iVerticesCnt;
	std::vector<MyVertex> m_xmVertex;
	char* m_FileRoute;
public:
	CMeshImporter();

	void LoadMeshData(char* in);
	
	~CMeshImporter();

};

