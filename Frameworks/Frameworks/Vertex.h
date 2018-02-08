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
// 2d 텍스처 정점
class CDiffused2TexturedVertex : public CDiffusedVertex
{
public:
	XMFLOAT2						m_texCoord0{ XMFLOAT2(0.0f, 0.0f) };
	XMFLOAT2						m_texCoord1{ XMFLOAT2(0.0f, 0.0f) };

public:
	CDiffused2TexturedVertex() {}
	CDiffused2TexturedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse, XMFLOAT2 xmf2TexCoord0, XMFLOAT2 xmf2TexCoord1)
		: CDiffusedVertex(x, y, z, xmf4Diffuse)
	{
		m_texCoord0 = xmf2TexCoord0; 
		m_texCoord1 = xmf2TexCoord1; 
	}
	CDiffused2TexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), 
		XMFLOAT2 xmf2TexCoord0 = XMFLOAT2(0.0f, 0.0f), XMFLOAT2 xmf2TexCoord1 = XMFLOAT2(0.0f, 0.0f)) 
		: CDiffusedVertex(xmf3Position, xmf4Diffuse)
	{
		m_texCoord0 = xmf2TexCoord0; 
		m_texCoord1 = xmf2TexCoord1; 
	}
	~CDiffused2TexturedVertex() { }
};
