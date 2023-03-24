#pragma once
#include"TGE.h"
#include"TGE/Core/Layer.h"
#include"ParticleSystem.h"

class Sandbox2D:public TGE::Layer
{
public:
	Sandbox2D();
	virtual ~Sandbox2D() = default;

	virtual void OnImGuiRender() override;
	void OnUpdate(TGE::TimeStep& ts) override;
	void OnEvent(TGE::Event& event) override;

	virtual void OnAttach() override;
	virtual void OnDetach() override;
private:
	TGE::OrthoCameraController m_CameraController;
	float MoveSpeed = 10.f;
	double time_temp = 0.0;
	//temp
	TGE::Ref<TGE::Shader> m_Shader;
	TGE::Ref<TGE::VertexArray> m_VertexArray;
	TGE::Ref<TGE::Texture2D> m_Texture;
	TGE::Ref<TGE::Texture2D> m_sheetTexture;
	TGE::Ref<TGE::SubTexture2D> m_StairTexture;
	TGE::Ref<TGE::SubTexture2D> m_BarrelTexture;
	TGE::Ref<TGE::SubTexture2D> m_TreeTexture;
	glm::vec4 SquareColor = glm::vec4(0.2, 0.3, 0.8, 1.0f);

	ParticleProps m_Particle;
	ParticleSystem m_ParticleSystem;
};

