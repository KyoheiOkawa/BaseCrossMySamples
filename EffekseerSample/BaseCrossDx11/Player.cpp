/*!
@file Player.cpp
@brief �v���C���[����
*/

#include "stdafx.h"
#include "Project.h"

namespace basecross {
	//--------------------------------------------------------------------------------------
	///	���̂̃v���C���[����
	//--------------------------------------------------------------------------------------
	Player::Player(const shared_ptr<Stage>& StagePtr,
		const wstring& TextureResName, bool Trace, const Vec3& Pos) :
		GameObject(StagePtr),
		m_TextureResName(TextureResName),
		m_Trace(Trace),
		m_BaseY(0.25f / 2.0f),
		m_Posision(Pos),
		m_JumpLock(false)
	{}
	Player::~Player()
	{
		ES_SAFE_RELEASE(m_Effect);
		m_EffekseerManager->Destroy();
		m_EffekseerRenderer->Destroy();
	}

	Vec3 Player::GetMoveVector(){
		auto& body = GetStage<GameStage>()->GetOwnRigidbody(GetThis<GameObject>());
		Vec3 Angle(0, 0, 0);
		auto PtrGameStage = GetStage<GameStage>();
		Vec3 CameraEye, CameraAt;
		PtrGameStage->GetCamera().GetCameraEyeAt(CameraEye, CameraAt);

		//�R���g���[���̎擾
		auto CntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
		if (CntlVec[0].bConnected) {
			if (CntlVec[0].fThumbLX != 0 || CntlVec[0].fThumbLY != 0) {
				float MoveLength = 0;	//���������̃X�s�[�h
										//�i�s�����̌������v�Z
				Vec3 Front = body.m_Pos - CameraEye;
				Front.y = 0;
				Front.normalize();
				//�i�s������������̊p�x���Z�o
				float FrontAngle = atan2(Front.z, Front.x);
				//�R���g���[���̌����v�Z
				float MoveX = CntlVec[0].fThumbLX;
				float MoveZ = CntlVec[0].fThumbLY;
				Vec2 MoveVec(MoveX, MoveZ);
				float MoveSize = length(MoveVec);
				//�R���g���[���̌�������p�x���v�Z
				float CntlAngle = atan2(-MoveX, MoveZ);
				//�g�[�^���̊p�x���Z�o
				float TotalAngle = FrontAngle + CntlAngle;
				//�p�x����x�N�g�����쐬
				Angle = Vec3(cos(TotalAngle), 0, sin(TotalAngle));
				//���K������
				Angle.normalize();
				//�ړ��T�C�Y��ݒ�B
				Angle *= MoveSize;
				//Y���͕ω������Ȃ�
				Angle.y = 0;
			}
		}
		return Angle;
	}

	Vec3 Player::GetPosition() {
		auto& body =  GetStage<GameStage>()->GetOwnRigidbody(GetThis<GameObject>());
		return body.m_Pos;
	}

	Effekseer::Matrix44 Player::ConvertToEffekseerMat(Mat4x4 m)
	{
		Effekseer::Matrix44 ret;
		ret.Values[0][0] = m._11;
		ret.Values[0][1] = m._12;
		ret.Values[0][2] = m._13;
		ret.Values[0][3] = m._14;

		ret.Values[1][0] = m._21;
		ret.Values[1][1] = m._22;
		ret.Values[1][2] = m._23;
		ret.Values[1][3] = m._24;

		ret.Values[2][0] = m._31;
		ret.Values[2][1] = m._32;
		ret.Values[2][2] = m._33;
		ret.Values[2][3] = m._34;

		ret.Values[3][0] = m._41;
		ret.Values[3][1] = m._42;
		ret.Values[3][2] = m._43;
		ret.Values[3][3] = m._44;

		return ret;
	}

	void Player::OnCreate() {
		vector<VertexPositionNormalTexture> vertices;
		vector<uint16_t> indices;
		MeshUtill::CreateSphere(1.0f, 18, vertices, indices);
		//���b�V���̍쐬�i�ύX�ł��Ȃ��j
		m_SphereMesh = MeshResource::CreateMeshResource(vertices, indices, false);
		//�^�O�̒ǉ�
		AddTag(L"Player");
		//Rigidbody�̏�����
		auto PtrGameStage = GetStage<GameStage>();
		Rigidbody body;
		body.m_Owner = GetThis<GameObject>();
		body.m_Mass = 1.0f;
		body.m_Scale = Vec3(0.25f);
		body.m_Quat = Quat();
		body.m_Pos = m_Posision;
		body.m_CollType = CollType::typeSPHERE;
		body.SetToBefore();

		PtrGameStage->AddRigidbody(body);

		auto deviceResource = App::GetApp()->GetDeviceResources();
		auto device = deviceResource->GetD3DDevice();
		auto context = deviceResource->GetD3DDeviceContext();

		m_EffekseerRenderer = EffekseerRendererDX11::Renderer::Create(device,context,100);
		m_EffekseerManager = Effekseer::Manager::Create(100);

		m_EffekseerManager->SetSpriteRenderer(m_EffekseerRenderer->CreateSpriteRenderer());
		m_EffekseerManager->SetRibbonRenderer(m_EffekseerRenderer->CreateRibbonRenderer());
		m_EffekseerManager->SetRingRenderer(m_EffekseerRenderer->CreateRingRenderer());

		m_EffekseerManager->SetTextureLoader(m_EffekseerRenderer->CreateTextureLoader());
		m_EffekseerManager->SetCoordinateSystem(Effekseer::CoordinateSystem::LH);

		wstring assetsDir;
		App::GetApp()->GetAssetsDirectory(assetsDir);
		wstring effectPath = assetsDir + L"EffecseerSimple.efk";

		m_Effect = Effekseer::Effect::Create(m_EffekseerManager->GetSetting(), (EFK_CHAR *)effectPath.c_str(),0.25f, (EFK_CHAR *)assetsDir.c_str());
		m_EffekseerHandle = m_EffekseerManager->Play(m_Effect, m_Posision.x, m_Posision.y, m_Posision.z);
	}
	void Player::OnUpdate() {
		auto& body = GetStage<GameStage>()->GetOwnRigidbody(GetThis<GameObject>());
		//�O��̃^�[������̌o�ߎ��Ԃ����߂�
		float ElapsedTime = App::GetApp()->GetElapsedTime();
		//�R���g���[���̎擾
		auto CntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
		if (CntlVec[0].bConnected) {
			if (!m_JumpLock) {
				//A�{�^��
				if (CntlVec[0].wPressedButtons & XINPUT_GAMEPAD_A) {
					body.m_BeforePos.y += 0.01f;
					body.m_Pos.y += 0.01f;
					body.m_Velocity += Vec3(0, 4.0f, 0);
					m_JumpLock = true;
				}
			}
			Vec3 Direction = GetMoveVector();
			if (length(Direction) < 0.1f) {
				body.m_Velocity.x *= 0.9f;
				body.m_Velocity.z *= 0.9f;
			}
			else {
				//�t�H�[�X�ŕύX����ꍇ�͈ȉ��̂悤�ɋL�q
				//body.m_Force += Direction * 10.0f;
				//���x�ŕύX����ꍇ�͈ȉ��̂悤�ɋL�q
				body.m_Velocity += Direction * 0.5f;
				Vec2 TempVelo(body.m_Velocity.x, body.m_Velocity.z);
				TempVelo = XMVector2ClampLength(TempVelo, 0, 5.0f);
				body.m_Velocity.x = TempVelo.x;
				body.m_Velocity.z = TempVelo.y;
			}
		}
		body.m_Force += body.m_Gravity * body.m_Mass;

		auto stage = GetStage();
		auto stagePtr = dynamic_pointer_cast<GameStage>(stage);
		auto camera = stagePtr->GetCamera();
		Mat4x4 view,proj;
		camera.GetViewProjMatrix(view,proj);

		Effekseer::Matrix44 viewEf, projEf;
		viewEf = ConvertToEffekseerMat(view);
		projEf = ConvertToEffekseerMat(proj);

		m_EffekseerRenderer->SetProjectionMatrix(projEf);
		m_EffekseerRenderer->SetCameraMatrix(viewEf);
		m_EffekseerManager->Update();

		bool isExist = m_EffekseerManager->Exists(m_EffekseerHandle);
		if (!isExist) {
			m_EffekseerHandle = m_EffekseerManager->Play(m_Effect, body.m_Pos.x, body.m_Pos.y, body.m_Pos.z);
		}
		m_EffekseerManager->SetLocation(m_EffekseerHandle, body.m_Pos.x, body.m_Pos.y, body.m_Pos.z);
	}

	void Player::OnUpdate2() {
		auto& body = GetStage<GameStage>()->GetOwnRigidbody(GetThis<GameObject>());
		if (body.m_Pos.y <= m_BaseY) {
			body.m_Pos.y = m_BaseY;
			body.m_Velocity.y = 0;
			m_JumpLock = false;
		}
		auto& StateVec = GetStage<GameStage>()->GetCollisionStateVec();
		for (auto& v : StateVec) {
			if (v.m_Src == &body) {
				Vec3 Normal = v.m_SrcHitNormal;
				Normal.normalize();
				Vec4 v = (Vec4)XMVector3AngleBetweenNormals(Vec3(0, 1, 0), Normal);
				if (v.x < 0.1f) {
					m_JumpLock = false;
					break;
				}
			}
			if (v.m_Dest == &body) {
				Vec3 Normal = v.m_SrcHitNormal;
				Normal.normalize();
				Vec4 v = (Vec4)XMVector3AngleBetweenNormals(Vec3(0, 1, 0), Normal);
				if (v.x < 0.1f) {
					m_JumpLock = false;
					break;
				}
			}
		}
		auto LenVec = body.m_Pos - body.m_BeforePos;
		LenVec.y = 0;
		auto Len = LenVec.length();
		if (Len > 0) {
			Vec3 Cross = cross(Vec3(0, 1, 0), LenVec);
			Quat Span(Cross, Len / 0.5f);
			body.m_Quat *= Span;
		}
	}

	void Player::OnDraw() {
		auto& body = GetStage<GameStage>()->GetOwnRigidbody(GetThis<GameObject>());
		auto TexPtr = App::GetApp()->GetResource<TextureResource>(m_TextureResName);
		auto PtrGameStage = GetStage<GameStage>();
		//���[���h�s��̌���
		Mat4x4 World;
		World.affineTransformation(body.m_Scale, Vec3(0, 0, 0),
			body.m_Quat, body.m_Pos);
		auto shptr = PtrGameStage->FindTagGameObject<PNTDrawObject>(L"PNTDrawObject");
		shptr->AddDrawMesh(
			m_SphereMesh,
			TexPtr,
			World,
			true
		);

		m_EffekseerRenderer->BeginRendering();
		m_EffekseerManager->Draw();
		m_EffekseerRenderer->EndRendering();
	}


	//--------------------------------------------------------------------------------------
	///	�J�v�Z���̃v���C���[����
	//--------------------------------------------------------------------------------------
	CapsulePlayer::CapsulePlayer(const shared_ptr<Stage>& StagePtr,
		const wstring& TextureResName, bool Trace, const Vec3& Pos) :
		GameObject(StagePtr),
		m_TextureResName(TextureResName),
		m_Trace(Trace),
		m_BaseY(0.25f),
		m_Posision(Pos),
		m_JumpLock(false)
	{}
	CapsulePlayer::~CapsulePlayer() {}

	Vec3 CapsulePlayer::GetMoveVector() {
		auto& body = GetStage<GameStage>()->GetOwnRigidbody(GetThis<GameObject>());
		Vec3 Angle(0, 0, 0);
		auto PtrGameStage = GetStage<GameStage>();
		Vec3 CameraEye, CameraAt;
		PtrGameStage->GetCamera().GetCameraEyeAt(CameraEye, CameraAt);

		//�R���g���[���̎擾
		auto CntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
		if (CntlVec[0].bConnected) {
			if (CntlVec[0].fThumbLX != 0 || CntlVec[0].fThumbLY != 0) {
				float MoveLength = 0;	//���������̃X�s�[�h
										//�i�s�����̌������v�Z
				Vec3 Front = body.m_Pos - CameraEye;
				Front.y = 0;
				Front.normalize();
				//�i�s������������̊p�x���Z�o
				float FrontAngle = atan2(Front.z, Front.x);
				//�R���g���[���̌����v�Z
				float MoveX = CntlVec[0].fThumbLX;
				float MoveZ = CntlVec[0].fThumbLY;
				Vec2 MoveVec(MoveX, MoveZ);
				float MoveSize = length(MoveVec);
				//�R���g���[���̌�������p�x���v�Z
				float CntlAngle = atan2(-MoveX, MoveZ);
				//�g�[�^���̊p�x���Z�o
				float TotalAngle = FrontAngle + CntlAngle;
				//�p�x����x�N�g�����쐬
				Angle = Vec3(cos(TotalAngle), 0, sin(TotalAngle));
				//���K������
				Angle.normalize();
				//�ړ��T�C�Y��ݒ�B
				Angle *= MoveSize;
				//Y���͕ω������Ȃ�
				Angle.y = 0;
			}
		}
		return Angle;
	}

	Vec3 CapsulePlayer::GetPosition() {
		auto& body = GetStage<GameStage>()->GetOwnRigidbody(GetThis<GameObject>());
		return body.m_Pos;
	}

	void CapsulePlayer::OnCreate() {
		vector<VertexPositionNormalTexture> vertices;
		vector<uint16_t> indices;
		MeshUtill::CreateCapsule(1.0f, Vec3(0, -0.5f, 0), Vec3(0, 0.5f, 0), 18, vertices, indices);
		//���b�V���̍쐬�i�ύX�ł��Ȃ��j
		m_CapsuleMesh = MeshResource::CreateMeshResource(vertices, indices, false);
		//�^�O�̒ǉ�
		AddTag(L"Player");
		//Rigidbody�̏�����
		auto PtrGameStage = GetStage<GameStage>();
		Rigidbody body;
		body.m_Owner = GetThis<GameObject>();
		body.m_Mass = 1.0f;
		body.m_Scale = Vec3(0.25f);
		body.m_Quat = Quat();
		body.m_Pos = m_Posision;
		body.m_CollType = CollType::typeCAPSULE;
		body.SetToBefore();

		PtrGameStage->AddRigidbody(body);

	}
	void CapsulePlayer::OnUpdate() {
		auto& body = GetStage<GameStage>()->GetOwnRigidbody(GetThis<GameObject>());
		//�O��̃^�[������̌o�ߎ��Ԃ����߂�
		float ElapsedTime = App::GetApp()->GetElapsedTime();
		//�R���g���[���̎擾
		auto CntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
		if (CntlVec[0].bConnected) {
			if (!m_JumpLock) {
				//A�{�^��
				if (CntlVec[0].wPressedButtons & XINPUT_GAMEPAD_A) {
					body.m_BeforePos.y += 0.01f;
					body.m_Pos.y += 0.01f;
					body.m_Velocity += Vec3(0, 4.0f, 0);
					m_JumpLock = true;
				}
			}
			Vec3 Direction = GetMoveVector();
			if (length(Direction) < 0.1f) {
				body.m_Velocity.x *= 0.9f;
				body.m_Velocity.z *= 0.9f;
			}
			else {
				//�t�H�[�X�ŕύX����ꍇ�͈ȉ��̂悤�ɋL�q
				//body.m_Force += Direction * 10.0f;
				//���x�ŕύX����ꍇ�͈ȉ��̂悤�ɋL�q
				body.m_Velocity += Direction * 0.5f;
				Vec2 TempVelo(body.m_Velocity.x, body.m_Velocity.z);
				TempVelo = XMVector2ClampLength(TempVelo, 0, 5.0f);
				body.m_Velocity.x = TempVelo.x;
				body.m_Velocity.z = TempVelo.y;
			}
		}
		body.m_Force += body.m_Gravity * body.m_Mass;
	}

	void CapsulePlayer::OnUpdate2() {
		auto& body = GetStage<GameStage>()->GetOwnRigidbody(GetThis<GameObject>());
		if (body.m_Pos.y <= m_BaseY) {
			body.m_Pos.y = m_BaseY;
			body.m_Velocity.y = 0;
			m_JumpLock = false;
		}
		auto& StateVec = GetStage<GameStage>()->GetCollisionStateVec();
		for (auto& v : StateVec) {
			if (v.m_Src == &body) {
				Vec3 Normal = v.m_SrcHitNormal;
				Normal.normalize();
				Vec4 v = (Vec4)XMVector3AngleBetweenNormals(Vec3(0, 1, 0), Normal);
				if (v.x < 0.1f) {
					m_JumpLock = false;
					break;
				}
			}
			if (v.m_Dest == &body) {
				Vec3 Normal = v.m_SrcHitNormal;
				Normal.normalize();
				Vec4 v = (Vec4)XMVector3AngleBetweenNormals(Vec3(0, 1, 0), Normal);
				if (v.x < 0.1f) {
					m_JumpLock = false;
					break;
				}
			}
		}
		//��]�̍X�V
		//Velocity�̒l�ŁA��]��ύX����
		Vec3 Temp = body.m_Velocity;
		Temp.normalize();
		float ToAngle = atan2(Temp.x, Temp.z);
		Quat Qt;
		Qt.rotationRollPitchYawFromVector(Vec3(0, ToAngle, 0));
		Qt.normalize();
		//���݂ƖڕW����
		body.m_Quat = XMQuaternionSlerp(body.m_Quat, Qt, 0.1f);
	}

	void CapsulePlayer::OnDraw() {
		auto& body = GetStage<GameStage>()->GetOwnRigidbody(GetThis<GameObject>());
		auto TexPtr = App::GetApp()->GetResource<TextureResource>(m_TextureResName);
		auto PtrGameStage = GetStage<GameStage>();
		//���[���h�s��̌���
		Mat4x4 World;
		World.affineTransformation(body.m_Scale, Vec3(0, 0, 0),
			body.m_Quat, body.m_Pos);
		auto shptr = PtrGameStage->FindTagGameObject<PNTDrawObject>(L"PNTDrawObject");
		shptr->AddDrawMesh(
			m_CapsuleMesh,
			TexPtr,
			World,
			true
		);
	}

	//--------------------------------------------------------------------------------------
	///	OBB�̃v���C���[����
	//--------------------------------------------------------------------------------------
	ObbPlayer::ObbPlayer(const shared_ptr<Stage>& StagePtr,
		const wstring& TextureResName, bool Trace, const Vec3& Pos) :
		GameObject(StagePtr),
		m_TextureResName(TextureResName),
		m_Trace(Trace),
		m_BaseY(0.125f),
		m_Posision(Pos),
		m_JumpLock(false)
	{}
	ObbPlayer::~ObbPlayer() {}

	Vec3 ObbPlayer::GetMoveVector() {
		auto& body = GetStage<GameStage>()->GetOwnRigidbody(GetThis<GameObject>());
		Vec3 Angle(0, 0, 0);
		auto PtrGameStage = GetStage<GameStage>();
		Vec3 CameraEye, CameraAt;
		PtrGameStage->GetCamera().GetCameraEyeAt(CameraEye, CameraAt);

		//�R���g���[���̎擾
		auto CntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
		if (CntlVec[0].bConnected) {
			if (CntlVec[0].fThumbLX != 0 || CntlVec[0].fThumbLY != 0) {
				float MoveLength = 0;	//���������̃X�s�[�h
										//�i�s�����̌������v�Z
				Vec3 Front = body.m_Pos - CameraEye;
				Front.y = 0;
				Front.normalize();
				//�i�s������������̊p�x���Z�o
				float FrontAngle = atan2(Front.z, Front.x);
				//�R���g���[���̌����v�Z
				float MoveX = CntlVec[0].fThumbLX;
				float MoveZ = CntlVec[0].fThumbLY;
				Vec2 MoveVec(MoveX, MoveZ);
				float MoveSize = length(MoveVec);
				//�R���g���[���̌�������p�x���v�Z
				float CntlAngle = atan2(-MoveX, MoveZ);
				//�g�[�^���̊p�x���Z�o
				float TotalAngle = FrontAngle + CntlAngle;
				//�p�x����x�N�g�����쐬
				Angle = Vec3(cos(TotalAngle), 0, sin(TotalAngle));
				//���K������
				Angle.normalize();
				//�ړ��T�C�Y��ݒ�B
				Angle *= MoveSize;
				//Y���͕ω������Ȃ�
				Angle.y = 0;
			}
		}
		return Angle;
	}

	Vec3 ObbPlayer::GetPosition() {
		auto& body = GetStage<GameStage>()->GetOwnRigidbody(GetThis<GameObject>());
		return body.m_Pos;
	}

	void ObbPlayer::OnCreate() {
		vector<VertexPositionNormalTexture> vertices;
		vector<uint16_t> indices;
		MeshUtill::CreateCube(1.0f, vertices, indices);
		//���b�V���̍쐬�i�ύX�ł��Ȃ��j
		m_ObbMesh = MeshResource::CreateMeshResource(vertices, indices, false);
		//�^�O�̒ǉ�
		AddTag(L"Player");
		//Rigidbody�̏�����
		auto PtrGameStage = GetStage<GameStage>();
		Rigidbody body;
		body.m_Owner = GetThis<GameObject>();
		body.m_Mass = 1.0f;
		body.m_Scale = Vec3(0.125f,0.25f,0.25f);
		body.m_Quat = Quat();
		body.m_Pos = m_Posision;
		body.m_CollType = CollType::typeOBB;
		body.SetToBefore();

		PtrGameStage->AddRigidbody(body);

	}
	void ObbPlayer::OnUpdate() {
		auto& body = GetStage<GameStage>()->GetOwnRigidbody(GetThis<GameObject>());
		//�O��̃^�[������̌o�ߎ��Ԃ����߂�
		float ElapsedTime = App::GetApp()->GetElapsedTime();
		//�R���g���[���̎擾
		auto CntlVec = App::GetApp()->GetInputDevice().GetControlerVec();
		if (CntlVec[0].bConnected) {
			if (!m_JumpLock) {
				//A�{�^��
				if (CntlVec[0].wPressedButtons & XINPUT_GAMEPAD_A) {
					body.m_BeforePos.y += 0.01f;
					body.m_Pos.y += 0.01f;
					body.m_Velocity += Vec3(0, 4.0f, 0);
					m_JumpLock = true;
				}
			}
			Vec3 Direction = GetMoveVector();
			if (length(Direction) < 0.1f) {
				body.m_Velocity.x *= 0.9f;
				body.m_Velocity.z *= 0.9f;
			}
			else {
				//�t�H�[�X�ŕύX����ꍇ�͈ȉ��̂悤�ɋL�q
				//body.m_Force += Direction * 10.0f;
				//���x�ŕύX����ꍇ�͈ȉ��̂悤�ɋL�q
				body.m_Velocity += Direction * 0.5f;
				Vec2 TempVelo(body.m_Velocity.x, body.m_Velocity.z);
				TempVelo = XMVector2ClampLength(TempVelo, 0, 5.0f);
				body.m_Velocity.x = TempVelo.x;
				body.m_Velocity.z = TempVelo.y;
			}
		}
		body.m_Force += body.m_Gravity * body.m_Mass;
	}

	void ObbPlayer::OnUpdate2() {
		auto& body = GetStage<GameStage>()->GetOwnRigidbody(GetThis<GameObject>());
		if (body.m_Pos.y <= m_BaseY) {
			body.m_Pos.y = m_BaseY;
			body.m_Velocity.y = 0;
			m_JumpLock = false;
		}
		auto& StateVec = GetStage<GameStage>()->GetCollisionStateVec();
		for (auto& v : StateVec) {
			if (v.m_Src == &body) {
				Vec3 Normal = v.m_SrcHitNormal;
				Normal.normalize();
				Vec4 v = (Vec4)XMVector3AngleBetweenNormals(Vec3(0, 1, 0), Normal);
				if (v.x < 0.1f) {
					m_JumpLock = false;
					break;
				}
			}
			if (v.m_Dest == &body) {
				Vec3 Normal = v.m_SrcHitNormal;
				Normal.normalize();
				Vec4 v = (Vec4)XMVector3AngleBetweenNormals(Vec3(0, 1, 0), Normal);
				if (v.x < 0.1f) {
					m_JumpLock = false;
					break;
				}
			}
		}
		//��]�̍X�V
		//Velocity�̒l�ŁA��]��ύX����
		Vec3 Temp = body.m_Velocity;
		Temp.normalize();
		float ToAngle = atan2(Temp.x, Temp.z);
		Quat Qt;
		Qt.rotationRollPitchYawFromVector(Vec3(0, ToAngle, 0));
		Qt.normalize();
		//���݂ƖڕW����
		body.m_Quat = XMQuaternionSlerp(body.m_Quat, Qt, 0.1f);
	}

	void ObbPlayer::OnDraw() {
		auto& body = GetStage<GameStage>()->GetOwnRigidbody(GetThis<GameObject>());
		auto TexPtr = App::GetApp()->GetResource<TextureResource>(m_TextureResName);
		auto PtrGameStage = GetStage<GameStage>();
		//���[���h�s��̌���
		Mat4x4 World;
		World.affineTransformation(body.m_Scale, Vec3(0, 0, 0),
			body.m_Quat, body.m_Pos);
		auto shptr = PtrGameStage->FindTagGameObject<PNTDrawObject>(L"PNTDrawObject");
		shptr->AddDrawMesh(
			m_ObbMesh,
			TexPtr,
			World,
			true
		);
	}



}
//end basecross
