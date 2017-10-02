#include "INCStructs.hlsli"

[maxvertexcount(6)]
void main(triangle PSPNTInput input[3], inout TriangleStream<PSPNTInput> triStream) {
	PSPNTInput result = (PSPNTInput)0;

	int i;

	for (i = 0; i < 3; i++) {
		result.position = input[i].position;
		result.norm = input[i].norm;
		result.tex = input[i].tex;
		result.eyeDir = input[i].eyeDir;
		result.outlineCol = float4(1, 1, 1, 1);

		triStream.Append(result);
	}

	triStream.RestartStrip();

	for (i = 2; i >= 0; i--) {
		result.position = input[i].subpos;
		result.norm = input[i].norm;
		result.outlineCol = float4(0, 0, 0, 1);

		triStream.Append(result);
	}

	triStream.RestartStrip();
}