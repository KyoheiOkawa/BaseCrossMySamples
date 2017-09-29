#include "INCStructs.hlsli"

cbuffer SimpleConstantBuffer : register(b0)
{
	float4x4 World : packoffset(c0);
	float4x4 View : packoffset(c4);
	float4x4 Projection : packoffset(c8);
	float4 LightDir : packoffset(c12);
	float4 Emissive : packoffset(c13);
	float4 Diffuse : packoffset(c14);
}

PSPNInput main(VSPNInput input)
{
	PSPNInput result;

	float4 pos = float4(input.position.xyz, 1.0f);
	pos = mul(pos, World);
	pos = mul(pos, View);
	pos = mul(pos, Projection);

	result.position = pos;

	result.norm = mul(input.norm, (float3x3)World);
	result.norm = normalize(result.norm);

	return result;
}