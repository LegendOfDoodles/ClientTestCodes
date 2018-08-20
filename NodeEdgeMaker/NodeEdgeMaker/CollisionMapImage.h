#pragma once
class CCollisionMapImage
{
public: // ������, �Ҹ���
	CCollisionMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	virtual ~CCollisionMapImage(void);

public: // ���� �Լ�
	bool GetCollision(float x, float z);

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }

	BYTE *GetCollisionMapPixels() { return(m_pCollisionMapPixels); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }

private: // ����
	BYTE * m_pCollisionMapPixels{ NULL };
	int m_nWidth;
	int m_nLength;

	XMFLOAT3 m_xmf3Scale;
};