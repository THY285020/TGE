#include "tgpch.h"
#include "Serializer.h"
#include <yaml-cpp/yaml.h>
#include <fstream>
#include"Component.h"

namespace YAML
{
	template<>
	struct convert<glm::vec3>
	{
		static Node encode(const glm::vec3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}
		static bool decode(const Node& node, glm::vec3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<glm::vec4>
	{
		static Node encode(const glm::vec4& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.push_back(rhs.w);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, glm::vec4& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			rhs.w = node[3].as<float>();
			return true;
		}
	};
}
namespace TGE
{
	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
	{
		out << YAML::Flow;//[x,y,z]
		out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
		return out;
	}

	YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
	{
		out << YAML::Flow;
		out << YAML::BeginSeq << v.x << v.y << v.z <<v.w << YAML::EndSeq;
		return out;
	}
	Serializer::Serializer(const Ref<Scene>& scene)
		:m_Scene(scene)
	{
	}

	static void SerializeEntity(YAML::Emitter& out, Entity entity)
	{
		out << YAML::BeginMap;//Entity
		out << YAML::Key << "Entity";
		out << YAML::Value << "121312131";

		if (entity.HasComponent<TagComponent>())
		{
			out << YAML::Key << "TagComponent";
			out << YAML::BeginMap;//TagComponent
			auto& tag = entity.GetComponent<TagComponent>().Tag;
			out << YAML::Key << "Tag" << YAML::Value << tag;
			out << YAML::EndMap;//TagComponent
		}

		if (entity.HasComponent<TransformComponent>())
		{
			out << YAML::Key << "TransformComponent";
			out << YAML::BeginMap;//TransformComponent
			auto& tc = entity.GetComponent<TransformComponent>();
			out << YAML::Key << "Translate" << YAML::Value << tc.Translate;
			out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
			out << YAML::Key << "Scale" << YAML::Value << tc.Scale;
			out << YAML::EndMap;//TransformComponent
		}

		if (entity.HasComponent<CameraComponent>())
		{
			out << YAML::Key << "CameraComponent";
			out << YAML::BeginMap;//CameraComponent
			auto& cc = entity.GetComponent<CameraComponent>();
			auto& Camera = cc.camera;

			out << YAML::Key << "Camera" << YAML::Value ;
			out << YAML::BeginMap;//Camera
			out << YAML::Key << "ProjectionType" << YAML::Value << (int)Camera.GetProjectionType();
			out << YAML::Key << "PerspectiveFOV" << YAML::Value << Camera.GetPerspectiveFOV();
			out << YAML::Key << "PerspectiveNear" << YAML::Value << Camera.GetPerspectiveNearClip();
			out << YAML::Key << "PerspectiveFar" << YAML::Value << Camera.GetPerspectiveFarClip();
			out << YAML::Key << "OrthographicSize" << YAML::Value << Camera.GetOrthographicSize();
			out << YAML::Key << "OrthographicNear" << YAML::Value << Camera.GetOrthographicNearClip();
			out << YAML::Key << "OrthographicFar" << YAML::Value << Camera.GetOrthographicFarClip();
			out << YAML::EndMap;//Camera

			out << YAML::Key << "Primary" << YAML::Value << cc.Primary;
			out << YAML::Key << "FixedAspectRatio" << YAML::Value << cc.FixedAspectRatio;

			out << YAML::EndMap;//CameraComponent
		}

		if (entity.HasComponent<SpriteRendererComponent>())
		{
			out << YAML::Key << "SpriteRendererComponent";
			out << YAML::BeginMap;//SpriteRendererComponent

			auto& src = entity.GetComponent<SpriteRendererComponent>();

			out << YAML::Key << "Color" << YAML::Value << src.Color;
			out << YAML::EndMap;//SpriteRendererComponent
		}

		out << YAML::EndMap;//Entity
	}

	void Serializer::Serialize(const std::string& filepath)
	{
		YAML::Emitter out;
		out << YAML::BeginMap;
		out << YAML::Key << "Scene" << YAML::Value << "Untitled";
		out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		m_Scene->m_Registry.each([&](auto entityID)
			{
				Entity entity = { entityID, m_Scene.get() };
				if (!entity)
					return;
				SerializeEntity(out, entity);
			});
		out << YAML::EndSeq;
		out << YAML::EndMap;

		std::ofstream fout(filepath);
		fout << out.c_str();//输出到文件
	}
	void Serializer::SerializeRuntime(const std::string& filepath)
	{
		TGE_CORE_ERROR(false);
	}
	bool Serializer::DeSerialize(const std::string& filepath)
	{
		std::ifstream stream(filepath);
		std::stringstream strStream;
		strStream << stream.rdbuf();

		YAML::Node data = YAML::Load(strStream.str());//数据读入到data
		if (!data["Scene"])
			return false;

		//读入场景名称
		std::string sceneName = data["Scene"].as<std::string>();
		TGE_CORE_TRACE("Deserializing scene '{0}'", sceneName);

		auto entities = data["Entities"];
		if (entities)
		{
			for (auto entity : entities)
			{
				uint64_t uuid = entity["Entity"].as<uint64_t>();

				std::string name;//读入tag名
				auto tagComponent = entity["TagComponent"];
				if (tagComponent)
					name = tagComponent["Tag"].as<std::string>();
				TGE_CORE_TRACE("Deserialized entity with ID = {0}, name = {1}", uuid, name);


				//创建实例
				auto transformComponent = entity["TransformComponent"];
				Entity deserializedEntity = m_Scene->CreateEntity(name, transformComponent["Translate"].as<glm::vec3>());

				//auto transformComponent = entity["TransformComponent"];
				if (transformComponent)
				{
					//auto& tc = deserializedEntity.AddComponent<TransformComponent>();
					auto& tc = deserializedEntity.GetComponent<TransformComponent>();
					//tc.Translate = transformComponent["Translate"].as<glm::vec3>();
					tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
					tc.Scale = transformComponent["Scale"].as<glm::vec3>();
				}

				auto cameraComponent = entity["CameraComponent"];
				if (cameraComponent)
				{
					auto& cc = deserializedEntity.AddComponent<CameraComponent>();
					auto& cameraProps = cameraComponent["Camera"];

					cc.camera.SetProjectionType((SceneCamera::ProjectionType)cameraProps["ProjectionType"].as<int>());

					cc.camera.SetPerspectiveFOV(cameraProps["PerspectiveFOV"].as<float>());
					cc.camera.SetPerspectiveNearClip(cameraProps["PerspectiveNear"].as<float>());
					cc.camera.SetPerspectiveFarClip(cameraProps["PerspectiveFar"].as<float>());

					cc.camera.SetOrthographicSize(cameraProps["OrthographicSize"].as<float>());
					cc.camera.SetOrthographicNearClip(cameraProps["OrthographicNear"].as<float>());
					cc.camera.SetOrthographicFarClip(cameraProps["OrthographicFar"].as<float>());

					cc.Primary = cameraComponent["Primary"].as<bool>();
					cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
				}

				auto spriteRendererComponent = entity["SpriteRendererComponent"];
				if (spriteRendererComponent)
				{
					auto& src = deserializedEntity.AddComponent<SpriteRendererComponent>();
					src.Color = spriteRendererComponent["Color"].as<glm::vec4>();
				}
			}
		}
		return true;
	}
	bool Serializer::DeSerializeRuntime(const std::string& filepath)
	{
		TGE_CORE_ERROR(false);
		return false;
	}
}