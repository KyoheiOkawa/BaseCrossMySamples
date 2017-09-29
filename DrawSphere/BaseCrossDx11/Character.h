/*!
@file Character.h
@brief キャラクターなど
*/

#pragma once
#include "stdafx.h"

namespace basecross {

	class SphereObject : public ObjectInterface, public ShapeInterface {
		shared_ptr<MeshResource> m_SphereMesh;
	public:
		SphereObject();
		virtual ~SphereObject();

		virtual void OnCreate()override;
		void OnUpdate()override;
		void OnDraw()override;
	};


}
//end basecross
