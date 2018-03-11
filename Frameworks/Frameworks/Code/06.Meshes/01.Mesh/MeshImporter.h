#pragma once
#include "stdafx.h"
#include <Vector>
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
	std::vector<MyVertex> m_xmVertex;
	char* m_FileRoute;
public:
	CMeshImporter();

	void LoadMeshData(char* in);
	
	~CMeshImporter();

};

struct KeyFrame {
	int m_iframe;
	XMFLOAT4X4  m_m4x4Matrix;
};

struct MySkeleton {
	int m_iID;
	int m_iParentID;
	XMFLOAT4X4  m_m4x4Matrix;
	std::vector<KeyFrame> m_Keyframe;
};
class CAnimationImporter {
	std::vector<MySkeleton> skeletons;
public:
	CAnimationImporter();
	void LoadAnimationData(char* in);
	~CAnimationImporter();
};

