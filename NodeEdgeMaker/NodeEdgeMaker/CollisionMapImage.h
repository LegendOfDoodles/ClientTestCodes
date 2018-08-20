#pragma once
class CCollisionMapImage
{
public: // 생성자, 소멸자
	CCollisionMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	virtual ~CCollisionMapImage(void);

public: // 공개 함수
	bool GetCollision(float x, float z);

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }

	BYTE *GetCollisionMapPixels() { return(m_pCollisionMapPixels); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }

private: // 변수
	BYTE * m_pCollisionMapPixels{ NULL };
	int m_nWidth;
	int m_nLength;

	XMFLOAT3 m_xmf3Scale;
};