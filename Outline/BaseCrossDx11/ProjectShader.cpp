/*!
@file ProjectShader.cpp
@brief �v���W�F�N�g�Ŏg�p����V�F�[�_�[����
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//PCDirect
	IMPLEMENT_DX11_VERTEX_SHADER(VSPCDirect, App::GetApp()->GetShadersPath() + L"VSPCDirect.cso")
		IMPLEMENT_DX11_PIXEL_SHADER(PSPCDirect, App::GetApp()->GetShadersPath() + L"PSPCDirect.cso")

		IMPLEMENT_DX11_CONSTANT_BUFFER(CBPNStatic)
		IMPLEMENT_DX11_VERTEX_SHADER(VSPNTStatic, App::GetApp()->GetShadersPath() + L"VSPNTStatic.cso")
		IMPLEMENT_DX11_PIXEL_SHADER(PSPNTStatic, App::GetApp()->GetShadersPath() + L"PSPNTStatic.cso")
		IMPLEMENT_DX11_GEOMETRY_SHADER(GSOutline, App::GetApp()->GetShadersPath() + L"GSOutline.cso");

}
//end basecross


