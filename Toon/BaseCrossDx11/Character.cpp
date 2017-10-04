/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	TorusObject::TorusObject(){

	}

	TorusObject::~TorusObject() {

	}

	void TorusObject::OnCreate() {
		vector<VertexPositionNormalTexture> vertices;
		vector<uint16_t> indices;
		MeshUtill::CreateTorus(1, 0.5f, 100, vertices, indices);
		m_SphereMesh = MeshResource::CreateMeshResource(vertices, indices, false);

		m_Rotation.identity();

		wstring assetDir;
		App::GetApp()->GetAssetsDirectory(assetDir);
		wstring texStr = assetDir + L"Checker.png";
		m_TextureResource = TextureResource::CreateTextureResource(texStr);

		texStr = assetDir + L"ToonShadow.png";
		//texStr = assetDir + L"ToonShadowBrown.png";//影の色を変えてみたり
		m_ToonShadowTexture = TextureResource::CreateTextureResource(texStr);
	}

	void TorusObject::OnUpdate() {
		Quat QtSpan;
		QtSpan.rotation(0.02f, Vec3(1, 0, 1));
		m_Rotation *= QtSpan;
		m_Rotation.normalize();
	}

	void TorusObject::OnDraw() {
		Mat4x4 World;
		
		World.affineTransformation(
			Vec3(1, 1, 1),
			Vec3(0, 0, 0),
			m_Rotation,
			Vec3(0, 0, 0)
		);

		auto Dev = App::GetApp()->GetDeviceResources();
		auto pD3D11DeviceContext = Dev->GetD3DDeviceContext();
		auto RenderState = Dev->GetRenderState();

		pD3D11DeviceContext->VSSetShader(VSPNTStatic::GetPtr()->GetShader(), nullptr, 0);
		pD3D11DeviceContext->GSSetShader(GSOutline::GetPtr()->GetShader(), nullptr, 0);
		pD3D11DeviceContext->PSSetShader(PSPNTStatic::GetPtr()->GetShader(), nullptr, 0);

		pD3D11DeviceContext->IASetInputLayout(VSPNTStatic::GetPtr()->GetInputLayout());

		pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		pD3D11DeviceContext->OMSetDepthStencilState(RenderState->GetDepthDefault(), 0);

		ID3D11SamplerState* pSamplerWrap = RenderState->GetLinearWrap();

		UINT stride = sizeof(VertexPositionNormalTexture);
		UINT offset = 0;

		Mat4x4 View, Proj;
		Vec3 CameraPos = Vec3(0, 5, -2);
		View = XMMatrixLookAtLH(CameraPos, Vec3(0, 0, 0), Vec3(0, 1, 0));
		float w = static_cast<float>(App::GetApp()->GetGameWidth());
		float h = static_cast<float>(App::GetApp()->GetGameHeight());
		Proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, w / h, 1.0f, 100.0f);

		Vec4 LightDir = Vec4(-1, -2, -3, 1);

		View.transpose();
		Proj.transpose();

		PNStaticConstantBuffer sb;
		sb.View = View;
		sb.CameraLocation = CameraPos;
		sb.Projection = Proj;
		sb.LightDir = LightDir;

		sb.Diffuse = Col4(1, 1, 1, 1.0f);
		sb.Emissive = Col4(0.25f);
		sb.Outline = Col4(0, 0, 0, 1);

		World.transpose();
		sb.World = World;

		pD3D11DeviceContext->UpdateSubresource(CBPNStatic::GetPtr()->GetBuffer(), 0, nullptr, &sb, 0, 0);

		ID3D11Buffer* pConstantBuffer = CBPNStatic::GetPtr()->GetBuffer();

		pD3D11DeviceContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
		pD3D11DeviceContext->PSSetConstantBuffers(0, 1, &pConstantBuffer);
		pD3D11DeviceContext->IASetVertexBuffers(0, 1, m_SphereMesh->GetVertexBuffer().GetAddressOf(), &stride, &offset);
		pD3D11DeviceContext->IASetIndexBuffer(m_SphereMesh->GetIndexBuffer().Get(), DXGI_FORMAT_R16_UINT, 0);

		pD3D11DeviceContext->PSSetSamplers(0, 1, &pSamplerWrap);

		pD3D11DeviceContext->OMSetBlendState(RenderState->GetOpaque(), nullptr, 0xffffffff);
		pD3D11DeviceContext->RSSetState(RenderState->GetCullBack());

		//テクスチャ
		pD3D11DeviceContext->PSSetShaderResources(0, 1, m_TextureResource->GetShaderResourceView().GetAddressOf());
		pD3D11DeviceContext->PSSetShaderResources(1, 1, m_ToonShadowTexture->GetShaderResourceView().GetAddressOf());

		pD3D11DeviceContext->DrawIndexed(m_SphereMesh->GetNumIndicis(), 0, 0);

		Dev->InitializeStates();
	}

}
//end basecross
