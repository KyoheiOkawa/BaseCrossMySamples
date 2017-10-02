/*!
@file ProjectShader.h
@brief プロジェクトで使用するシェーダー
*/

#pragma once
#include "stdafx.h"

namespace basecross {
	struct PNStaticConstantBuffer
	{
		Mat4x4 World;
		Mat4x4 View;
		Mat4x4 Projection;
		Vec4 LightDir;
		Col4 Emissive;
		Col4 Diffuse;
		Vec4 CameraLocation;
		PNStaticConstantBuffer() {
			memset(this, 0, sizeof(PNStaticConstantBuffer));
		};
	};

	DECLARE_DX11_CONSTANT_BUFFER(CBPNStatic, PNStaticConstantBuffer)
	DECLARE_DX11_VERTEX_SHADER(VSPNTStatic,VertexPositionNormalTexture)
	DECLARE_DX11_PIXEL_SHADER(PSPNTStatic)

	DECLARE_DX11_VERTEX_SHADER(VSPCDirect, VertexPositionColor)
	DECLARE_DX11_PIXEL_SHADER(PSPCDirect)
}
//end basecross

