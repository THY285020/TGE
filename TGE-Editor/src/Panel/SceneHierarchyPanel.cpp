#include "SceneHierarchyPanel.h"
#include <imgui/imgui.h>
#include <imgui/imgui_internal.h>
#include <filesystem>
#include "TGE/Scene/Component.h"
#include "glm/gtc/type_ptr.hpp"
#include "Platform/Opengl/OpenGLTexture.h"
#include "stb_image.h"

namespace TGE
{
	SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
	{
		SetContext(context);
		//imageArray.fill(Texture2D::Create());
	}

	void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
	{
		m_Context = context;
		m_SelectionContext = {};
		//imageArray.fill(Texture2D::Create());
	}

	void SceneHierarchyPanel::OnImGuiRenderer()
	{
		ImGui::Begin("Scene Hierachy");
		m_Context->m_Registry.each([&](auto ent_id)
			{
				Entity entity = Entity(entt::entity(ent_id), m_Context.get());
				DrawEntityNode(entity);
			});

		//若左键击空白处则置空
		if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered())
			m_SelectionContext = {};
		
		//右键空白处
		if (ImGui::BeginPopupContextWindow(0, 1))//0是ID，1是右键
		{
			if (ImGui::MenuItem("Create Entity"))
				m_SelectionContext = m_Context->CreateEntity("Empty Entity");
			ImGui::EndPopup();
		}

		ImGui::End();

		ImGui::Begin("Propertirs");
		if (m_SelectionContext)
		{
			DrawComponents(m_SelectionContext);

		}
	
		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntityNode(Entity entity)
	{
		auto& tag = entity.GetComponent<TagComponent>().Tag;
		//选中或点击箭头则展开标志成立
		ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0)|ImGuiTreeNodeFlags_OpenOnArrow;
		//flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
		bool opened = ImGui::TreeNodeEx((void*)(uint64_t)(uint32_t)(entity), flags, tag.c_str());//判断是否展开
		
		
		if (ImGui::IsItemClicked())//点击后设置为选中的entity
		{
			m_SelectionContext = entity;
			//双击后更新相机位置
			double current_time = ImGui::GetTime();
			if (current_time - last_time <= 1)
			{
				m_Context->UpdateEditCamera(entity.GetComponent<TransformComponent>().Translate);
			}
			last_time = current_time;
		}

		//删除entity菜单
		bool entityDeleted = false;
		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				entityDeleted = true;
			}
			ImGui::EndPopup();
		}

		if (opened)
		{
			//ImGuiTreeNodeFlags flags = ((m_SelectionContext == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow;
			//flags |= ImGuiTreeNodeFlags_SpanAvailWidth;
			//bool opened = ImGui::TreeNodeEx((void*)12342, flags, tag.c_str());//判断是否展开
			//if (opened)
			//	ImGui::TreePop();

			ImGui::TreePop();
		}


		if (entityDeleted)
		{
			m_Context->DestroyEntity(entity);
			if (m_SelectionContext == entity)
				m_SelectionContext = {};
		}
			
	}

	void SceneHierarchyPanel::DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue, float columnWidth)
	{
		ImGuiIO& io = ImGui::GetIO();
		auto boldFont = io.Fonts->Fonts[1];

		ImGui::PushID(label.c_str());

		ImGui::Columns(2);
		ImGui::SetColumnWidth(0, columnWidth);
		ImGui::Text(label.c_str());
		ImGui::NextColumn();

		ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0.f, 0.f));

		float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
		ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f,1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f,1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f,1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("X", buttonSize))
			values.x = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##X", &values.x, 0.1, 0.0, 0.0, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.7f, 0.15f,1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.8f, 0.2f,1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.7f, 0.15f,1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Y", buttonSize))
			values.y = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Y", &values.y, 0.1, 0.0, 0.0, "%.2f");
		ImGui::PopItemWidth();
		ImGui::SameLine();

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.2f, 0.8f, 1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.3f, 0.9f,1.0f });
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.2f, 0.8f, 1.0f });
		ImGui::PushFont(boldFont);
		if (ImGui::Button("Z", buttonSize))
			values.z = resetValue;
		ImGui::PopFont();
		ImGui::PopStyleColor(3);
		ImGui::SameLine();
		ImGui::DragFloat("##Z", &values.z, 0.1, 0.0, 0.0, "%.2f");
		ImGui::PopItemWidth();


		ImGui::PopStyleVar();
		
		ImGui::Columns(1);

		ImGui::PopID();
	}

	template<typename T, typename UIFunction>
	void SceneHierarchyPanel::DrawComponent(const std::string name, Entity entity, UIFunction function)
	{
		const ImGuiTreeNodeFlags treeNodeFlags = 
			ImGuiTreeNodeFlags_DefaultOpen 
			| ImGuiTreeNodeFlags_Framed 
			| ImGuiTreeNodeFlags_SpanAvailWidth
			| ImGuiTreeNodeFlags_FramePadding
			| ImGuiTreeNodeFlags_AllowItemOverlap ;
		if (entity.HasComponent<T>())
		{
			auto& component = entity.GetComponent<T>();

			ImVec2 contentRegionAvailable = ImGui::GetContentRegionAvail();
			ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 4));
			float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
			ImGui::Separator();
			bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), treeNodeFlags, name.c_str());
			ImGui::PopStyleVar();

			ImGui::SameLine(contentRegionAvailable.x - lineHeight*0.5);//ImGui::GetWindowWidth() - 35.0f
			if (ImGui::Button("+", ImVec2{ lineHeight, lineHeight }))
			{
				ImGui::OpenPopup("ComponentSettings");
			}

			bool removeComponent = false;
			if (ImGui::BeginPopup("ComponentSettings"))
			{
				if (ImGui::MenuItem("Remove component"))
					removeComponent = true;
				ImGui::EndPopup();
			}

			if (open)
			{
				function(component);
				ImGui::TreePop();
			}

			if (removeComponent)
			{
				//如果是transform组件则直接删除实体
				if (std::is_same<T, TransformComponent>::value)
				{
					if (m_SelectionContext == entity)
						m_SelectionContext = {};
					m_Context->DestroyEntity(entity);
				}
				else//不是则仅删除组件
				{
					entity.RemoveComponent<T>();
				}
			}				
		}
		
	}

	//绘制各种组件的属性
	void SceneHierarchyPanel::DrawComponents(Entity entity)
	{
		if (entity.HasComponent<TagComponent>())
		{
			auto& tag = entity.GetComponent<TagComponent>().Tag;

			char buffer[256];
			memset(buffer, 0, sizeof(buffer));
			strcpy_s(buffer, sizeof(buffer), tag.c_str());
			if(ImGui::InputText("##Tag",buffer, sizeof(buffer)))
			{
				tag = std::string(buffer);//如果InputText被改变则改变TagComponent
			}
		}
		//=========================Add Component=======================
		ImGui::SameLine();
		ImGui::PushItemWidth(-1);
		if (ImGui::Button("Add Component"))
			ImGui::OpenPopup("AddComponent");
		if (ImGui::BeginPopup("AddComponent"))
		{
			if (!entity.HasComponent<TransformComponent>() && ImGui::MenuItem("Transform"))
			{
				m_SelectionContext.AddComponent<TransformComponent>();
				ImGui::CloseCurrentPopup();
			}
			if (!entity.HasComponent<SpriteRendererComponent>() && ImGui::MenuItem("Sprite Renderer"))
			{
				m_SelectionContext.AddComponent<SpriteRendererComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (!entity.HasComponent<CircleRendererComponent>() && ImGui::MenuItem("Circle Renderer"))
			{
				m_SelectionContext.AddComponent<CircleRendererComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (!entity.HasComponent<CubeRendererComponent>() && ImGui::MenuItem("Cube Renderer"))
			{
				m_SelectionContext.AddComponent<CubeRendererComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (!entity.HasComponent<SphereRendererComponent>() && ImGui::MenuItem("Sphere Renderer"))
			{
				m_SelectionContext.AddComponent<SphereRendererComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (!entity.HasComponent<CameraComponent>() && ImGui::MenuItem("Camera"))
			{
				m_SelectionContext.AddComponent<CameraComponent>();
				ImGui::CloseCurrentPopup();
			}
			if (!entity.HasComponent<RigidBody2DComponent>() && ImGui::MenuItem("RigidBody 2D"))
			{
				m_SelectionContext.AddComponent<RigidBody2DComponent>();
				ImGui::CloseCurrentPopup();
			}
			if (!entity.HasComponent<BoxCollider2DComponent>() && ImGui::MenuItem("BoxCollider 2D"))
			{
				m_SelectionContext.AddComponent<BoxCollider2DComponent>();
				ImGui::CloseCurrentPopup();
			}

			if (!entity.HasComponent<CircleCollider2DComponent>() && ImGui::MenuItem("CircleCollider 2D"))
			{
				m_SelectionContext.AddComponent<CircleCollider2DComponent>();
				ImGui::CloseCurrentPopup();
			}
			ImGui::EndPopup();
		}

		ImGui::PopItemWidth();
		//=========================================================

		DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
			{
				DrawVec3Control("Translation", component.Translate);

				auto rotation = glm::degrees(component.Rotation);
				DrawVec3Control("Rotation", rotation);
				component.Rotation = glm::radians(rotation);

				DrawVec3Control("Scale", component.Scale, 1.0f);
			});

		
		DrawComponent<CameraComponent>("Camera", entity, [](auto& component) 
			{
				auto& camera = component.camera;
				ImGui::Checkbox("Primary", &component.Primary);

				const char* projectionType[] = { "Perspective", "Orthographic" };
				const char* currentProjectionTypeString = projectionType[(int)camera.GetProjectionType()];
				if (ImGui::BeginCombo("Projection", currentProjectionTypeString))
				{
					for (int i = 0; i < 2; ++i)
					{
						bool isSelected = currentProjectionTypeString == projectionType[i];
						if (ImGui::Selectable(projectionType[i], isSelected))
						{
							currentProjectionTypeString = projectionType[i];
							camera.SetProjectionType((SceneCamera::ProjectionType)i);
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
				{
					float orthoSize = camera.GetOrthographicSize();
					if (ImGui::DragFloat("Size", &orthoSize))
						camera.SetOrthographicSize(orthoSize);

					float orthoNear = camera.GetOrthographicNearClip();
					if (ImGui::DragFloat("Near", &orthoNear))
						camera.SetOrthographicNearClip(orthoNear);

					float orthoFar = camera.GetOrthographicFarClip();
					if (ImGui::DragFloat("Far", &orthoFar))
						camera.SetOrthographicFarClip(orthoFar);
				}

				if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
				{
					float persFOV = glm::degrees(camera.GetPerspectiveFOV());
					if (ImGui::DragFloat("FOV", &persFOV))
						camera.SetPerspectiveFOV(glm::radians(persFOV));

					float persNear = camera.GetPerspectiveNearClip();
					if (ImGui::DragFloat("Near", &persNear))
						camera.SetPerspectiveNearClip(persNear);

					float persFar = camera.GetPerspectiveFarClip();
					if (ImGui::DragFloat("Far", &persFar))
						camera.SetPerspectiveFarClip(persFar);
				}
			});

		DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](SpriteRendererComponent& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				//ImGui::Button("Texture", ImVec2(100.f, 0.f));
				ImGui::Image((void*)component.Texture->GetRendererID(), ImVec2{ 50.f, 50.f }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path texturePath = std::filesystem::path(path);
						component.Texture = Texture2D::Create(texturePath.string());
						component.usingTexture = true;
					}
					ImGui::EndDragDropTarget();
				}
				//Texture
				ImGui::DragFloat("Tiling Factor", &component.TilingFactor, 0.1f, 0.0f, 100.f);
			});

		DrawComponent<CircleRendererComponent>("Circle Renderer", entity, [](CircleRendererComponent& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				ImGui::DragFloat("Thickness", &component.Thickness, 0.025f, 0.0f, 1.0f);
				ImGui::DragFloat("Fade", &component.Fade, 0.0025f, 0.0f, 1.0f);
			});

		DrawComponent<CubeRendererComponent>("Cube Renderer", entity, [](CubeRendererComponent& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				ImGui::Image((void*)component.Texture->GetRendererID(), ImVec2{ 50.f, 50.f }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path texturePath = std::filesystem::path(path);
						component.Texture = Texture2D::Create(texturePath.string());
						component.usingTexture = true;
					}
					ImGui::EndDragDropTarget();
				}
			});

		DrawComponent<SphereRendererComponent>("Sphere Renderer", entity, [](SphereRendererComponent& component)
			{
				ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
				ImGui::Image((void*)component.Texture->GetRendererID(), ImVec2{ 50.f, 50.f }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });
				if (ImGui::BeginDragDropTarget())
				{
					if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
					{
						const wchar_t* path = (const wchar_t*)payload->Data;
						std::filesystem::path texturePath = std::filesystem::path(path);
						component.Texture = Texture2D::Create(texturePath.string());
						component.usingTexture = true;
					}
					ImGui::EndDragDropTarget();
				}
			});

		DrawComponent<RigidBody2DComponent>("RigidBody 2D", entity, [](RigidBody2DComponent& component)
			{
				const char* BodyType[] = { "Static", "Kinematic", "Dynamic"};
				const char* currentBodyTypeString = BodyType[(int)component.Type];
				if (ImGui::BeginCombo("BodyType", currentBodyTypeString))
				{
					for (int i = 0; i <= 2; ++i)
					{
						bool isSelected = currentBodyTypeString == BodyType[i];
						if (ImGui::Selectable(BodyType[i], isSelected))
						{
							currentBodyTypeString = BodyType[i];
							component.Type = (RigidBody2DComponent::BodyType)i;
						}
						if (isSelected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}

				ImGui::Checkbox("Fixed Rotation", &component.FixedRotation);
			});

		DrawComponent<BoxCollider2DComponent>("<BoxCollider 2D", entity, [](BoxCollider2DComponent& component)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
				ImGui::DragFloat2("Size", glm::value_ptr(component.Size));
				ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);

			});

		DrawComponent<CircleCollider2DComponent>("CircleCollider 2D", entity, [](CircleCollider2DComponent& component)
			{
				ImGui::DragFloat2("Offset", glm::value_ptr(component.Offset));
				ImGui::DragFloat("Radius", &component.Radius);
				ImGui::DragFloat("Density", &component.Density, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Friction", &component.Friction, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution", &component.Restitution, 0.01f, 0.0f, 1.0f);
				ImGui::DragFloat("Restitution Threshold", &component.RestitutionThreshold, 0.01f, 0.0f);

			});

		//if (entity.HasComponent<SpriteRendererComponent>())
		//{
		//	bool open = ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), treeNodeFlags, "Sprite Renderer");
		//	
		//	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 0,0 });
		//	ImGui::SameLine(ImGui::GetWindowWidth() - 35.0f);
		//	if (ImGui::Button("+", ImVec2{15, 15}))
		//	{
		//		ImGui::OpenPopup("ComponentSettings");
		//	}
		//	ImGui::PopStyleVar();

		//	bool removeComponent = false;
		//	if (ImGui::BeginPopup("ComponentSettings"))
		//	{
		//		if (ImGui::MenuItem("Remove component"))
		//			removeComponent = true;
		//		ImGui::EndPopup();
		//	}

		//	if (open)
		//	{
		//		auto& src = entity.GetComponent<SpriteRendererComponent>();
		//		ImGui::ColorEdit4("Color", glm::value_ptr(src.Color));
		//		ImGui::TreePop();
		//	}

		//	if (removeComponent)
		//		entity.RemoveComponent<SpriteRendererComponent>();
		//}
	}
}

