#define USE_INSTANCING 0
Texture2D gtxtTexture : register(t0);
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

cbuffer cbGameObjectInfo : register(b2)
{
	matrix		gmtxGameObject : packoffset(c0);
	uint		gnMaterial : packoffset(c4);
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

VS_TEXTURED_OUTPUT VSTextured(VS_TEXTURED_INPUT input)
{
	VS_TEXTURED_OUTPUT output;

	output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxGameObject), gmtxView), gmtxProjection);
	output.uv = input.uv;

	return(output);
}

float4 PSTextured(VS_TEXTURED_OUTPUT input) : SV_TARGET
{
	float4 cColor = gtxtTexture.Sample(wrapSampler, input.uv);

	return(cColor);
}

