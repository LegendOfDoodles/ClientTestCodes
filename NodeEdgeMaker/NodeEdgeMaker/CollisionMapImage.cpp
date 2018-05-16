#include "stdafx.h"
#include "CollisionMapImage.h"

////////////////////////////////////////////////////////////////////////
// ������, �Ҹ���
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
// ���� �Լ�
bool CCollisionMapImage::GetCollision(float fx, float fz)
{
	fx = fx / m_xmf3Scale.x;
	fz = fz / m_xmf3Scale.z;

	// ��ǥ�� ����� �׻� �ݸ����� �Ͼ���� �Ѵ�.
	if ((fx < 0.0f) || (fz < 0.0f) || (fx >= m_nWidth) || (fz >= m_nLength)) return(true);

	//���� ���� ��ǥ�� ���� �κа� �Ҽ� �κ��� ����Ѵ�.
	int x = (int)fx;
	int z = (int)fz;
	float fxPercent = fx - x;
	float fzPercent = fz - z;

	float fBottomLeft = (float)m_pCollisionMapPixels[x + (z*m_nWidth)];
	float fBottomRight = (float)m_pCollisionMapPixels[(x + 1) + (z*m_nWidth)];
	float fTopLeft = (float)m_pCollisionMapPixels[x + ((z + 1)*m_nWidth)];
	float fTopRight = (float)m_pCollisionMapPixels[(x + 1) + ((z + 1)*m_nWidth)];

	//�簢���� �� �� �� �ΰ� �̻��� �浹 ������ ��� �浹�ϴ� ������ �����Ѵ�.
	float sumValue = fBottomLeft + fBottomRight + fTopLeft + fTopRight;

	return(sumValue >= 510);
}
