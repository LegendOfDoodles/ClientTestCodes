#define USE_INSTANCING 1
Texture2D gtxtTexture : register(t0);
SamplerState wrapSampler : register(s0);

//게임 객체의 정보를 위한 상수 버퍼를 선언한다.
cbuffer cbPlayerInfo : register(b0)
{
	matrix gmtxWorld : packoffset(c0);
};

//카메라의 정보를 위한 상수 버퍼를 선언한다.
cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};

#if USE_INSTANCING
struct INSTANCE_INFO
{
	matrix m_mtxGameObject;
	uint	  m_nMaterial;
};

StructuredBuffer<INSTANCE_INFO> gGameObjectInfos : register(t1);
#else
cbuffer cbGameObjectInfo : register(b2)
{
	matrix		gmtxGameObject : packoffset(c0);
	uint		gnMaterial : packoffset(c4);
};
#endif

//정점 셰이더의 입력을 위한 구조체를 선언한다.
struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};

//정점 셰이더의 출력(픽셀 셰이더의 입력)을 위한 구조체를 선언한다.
struct VS_OUTPUT
{
	float4 position : SV_POSITION;
	float4 color : COLOR;
};

//정점 셰이더를 정의한다.
VS_OUTPUT VSDiffused(VS_INPUT input)
{
	VS_OUTPUT output;
	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView),
		gmtxProjection);
	output.color = input.color;

	return(output);
}

//픽셀 셰이더를 정의한다.
float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
	return(input.color);
}

///////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_TEXTURED_INPUT
{
	float3 position : POSITION;
	float2 uv : TEXCOORD;
};

struct VS_TEXTURED_OUTPUT
{
	float4 position : SV_POSITION;
	float2 uv : TEXCOORD;
};

#if USE_INSTANCING
VS_TEXTURED_OUTPUT VSTxtInstancing(VS_TEXTURED_INPUT input, uint nInstanceID : SV_InstanceID)
{
	VS_TEXTURED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gGameObjectInfos[nInstanceID].m_mtxGameObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}
#else
VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}
#endif

float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(wrapSampler, input.uv);

	return(cColor);
}

///////////////////////////////////////////////////////////////////////////////////////////
//
