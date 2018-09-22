// stdafx.h : 자주 사용하지만 자주 변경되지는 않는
// 표준 시스템 포함 파일 또는 프로젝트 관련 포함 파일이
// 들어 있는 포함 파일입니다.
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>

#include <fstream>

#include <Windows.h>

#include <d3d12.h>
#include <dxgi1_4.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <DirectXColors.h>
#include <DirectXCollision.h>

#include <vector>
#include <list>

using namespace DirectX;
using namespace DirectX::PackedVector;

// TODO: 프로그램에 필요한 추가 헤더는 여기에서 참조합니다.

#include "Edge.h"
#include "Node.h"
#include "CollisionMapImage.h"
#include "Vectors.h"

#define TERRAIN_SIZE_WIDTH 10000
#define TERRAIN_SIZE_HEIGHT 5000

#define TERRAIN_IMAGE_WIDTH 1000
#define TERRAIN_IMAGE_HEIGHT 500
#define TERRAIN_IMAGE_SCALE XMFLOAT3(TERRAIN_SIZE_WIDTH / TERRAIN_IMAGE_WIDTH, 0.7f, TERRAIN_SIZE_HEIGHT / TERRAIN_IMAGE_HEIGHT)

#define NODE_SIZE 41.4f
#define NODE_SIZE_HALF NODE_SIZE * 0.5f
#define NODE_SIZE_SQR NODE_SIZE * NODE_SIZE
#define NODE_SIZE_HALF_SQR NODE_SIZE_HALF * NODE_SIZE_HALF
#define NONE -1

// 단위 변환 함수
#define CONVERT_PaperUnit_to_InG(val) val * 8.28f
#define CONVERT_Unit_to_InG(val) val * 1000.0f
#define CONVERT_cm_to_InG(val) val * 34.5f

typedef std::vector<CNode> NodeVector;
typedef std::vector<CEdge> EdgeVector;
typedef std::vector<EdgeVector> EdgeArray;
typedef std::list<CPathEdge> Path;

template <typename T>
inline void Safe_Delete(T& p)
{
	if (!p) return;

	delete p;
	p = NULL;
}

template <typename T>
inline void Safe_Delete_Array(T& p)
{
	if (!p) return;

	delete[] p;
	p = NULL;
}

template <typename T>
inline void Safe_Release(T& p)
{
	if (!p) return;

	p->Release();
	p = NULL;
}