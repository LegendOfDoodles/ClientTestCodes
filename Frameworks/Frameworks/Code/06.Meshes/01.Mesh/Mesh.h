#pragma once

class CCreateMgr;
class CSkinnedVertex;
////////////////////////////////////////////////////////////////////////
// ±âº» ¸Þ½¬
class CMesh
{
public: // »ý¼ºÀÚ, ¼Ò¸êÀÚ
	CMesh(CCreateMgr *pCreateMgr);
	virtual ~CMesh();

public: // °ø°³ ÇÔ¼ö
	virtual void Render(UINT istanceCnt = 1);

	virtual void ReleaseUploadBuffers();
	
	ID3D12Resource* GetVertexBuffer() { 
		return m_pVertexBuffer; 
	}
	ID3D12Resource* GetVertexUploadBuffer() { 
		return m_pVertexUploadBuffer;
	}
	void UploadVertexBuffer(
		ID3D12GraphicsCommandList *pd3dCommandList,
		void *pData,
		UINT nBytes);
	void AddRef() { m_nReferences++; }
	void Release() { if (--m_nReferences <= 0) delete this; }

protected: // ³»ºÎ ÇÔ¼ö
	void CalculateTriangleListVertexTangents(XMFLOAT3 *pxmf3Tangents, XMFLOAT3 *pxmf3Positions, UINT nVertices, XMFLOAT2 *xmf2TexCoord, UINT *pnIndices = NULL, UINT nIndices = NULL);
	void CalculateTriangleStripVertexTangents(XMFLOAT3 *pxmf3Tangents, XMFLOAT3 *pxmf3Positions, UINT nVertices, XMFLOAT2 *xmf2TexCoord, UINT *pnIndices = NULL, UINT nIndices = NULL);

protected: // º¯¼ö
	int m_nReferences{ 0 };

	ID3D12Resource *m_pVertexBuffer{ NULL };
	ID3D12Resource *m_pVertexUploadBuffer{ NULL };

	ID3D12Resource *m_pIndexBuffer{ NULL };
	ID3D12Resource *m_pIndexUploadBuffer{ NULL };

	D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
	D3D12_INDEX_BUFFER_VIEW m_indexBufferView;

	D3D12_PRIMITIVE_TOPOLOGY m_primitiveTopology{ D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST };

	UINT m_nVertices{ 0 };
	UINT m_nIndices{ 0 };

	UINT m_nSlot{ 0 };
	UINT m_nStride{ 0 };
	UINT m_nOffset{ 0 };

	UINT m_nStartIndex{ 0 };
	UINT m_nBaseVertex{ 0 };

	ID3D12GraphicsCommandList *m_pCommandList{ NULL };
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ÅØ½ºÃ³ ¸Þ½¬
class CMeshTextured : public CMesh
{
public:
	CMeshTextured(CCreateMgr *pCreateMgr);
	virtual ~CMeshTextured();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ÅØ½ºÃ³ »óÀÚ ¸Þ½¬
class CCubeMeshTextured : public CMeshTextured
{
public:
	CCubeMeshTextured(CCreateMgr *pCreateMgr, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshTextured();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMeshIlluminated : public CMesh
{
public:
	CMeshIlluminated(CCreateMgr *pCreateMgr);
	virtual ~CMeshIlluminated();

public:
	void CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices);
	void CalculateTriangleListVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices);
	void CalculateTriangleStripVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, UINT nVertices, UINT *pnIndices, UINT nIndices);
	void CalculateVertexNormals(XMFLOAT3 *pxmf3Normals, XMFLOAT3 *pxmf3Positions, int nVertices, UINT *pnIndices, int nIndices);
};

class CCubeMeshIlluminated : public CMeshIlluminated
{
public:
	CCubeMeshIlluminated(CCreateMgr *pCreateMgr, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshIlluminated();
};


class CFBXMesh :public CMeshIlluminated
{
public :
	CFBXMesh(CCreateMgr *pCreateMgr);
	virtual ~CFBXMesh();
};

class CSphereMeshIlluminated : public CMeshIlluminated
{
public:
	CSphereMeshIlluminated(CCreateMgr *pCreateMgr, float fRadius = 2.0f, UINT nSlices = 20, UINT nStacks = 20);
	virtual ~CSphereMeshIlluminated();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CMeshIlluminatedTextured : public CMeshIlluminated
{
public:
	CMeshIlluminatedTextured(CCreateMgr *pCreateMgr);
	CMeshIlluminatedTextured(CCreateMgr *pCreateMgr, UINT nVertices, XMFLOAT3 *pxmf3Positions, XMFLOAT3 *pxmf3Normals, XMFLOAT2 *pxmf2UVs, UINT nIndices, UINT *pnIndices);
	virtual ~CMeshIlluminatedTextured();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CCubeMeshIlluminatedTextured : public CMeshIlluminatedTextured
{
public:
	CCubeMeshIlluminatedTextured(CCreateMgr *pCreateMgr, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeMeshIlluminatedTextured();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CCubeWithIndices : public CMeshIlluminatedTextured
{
public:
	CCubeWithIndices(CCreateMgr *pCreateMgr, float fWidth = 2.0f, float fHeight = 2.0f, float fDepth = 2.0f);
	virtual ~CCubeWithIndices();
};

class CSkinnedMesh : public CMeshIlluminatedTextured
{
public:
	CSkinnedVertex* m_pVertices;
	int m_nVerticesCnt;

	CSkinnedMesh(CCreateMgr* pCreateMgr, char* in);
	UINT GetStride() { return m_nStride; }
	virtual void ReleaseUploadBuffers();
	virtual ~CSkinnedMesh();
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CHeightMapImage
{
private:
	//³ôÀÌ ¸Ê ÀÌ¹ÌÁö ÇÈ¼¿(8-ºñÆ®)µéÀÇ ÀÌÂ÷¿ø ¹è¿­ÀÌ´Ù. °¢ ÇÈ¼¿Àº 0~255ÀÇ °ªÀ» °®´Â´Ù.
	BYTE *m_pHeightMapPixels;
	//³ôÀÌ ¸Ê ÀÌ¹ÌÁöÀÇ °¡·Î¿Í ¼¼·Î Å©±âÀÌ´Ù.
	int m_nWidth;
	int m_nLength;
	//³ôÀÌ ¸Ê ÀÌ¹ÌÁö¸¦ ½ÇÁ¦·Î ¸î ¹è È®´ëÇÏ¿© »ç¿ëÇÒ °ÍÀÎ°¡¸¦ ³ªÅ¸³»´Â ½ºÄÉÀÏ º¤ÅÍÀÌ´Ù.
	XMFLOAT3 m_xmf3Scale;

public:
	CHeightMapImage(LPCTSTR pFileName, int nWidth, int nLength, XMFLOAT3 xmf3Scale);
	~CHeightMapImage(void);

	//³ôÀÌ ¸Ê ÀÌ¹ÌÁö¿¡¼­ (x, z) À§Ä¡ÀÇ ÇÈ¼¿ °ª¿¡ ±â¹ÝÇÑ ÁöÇüÀÇ ³ôÀÌ¸¦ ¹ÝÈ¯ÇÑ´Ù.
	float GetHeight(float x, float z);
	//³ôÀÌ ¸Ê ÀÌ¹ÌÁö¿¡¼­ (x, z) À§Ä¡ÀÇ ¹ý¼± º¤ÅÍ¸¦ ¹ÝÈ¯ÇÑ´Ù.
	XMFLOAT3 GetHeightMapNormal(int x, int z);

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }

	BYTE *GetHeightMapPixels() { return(m_pHeightMapPixels); }
	int GetHeightMapWidth() { return(m_nWidth); }
	int GetHeightMapLength() { return(m_nLength); }
};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CHeightMapGridMesh : public CMesh
{
protected:
	//°ÝÀÚÀÇ Å©±â(°¡·Î: x-¹æÇâ, ¼¼·Î: z-¹æÇâ)ÀÌ´Ù.
	int m_nWidth;
	int m_nLength;
	/*°ÝÀÚÀÇ ½ºÄÉÀÏ(°¡·Î: x-¹æÇâ, ¼¼·Î: z-¹æÇâ, ³ôÀÌ: y-¹æÇâ) º¤ÅÍÀÌ´Ù. ½ÇÁ¦ °ÝÀÚ ¸Þ½¬ÀÇ °¢ Á¤Á¡ÀÇ x-ÁÂÇ¥, y-ÁÂÇ¥,
	z-ÁÂÇ¥´Â ½ºÄÉÀÏ º¤ÅÍÀÇ x-ÁÂÇ¥, y-ÁÂÇ¥, z-ÁÂÇ¥·Î °öÇÑ °ªÀ» °®´Â´Ù. Áï, ½ÇÁ¦ °ÝÀÚÀÇ x-Ãà ¹æÇâÀÇ °£°ÝÀº 1ÀÌ ¾Æ´Ï
	¶ó ½ºÄÉÀÏ º¤ÅÍÀÇ x-ÁÂÇ¥°¡ µÈ´Ù. ÀÌ·¸°Ô ÇÏ¸é ÀÛÀº °ÝÀÚ(ÀûÀº Á¤Á¡)¸¦ »ç¿ëÇÏ´õ¶óµµ Å« Å©±âÀÇ °ÝÀÚ(ÁöÇü)¸¦ »ý¼ºÇÒ
	¼ö ÀÖ´Ù.*/
	XMFLOAT3 m_xmf3Scale;

public:
	CHeightMapGridMesh(CCreateMgr *pCreateMgr, int xStart, int zStart, int nWidth, int nLength, XMFLOAT3 xmf3Scale =
		XMFLOAT3(1.0f, 1.0f, 1.0f), XMFLOAT4 xmf4Color = XMFLOAT4(1.0f, 1.0f, 0.0f, 0.0f), void
		*pContext = NULL);
	virtual ~CHeightMapGridMesh();

	XMFLOAT3 GetScale() { return(m_xmf3Scale); }
	int GetWidth() { return(m_nWidth); }
	int GetLength() { return(m_nLength); }

	//°ÝÀÚÀÇ ÁÂÇ¥°¡ (x, z)ÀÏ ¶§ ±³Á¡(Á¤Á¡)ÀÇ ³ôÀÌ¸¦ ¹ÝÈ¯ÇÏ´Â ÇÔ¼öÀÌ´Ù.
	virtual float OnGetHeight(int x, int z, void *pContext);
	//°ÝÀÚÀÇ ÁÂÇ¥°¡ (x, z)ÀÏ ¶§ ±³Á¡(Á¤Á¡)ÀÇ »ö»óÀ» ¹ÝÈ¯ÇÏ´Â ÇÔ¼öÀÌ´Ù.
	virtual XMFLOAT4 OnGetColor(int x, int z, void *pContext);
};

<<<<<<< HEAD


////////////////////////////////////////////////////////
void UploadVertexBuffer(
	ID3D12GraphicsCommandList *pd3dCommandList,
	void *pData,
	UINT nBytes,
	CMesh *pMesh);
=======
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
class CTexturedRectMesh : public CMesh
{
public:
	CTexturedRectMesh(CCreateMgr *pCreateMgr, float fWidth = 20.0f, float fHeight = 20.0f, float fDepth = 20.0f, float fxPosition = 0.0f, float fyPosition = 0.0f, float fzPosition = 0.0f);
	virtual ~CTexturedRectMesh();
};
>>>>>>> ê¹€ë‚˜ë‹¨
