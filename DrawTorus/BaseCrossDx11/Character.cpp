/*!
@file Character.cpp
@brief キャラクターなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	TorusObject::TorusObject() {

	}

	TorusObject::~TorusObject() {

	}

	void TorusObject::OnCreate() {
		vector<VertexPositionNormalTexture> vertices;
		vector<uint16_t> indices;
		MeshUtill::CreateTorus(1, 0.5f, 100, vertices, indices);
		m_SphereMesh = MeshResource::CreateMeshResource(vertices, indices, false);

	}

	void TorusObject::OnUpdate() {

	}

	void TorusObject::OnDraw() {
		Mat4x4 World;
		
		World.affineTransformation(
			Vec3(1, 1, 1),
			Vec3(0, 0, 0),
			Quat(0,0,0,1),
			Vec3(0, 0, 0)
		);

		auto Dev = App::GetApp()->GetDeviceResources();
		auto pD3D11DeviceContext = Dev->GetD3DDeviceContext();
		auto RenderState = Dev->GetRenderState();

		pD3D11DeviceContext->VSSetShader(VSPNStatic::GetPtr()->GetShader(), nullptr, 0);
		pD3D11DeviceContext->PSSetShader(PSPNStatic::GetPtr()->GetShader(), nullptr, 0);

		pD3D11DeviceContext->IASetInputLayout(VSPNStatic::GetPtr()->GetInputLayout());

		pD3D11DeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		pD3D11DeviceContext->OMSetDepthStencilState(RenderState->GetDepthDefault(), 0);

		ID3D11SamplerState* pSamplerWrap = RenderState->GetLinearWrap();

		UINT stride = sizeof(VertexPositionNormalTexture);
		UINT offset = 0;

		Mat4x4 View, Proj;
		View = XMMatrixLookAtLH(Vec3(0, 5, -2), Vec3(0, 0, 0), Vec3(0, 1, 0));
		float w = static_cast<float>(App::GetApp()->GetGameWidth());
		float h = static_cast<float>(App::GetApp()->GetGameHeight());
		Proj = XMMatrixPerspectiveFovLH(XM_PIDIV4, w / h, 1.0f, 100.0f);

		Vec4 LightDir = Vec4(-1, -2, -3, 1);

		View.transpose();
		Proj.transpose();

		PNStaticConstantBuffer sb;
		sb.View = View;
		sb.Projection = Proj;
		sb.LightDir = LightDir;

		sb.Diffuse = Col4(1.0f, 1.0f, 1.0f, 1.0f);
		sb.Emissive = Col4(0.4f, 0.4f, 0.4f, 0);

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

		pD3D11DeviceContext->DrawIndexed(m_SphereMesh->GetNumIndicis(), 0, 0);

		Dev->InitializeStates();
	}

}
//end basecross
