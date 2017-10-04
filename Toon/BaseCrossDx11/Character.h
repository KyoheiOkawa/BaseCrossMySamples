/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	class TorusObject : public ObjectInterface, public ShapeInterface {
		shared_ptr<MeshResource> m_SphereMesh;
		Quat m_Rotation;
		std::shared_ptr<TextureResource> m_TextureResource;
		std::shared_ptr<TextureResource> m_ToonShadowTexture;
	public:
		TorusObject();
		virtual ~TorusObject();

		virtual void OnCreate()override;
		void OnUpdate()override;
		void OnDraw()override;
	};


}
//end basecross
