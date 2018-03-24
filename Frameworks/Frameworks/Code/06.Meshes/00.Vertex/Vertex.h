#pragma once

////////////////////////////////////////////////////////////////////////
// 색상 정점
class CDiffusedVertex
{
protected:
	XMFLOAT3						m_position;
	XMFLOAT4						m_diffuseColor;
public:
	CDiffusedVertex() {}
	CDiffusedVertex(float x, float y, float z, XMFLOAT4 xmf4Diffuse)
	{
		m_position = XMFLOAT3(x, y, z);
		m_diffuseColor = xmf4Diffuse;
	}
	CDiffusedVertex(XMFLOAT3 xmf3Position, XMFLOAT4 xmf4Diffuse)
	{
		m_position = xmf3Position;
		m_diffuseColor = xmf4Diffuse;
	}
	~CDiffusedVertex() { }
};

////////////////////////////////////////////////////////////////////////
// 텍스처 정점
class CTexturedVertex
{
public:
	XMFLOAT3						m_position;
	XMFLOAT2						m_texCoord{ XMFLOAT2(0.0f, 0.0f) };

public:
	CTexturedVertex() {}
	CTexturedVertex(float x, float y, float z, XMFLOAT2 xmf2TexCoord)
	{ 
		m_position = XMFLOAT3(x, y, z);
		m_texCoord = xmf2TexCoord; 
	}
	CTexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f))
	{
		m_position = xmf3Position;
		m_texCoord = xmf2TexCoord; 
	}
	~CTexturedVertex() { }
};

////////////////////////////////////////////////////////////////////////
//
class CIlluminatedVertex
{
protected:
	XMFLOAT3						m_position;
	XMFLOAT3						m_normal;

public:
	CIlluminatedVertex() { m_position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_normal = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CIlluminatedVertex(float x, float y, float z, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f)) { m_position = XMFLOAT3(x, y, z); m_normal = xmf3Normal; }
	CIlluminatedVertex(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f)) { m_position = xmf3Position; m_normal = xmf3Normal; }
	~CIlluminatedVertex() { }
};

////////////////////////////////////////////////////////////////////////
//
class CIlluminatedTexturedVertex
{
protected:
	XMFLOAT3						m_position;
	XMFLOAT3						m_normal;
	XMFLOAT2						m_xmf2TexCoord;
	XMFLOAT3						m_xmf3Tangent;

public:
	CIlluminatedTexturedVertex() { m_position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f); m_normal = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CIlluminatedTexturedVertex(float x, float y, float z, XMFLOAT2 xmf2TexCoord, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT3 xmf3Tangent = XMFLOAT3(0.0f, 0.0f, 0.0f)) { m_position = XMFLOAT3(x, y, z); m_normal = xmf3Normal; m_xmf2TexCoord = xmf2TexCoord; }
	CIlluminatedTexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f), XMFLOAT3 xmf3Tangent = XMFLOAT3(0.0f, 0.0f, 0.0f)) { m_position = xmf3Position; m_normal = xmf3Normal; m_xmf2TexCoord = xmf2TexCoord; m_xmf3Tangent = xmf3Tangent; }
	~CIlluminatedTexturedVertex() { }
};

////////////////////////////////////////////////////////////////////////
//
class CDiffuseTexturedVertex
{
protected:
	XMFLOAT3						m_position;
	XMFLOAT2						m_xmf2TexCoord;
	XMFLOAT4						m_diffuseColor;

public:
	CDiffuseTexturedVertex() { m_position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f); m_diffuseColor = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f); }
	CDiffuseTexturedVertex(float x, float y, float z, XMFLOAT2 xmf2TexCoord, XMFLOAT3, XMFLOAT4 xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)) { m_position = XMFLOAT3(x, y, z); m_xmf2TexCoord = xmf2TexCoord;  m_diffuseColor = xmf4Diffuse; }
	CDiffuseTexturedVertex(XMFLOAT3 xmf3Position, XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f), XMFLOAT4 xmf4Diffuse = XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f)) { m_position = xmf3Position; m_xmf2TexCoord = xmf2TexCoord; m_diffuseColor = xmf4Diffuse; }
	~CDiffuseTexturedVertex() { }
};

////////////////////////////////////////////////////////////////////////////
class CSkinnedVertex 
{
protected:
	XMFLOAT3						m_position;
	XMFLOAT3						m_normal;
	XMFLOAT2						m_xmf2TexCoord;
	XMFLOAT3						m_xmf3Tangent;

	XMFLOAT4						m_xmf4SkinWeight;
	XMFLOAT4						m_xmf4SkinIndex;

public:
	CSkinnedVertex() { m_position = XMFLOAT3(0.0f, 0.0f, 0.0f); m_xmf2TexCoord = XMFLOAT2(0.0f, 0.0f); m_normal = XMFLOAT3(0.0f, 0.0f, 0.0f); }
	CSkinnedVertex(XMFLOAT3 xmf3Position, XMFLOAT3 xmf3Normal = XMFLOAT3(0.0f, 0.0f, 0.0f), XMFLOAT2 xmf2TexCoord = XMFLOAT2(0.0f, 0.0f), XMFLOAT3 xmf3Tangent = XMFLOAT3(0.0f, 0.0f, 0.0f)
		,XMFLOAT4 xmf4SkinWeight = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f), XMFLOAT4 xmf4SkinIndex = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f)) 
	{
		m_position = xmf3Position; m_normal = xmf3Normal; m_xmf2TexCoord = xmf2TexCoord; m_xmf3Tangent = xmf3Tangent;
		m_xmf4SkinWeight = xmf4SkinWeight; m_xmf4SkinIndex = xmf4SkinIndex;
	}


	void SetPosition(XMFLOAT3 position) {m_position = position; }
	XMFLOAT3 GetPosition() { return m_position; }
	XMFLOAT4 GetSkinWeight() { return m_xmf4SkinWeight; }
	XMFLOAT4 GetSkinSkinIndex() { return m_xmf4SkinIndex; }

	~CSkinnedVertex() {}
};
////////////////////////////////////////////////////////////////////////////