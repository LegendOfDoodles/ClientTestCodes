#define USE_INSTANCING 0

Texture2D gtxtTexture : register(t0);
Texture2D gtxtNormal : register(t1);
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
    float3 gvCameraPosition : packoffset(c8);
};

#if USE_INSTANCING
struct INSTANCE_INFO
{
	matrix m_mtxGameObject;
};
StructuredBuffer<INSTANCE_INFO> gGameObjectInfos : register(t2);
#else
cbuffer cbGameObjectInfo : register(b2)
{
	matrix		gmtxGameObject : packoffset(c0);
};
#endif

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
    return (output);
}


//�ȼ� ���̴��� �����Ѵ�.
float4 PSDiffused(VS_OUTPUT input) : SV_TARGET
{
    return (input.color);
}

#include "./Light.hlsl"

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

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
//#define _WITH_VERTEX_LIGHTING

struct VS_LIGHTING_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
};

struct VS_LIGHTING_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
//	nointerpolation float3 normalW : NORMAL;
#ifdef _WITH_VERTEX_LIGHTING
    float4 color : COLOR;
#endif
};

#if USE_INSTANCING
VS_LIGHTING_OUTPUT VSLightingInstancing(VS_LIGHTING_INPUT input, uint nInstanceID : SV_InstanceID)
{
    VS_LIGHTING_OUTPUT output;

    output.normalW = mul(input.normal, (float3x3) gGameObjectInfos[nInstanceID].m_mtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gGameObjectInfos[nInstanceID].m_mtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
#ifdef _WITH_VERTEX_LIGHTING
    output.normalW = normalize(output.normalW);
    output.color = Lighting(output.positionW, output.normalW);
#endif
    return (output);
}
#else
VS_LIGHTING_OUTPUT VSLighting(VS_LIGHTING_INPUT input)
{
    VS_LIGHTING_OUTPUT output;

    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
#ifdef _WITH_VERTEX_LIGHTING
    output.normalW = normalize(output.normalW);
    output.color = Lighting(output.positionW, output.normalW);
#endif
    return (output);
}
#endif

float4 PSLighting(VS_LIGHTING_OUTPUT input) : SV_TARGET
{
#ifdef _WITH_VERTEX_LIGHTING
    return (input.color);
#else
	input.normalW = normalize(input.normalW);
	float4 color = Lighting(input.positionW, input.normalW);
	return(color);
#endif
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
struct VS_TEXTURED_LIGHTING_INPUT
{
    float3 position : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangent : TANGENT;
};

struct VS_TEXTURED_LIGHTING_OUTPUT
{
    float4 position : SV_POSITION;
    float3 positionW : POSITION;
    float3 normalW : NORMAL;
	//	nointerpolation float3 normalW : NORMAL;
    float2 uv : TEXCOORD;
    float3 tangentW : TANGENT;
#ifdef _WITH_VERTEX_LIGHTING
    float4 color : COLOR;
#endif
};

#if USE_INSTANCING
VS_TEXTURED_LIGHTING_OUTPUT VSTexturedLightingInstancing(VS_TEXTURED_LIGHTING_INPUT input, uint nInstanceID : SV_InstanceID)
{
    VS_TEXTURED_LIGHTING_OUTPUT output;

    output.normalW = mul(input.normal, (float3x3) gGameObjectInfos[nInstanceID].m_mtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gGameObjectInfos[nInstanceID].m_mtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;
    output.tangentW = mul(input.tangent, (float3x3) gGameObjectInfos[nInstanceID].m_mtxGameObject);
#ifdef _WITH_VERTEX_LIGHTING
    output.normalW = normalize(output.normalW);
    output.color = Lighting(output.positionW, output.normalW);
#endif
    return (output);
}
#else
VS_TEXTURED_LIGHTING_OUTPUT VSTexturedLighting(VS_TEXTURED_LIGHTING_INPUT input)
{
    VS_TEXTURED_LIGHTING_OUTPUT output;

    output.normalW = mul(input.normal, (float3x3) gmtxGameObject);
    output.positionW = (float3) mul(float4(input.position, 1.0f), gmtxGameObject);
    output.position = mul(mul(float4(output.positionW, 1.0f), gmtxView), gmtxProjection);
    output.uv = input.uv;
	output.tangentW = mul(input.tangent, (float3x3)gmtxGameObject);
#ifdef _WITH_VERTEX_LIGHTING
    output.normalW = normalize(output.normalW);
    output.color = Lighting(output.positionW, output.normalW);
#endif
    return (output);
}
#endif

float4 PSTexturedLighting(VS_TEXTURED_LIGHTING_OUTPUT input) : SV_TARGET
{
    float4 cColor = gtxtTexture.Sample(wrapSampler, input.uv);
#ifdef _WITH_VERTEX_LIGHTING
    float4 cIllumination = input.color;
#else
    float3 N = normalize(input.normalW);
    if (input.tangentW.x != 0 || input.tangentW.y != 0 || input.tangentW.z != 0)
    {
        float3 T = normalize(input.tangentW - dot(input.tangentW, N) * N);
        float3 B = cross(N, T); // �븻�� ź��Ʈ�� �����ؼ� ���� ź��Ʈ(���� �븻)����
        float3x3 TBN = float3x3(T, B, N); // �̸� �������� TBN��� ����
        float3 normal = gtxtNormal.Sample(wrapSampler, input.uv); // �븻 �ʿ��� �ش��ϴ� uv�� �ش��ϴ� �븻 �б�
        normal = 2.0f * normal - 1.0f; // �븻�� -1���� 1������ ������ ��ȯ
        N = mul(normal, TBN); // �븻�� TBN��ķ� ��ȯ
    }

    float4 cIllumination = Lighting(input.positionW, N);
#endif
    return (lerp(cColor, cIllumination, 0.5f));
}
