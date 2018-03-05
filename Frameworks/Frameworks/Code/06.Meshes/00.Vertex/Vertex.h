#pragma once

////////////////////////////////////////////////////////////////////////
// 기본 정점
class CVertex
{
protected:
	//정점의 위치 벡터이다(모든 정점은 최소한 위치 벡터를 가져야 한다).
	XMFLOAT3 m_position{ XMFLOAT3(0.0f, 0.0f, 0.0f) };
public:
	CVertex() {}
	CVertex(float x, float y, float z) { m_position.x = x; m_position.y = y; m_position.z = z; }
	CVertex(XMFLOAT3 xmf3Position) { m_position = xmf3Position; }
	~CVertex() { }
};

////////////////////////////////////////////////////////////////////////
// 색상 정점
class CDiffusedVertex : public CVertex
{
protected:
	//정점의 색상이다.
	XMFLOAT4 m_diffuseColor{ XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f) };
public:
	CDiffusedVertex() {}
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse) : CVertex(x, y, z)
	{
		m_diffuseColor = xmf4Diffuse;
	}
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse) : CVertex(xmf3Position)
	{
		m_diffuseColor = xmf4Diffuse;
	}
	~CDiffusedVertex() { }
};

////////////////////////////////////////////////////////////////////////
// 텍스처 정점
class CTexturedVertex : public CVertex
{
public:
	XMFLOAT2						m_texCoord{ XMFLOAT2(0.0f, 0.0f) };

public:
	CTexturedVertex() {}
	CTexturedVertex(float x, float y, float z, XMFLOAT2 xmf2TexCoord) : CVertex(x, y, z)
	{ 
		m_texCoord = xmf2TexCoord; 
	}
	CTexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f)) : CVertex(xmf3Position)
	{
		m_texCoord = xmf2TexCoord; 
	}
	~CTexturedVertex() { }
};

////////////////////////////////////////////////////////////////////////
// 텍스처 + 색상 정점
class CDiffusedTexturedVertex : public CDiffusedVertex
{
public:
	XMFLOAT2						m_texCoord{ XMFLOAT2(0.0f, 0.0f) };
	XMFLOAT3						m_xmf3Tangent;

public:
	CDiffusedTexturedVertex() {}
	CDiffusedTexturedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse, XMFLOAT2 xmf2TexCoord) : CDiffusedVertex(x, y, z, xmf4Diffuse)
	{ 
		m_texCoord = xmf2TexCoord; 
	}
	CDiffusedTexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), 
		XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f)) : CDiffusedVertex(xmf3Position, xmf4Diffuse)
	{ 
		m_texCoord = xmf2TexCoord; 
	}
	~CDiffusedTexturedVertex() { }
};

////////////////////////////////////////////////////////////////////////
//
class CIlluminatedVertex : public CVertex
{
protected:
	XMFLOAT3						m_normal;

public:
	CIlluminatedVertex() { m_position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_normal = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CIlluminatedVertex(float x, float y, float z, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f)) { m_position = XMFLOAT3(x, y, z); m_normal = xmf3Normal; }
	CIlluminatedVertex(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f)) { m_position = xmf3Position; m_normal = xmf3Normal; }
	~CIlluminatedVertex() { }
};

////////////////////////////////////////////////////////////////////////
//
class CIlluminatedTexturedVertex : public CIlluminatedVertex
{
protected:
	XMFLOAT2						m_xmf2TexCoord;
	XMFLOAT3						m_xmf3Tangent;

public:
	CIlluminatedTexturedVertex() { m_position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f); m_normal = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CIlluminatedTexturedVertex(float x, float y, float z, XMFLOAT2 xmf2TexCoord, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3 xmf3Tangent = XMFLOAT3(0.0f, 0.0f, 0.0f)) { m_position = XMFLOAT3(x, y, z); m_normal = xmf3Normal; m_xmf2TexCoord = xmf2TexCoord; }
	CIlluminatedTexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f), XMFLOAT3 xmf3Tangent = XMFLOAT3(0.0f, 0.0f, 0.0f)) { m_position = xmf3Position; m_normal = xmf3Normal; m_xmf2TexCoord = xmf2TexCoord; m_xmf3Tangent = xmf3Tangent; }
	~CIlluminatedTexturedVertex() { }
};