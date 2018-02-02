#define USE_INSTANCING 1
Texture2D gtxtTexture : register(t1);
SamplerState wrapSampler : register(s0);

//���� ��ü�� ������ ���� ��� ���۸� �����Ѵ�.
cbuffer cbPlayerInfo : register(b0)
{
	matrix gmtxWorld : packoffset(c0);
};

//ī�޶��� ������ ���� ��� ���۸� �����Ѵ�.
cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

//���� ���̴��� �Է��� ���� ����ü�� �����Ѵ�.
struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

//���� ���̴��� ���(�ȼ� ���̴��� �Է�)�� ���� ����ü�� �����Ѵ�.
struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

//���� ���̴��� �����Ѵ�.
VS_OUTPUT VSDiffused(VS_INPUT input)
{
	VS_OUTPUT output;
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView),
		gmtxProjection);
	output.color = input.color;

	return(output);
}

//�ȼ� ���̴��� �����Ѵ�.
float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

///////////////////////////////////////////////////////////////////////////////////////////
//
#if USE_INSTANCING
struct INSTANCEDGAMEOBJECTINFO
{
	matrix m_mtxObject;
	float4 m_cColor;
};

StructuredBuffer<INSTANCEDGAMEOBJECTINFO> gGameObjectInfos : register(t0);

VS_OUTPUT VSInstancing(VS_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_OUTPUT output;
	output.position = mul(mul(mul(float4(input.position, 1.0f),
		gGameObjectInfos[nInstanceID].m_mtxObject), gmtxView), gmtxProjection);
	output.color = input.color + gGameObjectInfos[nInstanceID].m_cColor;
	return(output);
}
#else
cbuffer cbObjectInfo : register(b2)
{
	matrix	gmtxObject : packoffset(c0);
	float4 cColor  : packoffset(c4);
};

VS_OUTPUT VSObject(VS_INPUT input)
{
	VS_OUTPUT output;
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxObject), gmtxView), gmtxProjection);
	output.color = input.color + cColor;

	return(output);
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////
//
//