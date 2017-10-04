/*!
@file Scene.cpp
@brief シーンなど実体
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {

	//--------------------------------------------------------------------------------------
	///	ゲームシーン
	//--------------------------------------------------------------------------------------
	void Scene::OnCreate() {
		m_TorusObject = ObjectFactory::Create<TorusObject>();
	}

	void Scene::OnUpdate() {
		m_TorusObject->OnUpdate();
	}
	void Scene::OnDraw() {
		//描画デバイスの取得
		auto Dev = App::GetApp()->GetDeviceResources();
		Dev->ClearDefaultViews(Col4(0.8f, 0.8f, 0.8f, 1.0f));
		//デフォルト描画の開始
		Dev->StartDefaultDraw();
		m_TorusObject->OnDraw();
		//デフォルト描画の終了
		Dev->EndDefaultDraw();
	}

}
//end basecross
