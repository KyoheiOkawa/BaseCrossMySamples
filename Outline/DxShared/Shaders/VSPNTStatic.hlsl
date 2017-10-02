#include "INCStructs.hlsli"

cbuffer SimpleConstantBuffer : register(b0)
{
	float4x4 World : packoffset(c0);
	float4x4 View : packoffset(c4);
	float4x4 Projection : packoffset(c8);
	float4 LightDir : packoffset(c12);
	float4 Emissive : packoffset(c13);
	float4 Diffuse : packoffset(c14);
	float4 CameraLocation : packoffset(c15);
	float4 Outline : packoffset(c16);
}

PSPNTInput main(VSPNTInput input)
{
	PSPNTInput result = (PSPNTInput)0;

	float4 pos = float4(input.position.xyz, 1.0f);
	pos = mul(pos, World);
	pos = mul(pos, View);
	pos = mul(pos, Projection);

	result.position = pos;

	result.norm = mul(input.norm, (float3x3)World);
	result.norm = normalize(result.norm);

	result.tex = input.tex;

	result.eyeDir = normalize(CameraLocation.xyz - input.position.xyz);

	float4 subpos = input.position + float4(input.norm,0) * 0.025f;
	subpos = mul(subpos, World);
	subpos = mul(subpos, View);
	subpos = mul(subpos, Projection);
	result.subpos = subpos;

	return result;
}