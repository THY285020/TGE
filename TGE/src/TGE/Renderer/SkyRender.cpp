#include "tgpch.h"
#include "SkyRender.h"
#include "RenderCommand.h"
#include "Renderer3D.h"
#include "TGE/Math/Math.h"
#include "PerspCamera.h"

namespace TGE
{

	void SkyRender::Init(float viewportWidth, float viewportHeight)
	{
		m_Shader = Shader::Create("assets/shaders/Sky.glsl");
		m_Texture = Texture2D::Create("assets/image/SkyTex.png");

		FrameBufferSpecification fbSpec;
		fbSpec.Width = viewportWidth;
		fbSpec.Height = viewportHeight;
		fbSpec.Attachments = { FramebufferTextureFormat::RGB16F, FramebufferTextureFormat::RED_INT };
		m_FBO = FrameBuffer::Create(fbSpec);

		FrameBufferSpecification fbSpecCube;
		fbSpecCube.Width = 512;
		fbSpecCube.Height = 512;
		fbSpecCube.Attachments = { FramebufferTextureFormat::Cube_RGB8, FramebufferTextureFormat::RED_INT };
		m_CubeFBO = FrameBuffer::Create(fbSpecCube);
	}

	void SkyRender::Render(EditorCamera& camera)
	{

		m_FBO->Bind();
		RenderCommand::SetCullFaceFront();

		float turbidity = 4.0f;
		float exposure = 30.f;
		float thetaS;

		float latitude = 0.0f;//1.0f维度
		float A2R = 180.0f / PI;
		float l = latitude * A2R;
		float day = 180;
		float hour = 12.f;
		float lightR = 3000.0f;
		glm::vec4 SunPos;

		float paraA = 0.17f * sin(4.0f * PI * (day - 80.0f) / 373.0f);
		float paraB = 0.129f * sin(2.0f * PI * (day - 8.0f) / 355.0f);
		float t = hour + paraA - paraB;

		float delta = 0.4093f * sin(2.0f * PI * (day - 81.0f) / 368.0f);
		thetaS = PI * 0.5f - asin(sin(l) * sin(delta) - cos(l) * cos(delta) * cos(PI * t / 12.0f));
		float Phi = atan((-cos(delta) * sin(PI * t / 12.0f)) / (cos(l) * sin(delta) - sin(l) * cos(delta) * cos(PI * t / 12.0f)));
		SunPos.y = lightR * cos(thetaS);
		SunPos.x = -lightR * sin(thetaS) * sin(Phi);
		SunPos.z = lightR * sin(thetaS) * cos(Phi);
		SunPos.w = 1.0f;

		//zenith
		glm::vec3 zenith;
		float X = ((4.0f / 9.0f) - (turbidity / 120.0f)) * (PI - 2.0f * thetaS);
		zenith.z = (4.0453f * turbidity - 4.9710f) * tan(X) - 0.2155f * turbidity + 2.4192f;
		float ThetaS_3 = pow(thetaS, 3.0f);
		float ThetaS_2 = pow(thetaS, 2.0f);
		float T_2 = turbidity * turbidity;
		glm::vec4 temp(T_2 * 0.00166f - turbidity * 0.02903f + 0.11693f,
			-T_2 * 0.00375f + turbidity * 0.06377f - 0.21196f,
			T_2 * 0.00209f - turbidity * 0.03202f + 0.06052f,
			turbidity * 0.00394f + 0.25886f);
		zenith.x = ThetaS_3 * temp.x + ThetaS_2 * temp.y + thetaS * temp.z + temp.w;

		temp = glm::vec4(T_2 * 0.00275f - turbidity * 0.04214f + 0.15346f,
			-T_2 * 0.00610f + turbidity * 0.08970f - 0.26756f,
			T_2 * 0.00317f - turbidity * 0.04156f + 0.06670f,
			turbidity * 0.00516f + 0.26688f);
		zenith.y = ThetaS_3 * temp.x + ThetaS_2 * temp.y + thetaS * temp.z + temp.w;

		glm::mat4 transform = glm::scale(glm::mat4(1.0f), glm::vec3(10000.0f, 10000.0f, 10000.0f));

		m_Shader->Bind();
		m_Shader->SetFloat("turbidity", turbidity);
		m_Shader->SetFloat("exposure", exposure);
		m_Shader->SetFloat("thetaS", thetaS);
		m_Shader->SetFloat4("SunPos", SunPos);
		m_Shader->SetFloat3("zenith", zenith);
		m_Shader->SetMat4("model", transform);
		m_Shader->SetMat4("viewProj", camera.GetViewProjection());

		m_Texture->Bind(0);//绑定到slot
		
		Ref<VertexArray> VAO = Renderer3D::GetSphereVAO();
		uint32_t SphereIndexCount = 65 * 64 * 2;
		RenderCommand::DrawIndexTS(VAO, SphereIndexCount);
		m_FBO->UnBind();

		//RenderCube
		constexpr int cubeSize = 512;
		PerspCamera cubeCam = PerspCamera();
		cubeCam.SetFarPlane(20000.0f);
		cubeCam.SetNearPlane(1000.0f);
		cubeCam.SetPosition(camera.GetPosition());
		//cubeCam.SetRotation(glm::vec3(camera.GetPitch(), -camera.GetYaw(), 0));//相机旋转
		cubeCam.SetAspectRatio(cubeSize/ cubeSize);
		//RenderCommand::SetViewport(0, 0, cubeSize, cubeSize);//包含在bind中
		glm::vec3 pos = { 0.0,0.0,0.0 };

		glm::vec3 rf[6] =
		{
			glm::vec3( 1, 0, 0),
			glm::vec3(-1, 0, 0),
			glm::vec3( 0, 1, 0),
			glm::vec3( 0,-1, 0),
			glm::vec3( 0, 0, 1),
			glm::vec3( 0, 0,-1)
		};

		glm::vec3 ru[6] =
		{
			glm::vec3(0,-1, 0),
			glm::vec3(0,-1, 0),
			glm::vec3(0, 0, 1),
			glm::vec3(0, 0,-1),
			glm::vec3(0,-1, 0),
			glm::vec3(0,-1, 0)
		};

		m_CubeFBO->Bind();
		for (int i = 0; i < 6; ++i)
		{
			m_CubeFBO->SetCubeAttachment(0, i, 0);
			cubeCam.SetLookAt(pos, rf[i], ru[i]);
			m_Shader->SetMat4("viewProj", cubeCam.GetViewProjectionMatrix());
			RenderCommand::DrawIndexTS(VAO, SphereIndexCount);
		}
		
		m_CubeTextureID = m_CubeFBO->GetCubeAttachment(0);

		TextureCube::GenMipmap(m_CubeTextureID);

		m_CubeFBO->UnBind();
		RenderCommand::CloseCullFace();
		RenderCommand::SetCullFaceBack();

	}
	void SkyRender::Resize(float width, float height)
	{
		m_FBO->Resize(width, width);
	}
}

