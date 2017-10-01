/*!
@file ProjectShader.cpp
@brief プロジェクトで使用するシェーダー実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//PCDirect
	IMPLEMENT_DX11_VERTEX_SHADER(VSPCDirect, App::GetApp()->GetShadersPath() + L"VSPCDirect.cso")
	IMPLEMENT_DX11_PIXEL_SHADER(PSPCDirect, App::GetApp()->GetShadersPath() + L"PSPCDirect.cso")

	IMPLEMENT_DX11_CONSTANT_BUFFER(CBPNStatic)
	IMPLEMENT_DX11_VERTEX_SHADER(VSPNStatic,App::GetApp()->GetShadersPath() + L"VSPNStatic.cso")
	IMPLEMENT_DX11_PIXEL_SHADER(PSPNStatic,App::GetApp()->GetShadersPath() + L"PSPNStatic.cso")

}
//end basecross


