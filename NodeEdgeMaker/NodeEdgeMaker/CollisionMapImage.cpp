#include "stdafx.h"
#include "CollisionMapImage.h"

////////////////////////////////////////////////////////////////////////
// 생성자, 소멸자
CCollisionMapImage::CCollisionMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale)
{
	m_nWidth = nWidth;
	m_nLength = nLength;
	m_xmf3Scale = xmf3Scale;
	BYTE *pCollisionMapPixels = new BYTE[m_nWidth * m_nLength];

	HANDLE hFile = ::CreateFile(pFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL | FILE_ATTRIBUTE_READONLY, NULL);
	DWORD dwBytesRead;
	::ReadFile(hFile, pCollisionMapPixels, (m_nWidth * m_nLength), &dwBytesRead, NULL);
	::CloseHandle(hFile);

	m_pCollisionMapPixels = new BYTE[m_nWidth * m_nLength];
	for (int y = 0; y < m_nLength; y++)
	{
		for (int x = 0; x < m_nWidth; x++)
		{
			m_pCollisionMapPixels[x + ((m_nLength - 1 - y)*m_nWidth)] = pCollisionMapPixels[x + (y*m_nWidth)];
		}
	}

	if (pCollisionMapPixels) Safe_Delete_Array(pCollisionMapPixels);
}

CCollisionMapImage::~CCollisionMapImage(void)
{
	if (m_pCollisionMapPixels) Safe_Delete_Array(m_pCollisionMapPixels);
}

////////////////////////////////////////////////////////////////////////
// 공개 함수
bool CCollisionMapImage::GetCollision(float fx, float fz)
{
	fx = fx / m_xmf3Scale.x;
	fz = fz / m_xmf3Scale.z;

	// 좌표를 벗어나면 항상 콜리전이 일어나도록 한다.
	if ((fx <= 1.0f) || (fz <= 1.0f) || (fx >= m_nWidth - 1) || (fz >= m_nLength - 1)) return(true);

	//높이 맵의 좌표의 정수 부분과 소수 부분을 계산한다.
	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;

	// 정수부 기준으로 사각형을 짠다.
	float fBottomLeft{ (float)m_pCollisionMapPixels[x + (z * m_nWidth)] };
	float fBottomRight{ (float)m_pCollisionMapPixels[(x + 1) + (z * m_nWidth)] };
	float fTopLeft{ (float)m_pCollisionMapPixels[x + ((z + 1) * m_nWidth)] };
	float fTopRight{ (float)m_pCollisionMapPixels[(x + 1) + ((z + 1) * m_nWidth)] };

	// 소수부 위치를 처리하기 위해 보간 처리 한다.
	float xUpCenter{ fTopLeft * fxPercent + fTopRight * (1 - fxPercent) };
	float xDownCenter{ fBottomLeft * fxPercent + fBottomRight * (1 - fxPercent) };

	return xUpCenter * (1 - fzPercent) + xDownCenter * fzPercent > 250.f;
}
