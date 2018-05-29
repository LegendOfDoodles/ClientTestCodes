//게임 객체의 정보를 위한 상수 버퍼를 선언한다.
cbuffer cbGameObjectInfo : register(b0)
{
	matrix gmtxWorld : packoffset(c0);
};


//카메라의 정보를 위한 상수 버퍼를 선언한다.
cbuffer cbCameraInfo : register(b1)
{
	matrix gmtxView : packoffset(c0);
	matrix gmtxProjection : packoffset(c4);
};


//정점 셰이더의 입력을 위한 구조체를 선언한다.
struct VS_INPUT
{
	float3 position : POSITION;
	float4 color : COLOR;
};


//정점 셰이더의 출력(픽셀 셰이더의 입력)을 위한 구조체를 선언한다.
struct VS_OUTPUT
{
    float4 position : POSITION;
	float4 color : COLOR;
};


//정점 셰이더를 정의한다.
VS_OUTPUT VSDiffused(VS_INPUT input)
{
	VS_OUTPUT output;
    output.position = mul(mul(mul(float4(input.position, 1.0f), gmtxWorld), gmtxView), gmtxProjection);
    //output.position = float4(input.position, 1);
	output.color = input.color;
	return(output);
}

struct PatchTess
{
    float EdgeTess[4] : SV_TessFactor;
    float InsideTess[2] : SV_InsideTessFactor;
};

PatchTess ConstantHSDiffused(InputPatch<VS_OUTPUT, 4> patch, uint patchID : SV_PrimitiveID)
{
    PatchTess pt;

    pt.EdgeTess[0] = 64;
    pt.EdgeTess[1] = 64;
    pt.EdgeTess[2] = 64;
    pt.EdgeTess[3] = 64;

    pt.InsideTess[0] = 64;
    pt.InsideTess[1] = 64;

    return pt;
}

struct HullOut
{
    float4 position : POSITION;
    float4 color : COLOR;
};

[domain("quad")]
[partitioning("integer")]
[outputtopology("triangle_ccw")]
[outputcontrolpoints(4)]
[patchconstantfunc("ConstantHSDiffused")]
[maxtessfactor(64.0f)]
HullOut HSDiffused(InputPatch<VS_OUTPUT, 4> P, uint i : SV_OutputControlPointID, uint patchId : SV_PrimitiveID)
{
    HullOut hout;
    hout.position = P[i].position;
    hout.color = P[i].color;

    return hout;
}

struct DomainOut
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

[domain("quad")]
DomainOut DSDiffused(PatchTess patchTess, float2 uv : SV_DomainLocation, const OutputPatch<HullOut, 4> quad)
{
    DomainOut dout;

    float4 v1 = lerp(quad[0].position, quad[1].position, 1 - uv.y);
    float4 v2 = lerp(quad[2].position, quad[3].position, 1 - uv.y);
    float4 p = lerp(v1, v2, 1 - uv.x);

    //p.y += 0.3f * (p.z * sin(p.x) + p.x * cos(p.z));

    float4 v3 = lerp(quad[0].color, quad[1].color, 1 - uv.y);
    float4 v4 = lerp(quad[2].color, quad[3].color, 1 - uv.y);
    float4 c = lerp(v3, v4, 1 - uv.x);

    dout.color = c;
    dout.position = p;
    //dout.position = mul(mul(mul(float4(p), gmtxWorld), gmtxView), gmtxProjection);

    return dout;
}

//픽셀 셰이더를 정의한다.
float4 PSDiffused(DomainOut input) : SV_TARGET
{
     return (input.color);
}