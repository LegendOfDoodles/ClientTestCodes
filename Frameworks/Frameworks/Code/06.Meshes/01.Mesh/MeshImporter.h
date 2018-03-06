#pragma once
#include "stdafx.h"
struct MyVertex {
	XMFLOAT3 pos;
	XMFLOAT3 normal;
	double skinweight[10];
	int skinindex[10];
	double uv[10];
	~MyVertex() {}
};

class CMeshImporter
{
public:
	int m_iTriCnt;
	std::vector<XMFLOAT3> m_xmTriIndex;
	int m_iVerticesCnt;
	std::vector<MyVertex> m_Vertices;
	char* m_FileRoute;
public:
	CMeshImporter(char* in);


	~CMeshImporter();

	int GetVerticesCount() { return m_iVerticesCnt; }
};

