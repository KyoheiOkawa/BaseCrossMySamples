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
};

SamplerState g_sampler : register(s0);

float4 main(PSPNInput input) : SV_TARGET
{
	float3 lightdir = normalize(LightDir.xyz);

	float3 N1 = normalize(input.norm);
	float4 Light = saturate(dot(N1, -lightdir) * Diffuse) + Emissive;

	float3 R = normalize(LightDir - input.norm * 2 * dot(LightDir, input.norm));
	float4 spec = pow(max(0, dot(R, input.eyeDir)), 50.0f);
	Light += spec;

	Light.a = Diffuse.a;
	return saturate(Light);
}