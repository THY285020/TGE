#include "tgpch.h"

#include <glm/gtx/matrix_decompose.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "ImGuizmo.h"

#include "EditorLayer.h"
#include "TGE/Utils/PlatformUtils.h"
#include "TGE/Scene/Serializer.h"
#include "TGE/Math/Math.h"

namespace TGE
{
    EditorLayer::EditorLayer() :Layer("EditorLayer"), m_CameraController(1280.f / 720.f, true)
    {
    }

    void EditorLayer::OnAttach()
    {
        ////Texture Init here
        //m_Texture = Texture2D::Create("assets/textures/wood.png");

        FrameBufferSpecification fbSpec;
        fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INT, FramebufferTextureFormat::Depth };
        fbSpec.Width = Application::Get().GetWindow().GetWidth();//Application::Get().GetWindow().GetWidth();
        fbSpec.Height = Application::Get().GetWindow().GetHeight();//Application::Get().GetWindow().GetHeight();
        m_FrameBuffer = FrameBuffer::Create(fbSpec);

        //Entity
        m_ActiveScene = std::make_shared<Scene>(fbSpec.Width, fbSpec.Height);//创建registry
#if 0

    //m_Camera = m_ActiveScene->CreateEntity("Camera EntityA", glm::vec3(0.f, 0.f, 5.0f));
    //m_Camera.AddComponent<CameraComponent>();//glm::ortho(-16.f, 16.f, -9.f, 9.f, -1.f, 1.f)
    //m_Camera.GetComponent<CameraComponent>().camera.SetPerspective(glm::radians(45.0f), 0.1f, 100.f);

    //m_SquareEntity = m_ActiveScene->CreateEntity("Square Green", glm::vec3(-1.f, 0.0f, 0.f));//创建entity
    //m_SquareEntity.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0, 1.0, 0.0, 1.0 });

        m_SquareEntity = m_ActiveScene->CreateEntity("Square Green");//创建entity
        m_SquareEntity.AddComponent<TransformComponent>(glm::vec3(-1.f, 0.0f, 0.f));
        m_SquareEntity.AddComponent<SpriteRendererComponent>(glm::vec4{ 0.0, 1.0, 0.0, 1.0 });

        m_SquareEntity2 = m_ActiveScene->CreateEntity("Square Red");//创建entity
        m_SquareEntity2.AddComponent<TransformComponent>(glm::vec3(0.5f, 0.0f, 0.f));
        m_SquareEntity2.AddComponent<SpriteRendererComponent>(glm::vec4{ 1.0, 0.0, 0.0, 1.0 });

        m_Camera = m_ActiveScene->CreateEntity("Camera EntityA");
        m_Camera.AddComponent<CameraComponent>();//glm::ortho(-16.f, 16.f, -9.f, 9.f, -1.f, 1.f)
        m_Camera.AddComponent<TransformComponent>(glm::vec3(0.f, 0.f, 5.0f));//初始化位置
        m_Camera.GetComponent<CameraComponent>().camera.SetPerspective(glm::radians(45.0f), 0.0f, 1000.f);


        m_Camera2 = m_ActiveScene->CreateEntity("Camera EntityB");
        auto& cc = m_Camera2.AddComponent<CameraComponent>();//glm::ortho(-1.f, 1.f, -1.f, 1.f, -1.f, 1.f)
        cc.Primary = false;
        m_Camera2.AddComponent<TransformComponent>();

        class CameraController :public ScriptableEntity
        {
        public:
            void OnCreate()
            {
                std::cout << "CameraComtroller:OnCreate!" << std::endl;
            }
            void OnDestroy()
            {
            }
            void OnUpdate(TimeStep ts)
            {
                auto& translation = GetComponent<TransformComponent>().Translate;

                float speed = 5.0f;
                if (Input::IsKeyPressed(TGE_KEY_A))
                    translation.x += speed * ts;
                if (Input::IsKeyPressed(TGE_KEY_D))
                    translation.x -= speed * ts;
                if (Input::IsKeyPressed(TGE_KEY_W))
                    translation.y += speed * ts;
                if (Input::IsKeyPressed(TGE_KEY_S))
                    translation.y -= speed * ts;
            }
        };

        绑定OnUpdate等函数
            m_Camera2.AddComponent<NativeScriptComponent>().Bind<CameraController>();
#endif
        m_EditorCamera = EditorCamera(30.f, fbSpec.Width / fbSpec.Height, 0.1, 1000.f);
        m_SHP.SetContext(m_ActiveScene);
    }

    void EditorLayer::EditTransform(float* cameraView, float* cameraProjection, float* matrix, int& m_GizmoType)
    {
        static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);
        //static bool useSnap = false;
        //static float snap[3] = { 1.f, 1.f, 1.f };
        //static float bounds[] = { -0.5f, -0.5f, -0.5f, 0.5f, 0.5f, 0.5f };
        //static float boundsSnap[] = { 0.1f, 0.1f, 0.1f };
        //static bool boundSizing = false;
        //static bool boundSizingSnap = false;
        //m_GizmoType = ImGuizmo::TRANSLATE;


            //if (ImGui::IsKeyPressed(ImGuiKey_T))
            //    m_GizmoType = ImGuizmo::TRANSLATE;
            //if (ImGui::IsKeyPressed(ImGuiKey_E))
            //    m_GizmoType = ImGuizmo::ROTATE;
            //if (ImGui::IsKeyPressed(ImGuiKey_R)) // r Key
            //    m_GizmoType = ImGuizmo::SCALE;

            //if (ImGui::RadioButton("Translate", m_GizmoType == ImGuizmo::TRANSLATE))
            //    m_GizmoType = ImGuizmo::TRANSLATE;
            //ImGui::SameLine();
            //if (ImGui::RadioButton("Rotate", m_GizmoType == ImGuizmo::ROTATE))
            //    m_GizmoType = ImGuizmo::ROTATE;
            //ImGui::SameLine();
            //if (ImGui::RadioButton("Scale", m_GizmoType == ImGuizmo::SCALE))
            //    m_GizmoType = ImGuizmo::SCALE;
            //if (ImGui::RadioButton("Universal", m_GizmoType == ImGuizmo::UNIVERSAL))
            //    m_GizmoType = ImGuizmo::UNIVERSAL;

            //float matrixTranslation[3], matrixRotation[3], matrixScale[3];
            //ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
            //ImGui::InputFloat3("Tr", matrixTranslation);
            //ImGui::InputFloat3("Rt", matrixRotation);
            //ImGui::InputFloat3("Sc", matrixScale);
            //ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, matrix);

            //if (mCurrentGizmoOperation != ImGuizmo::SCALE)
            //{
            //    if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
            //        mCurrentGizmoMode = ImGuizmo::LOCAL;
            //    ImGui::SameLine();
            //    if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
            //        mCurrentGizmoMode = ImGuizmo::WORLD;
            //}

            //SNAP
        bool snap = Input::IsKeyPressed(TGE_KEY_LEFT_CONTROL);
        float snapValue = 0.5f; // Snap to 0.5m for translation/scale
        // Snap to 45 degrees for rotation
        if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
            snapValue = 30.0f;

        float snapValues[3] = { snapValue, snapValue, snapValue };
        //if (ImGui::IsKeyPressed(ImGuiKey_S))
        //    useSnap = !useSnap;
        //ImGui::Checkbox("##UseSnap", &useSnap);
        //ImGui::SameLine();

        //switch (mCurrentGizmoOperation)
        //{
        //case ImGuizmo::TRANSLATE:
        //    ImGui::InputFloat3("Snap", &snap[0]);
        //    break;
        //case ImGuizmo::ROTATE:
        //    ImGui::InputFloat("Angle Snap", &snap[0]);
        //    break;
        //case ImGuizmo::SCALE:
        //    ImGui::InputFloat("Scale Snap", &snap[0]);
        //    break;
        //}
        //ImGui::Checkbox("Bound Sizing", &boundSizing);
        //if (boundSizing)
        //{
        //    ImGui::PushID(3);
        //    ImGui::Checkbox("##BoundSizing", &boundSizingSnap);
        //    ImGui::SameLine();
        //    ImGui::InputFloat3("Snap", boundsSnap);
        //    ImGui::PopID();
        //}


        ImGuiIO& io = ImGui::GetIO();
        float viewManipulateRight = io.DisplaySize.x;
        float viewManipulateTop = 0;
        static ImGuiWindowFlags gizmoWindowFlags = 0;

        //ImGui::SetNextWindowSize(ImVec2(800, 400), ImGuiCond_Appearing);
        //ImGui::SetNextWindowPos(ImVec2(400, 20), ImGuiCond_Appearing);
        //ImGui::PushStyleColor(ImGuiCol_WindowBg, (ImVec4)ImColor(0.35f, 0.3f, 0.3f));
        //ImGui::Begin("Gizmo", 0, gizmoWindowFlags);
        ImGuizmo::SetDrawlist();
        float windowWidth = (float)ImGui::GetWindowWidth();
        float windowHeight = (float)ImGui::GetWindowHeight();
        ImGuizmo::SetRect(ImGui::GetWindowPos().x, ImGui::GetWindowPos().y, windowWidth, windowHeight);
        viewManipulateRight = ImGui::GetWindowPos().x + windowWidth;
        viewManipulateTop = ImGui::GetWindowPos().y;
        ImGuiWindow* window = ImGui::GetCurrentWindow();
        gizmoWindowFlags = ImGui::IsWindowHovered() && ImGui::IsMouseHoveringRect(window->InnerRect.Min, window->InnerRect.Max) ? ImGuiWindowFlags_NoMove : 0;


        //ImGuizmo::DrawGrid(cameraView, cameraProjection, identityMatrix, 100.f);
        //ImGuizmo::DrawCubes(cameraView, cameraProjection, &objectMatrix[0][0], 1);
        ImGuizmo::Manipulate(cameraView, cameraProjection, (ImGuizmo::OPERATION)m_GizmoType, mCurrentGizmoMode, matrix, NULL, snap ? snapValues : NULL);//matrix

        //ImGuizmo::ViewManipulate(cameraView, 8.f, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);    
    }


    //template <typename T, std::size_t N>
    //struct Array
    //{
    //    T data[N];
    //    const size_t size() const { return N; }
    //
    //    const T operator [] (size_t index) const { return data[index]; }
    //    operator T* () {
    //        T* p = new T[N];
    //        memcpy(p, data, sizeof(data));
    //        return p;
    //    }
    //};

    void EditorLayer::OnImGuiRender()
    {

        static bool dockspaceOpen = true;
        static bool opt_fullscreen = true;
        static bool opt_padding = false;
        static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

        // We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
        // because it would be confusing to have two docking targets within each others.
        ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
        if (opt_fullscreen)
        {
            const ImGuiViewport* viewport = ImGui::GetMainViewport();
            ImGui::SetNextWindowPos(viewport->WorkPos);
            ImGui::SetNextWindowSize(viewport->WorkSize);
            ImGui::SetNextWindowViewport(viewport->ID);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
            ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
            window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
            window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
        }
        else
        {
            dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
        }

        // When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
        // and handle the pass-thru hole, so we ask Begin() to not render a background.
        if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
            window_flags |= ImGuiWindowFlags_NoBackground;

        // Important: note that we proceed even if Begin() returns false (aka window is collapsed).
        // This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
        // all active windows docked into it will lose their parent and become undocked.
        // We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
        // any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
        if (!opt_padding)
            ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
        ImGui::Begin("DockSpace Demo", &dockspaceOpen, window_flags);
        if (!opt_padding)
            ImGui::PopStyleVar();

        if (opt_fullscreen)
            ImGui::PopStyleVar(2);

        // Submit the DockSpace
        ImGuiIO& io = ImGui::GetIO();
        if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
        {
            ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
            ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
        }

        if (ImGui::BeginMenuBar())
        {
            if (ImGui::BeginMenu("Files"))
            {
                // Disabling fullscreen would allow the window to be moved to the front of other windows,
                // which we can't undo at the moment without finer window depth/z control.
                //ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen);
                ImGui::MenuItem("Padding", NULL, &opt_padding);
                ImGui::Separator();

                //if (ImGui::MenuItem("Close", NULL, false, dockspaceOpen != NULL))
                //	dockspaceOpen = false;

                if (ImGui::MenuItem("New", "Ctrl+N"))
                    NewScene();

                if (ImGui::MenuItem("Save As...", "Ctrl+Shift+S"))
                    SaveSceneAs();

                if (ImGui::MenuItem("Open...", "Ctrl+O"))
                    OpenScene();

                if (ImGui::MenuItem("Exit"))
                    Application::Get().Close();
                ImGui::EndMenu();
            }
            ImGui::EndMenuBar();
        }

        //ImGui::Begin("Editor");
        //if (ImGui::RadioButton("Full view", !useWindow)) useWindow = false;
        //ImGui::SameLine();
        //if (ImGui::RadioButton("Window", useWindow)) useWindow = true;

        m_SHP.OnImGuiRenderer();//多级菜单
        m_CBP.OnImGuiRenderer();//资产菜单

        ImGui::Begin("Statistics");

        std::string name = "None";
        if (m_HoveredEntity)
            name = m_HoveredEntity.GetComponent<TagComponent>().Tag;
        ImGui::Text("Hovered Entity: %s", name.c_str());

        auto stats = Renderer2D::GetStats();
        ImGui::Text("Renderer2D Stats:");
        ImGui::Text("Draw Calls: %d", stats.DrawCalls);
        ImGui::Text("Quads : %d", stats.QuadCount);
        ImGui::Text("Vertices : %d", stats.GetTotalVertexCount());
        ImGui::Text("Indices : %d", stats.GetTotalIndexCount());
        ImGui::End();

        //----------------Viewport--------------------
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0, 0.0));//间隔缩小为0
        ImGui::Begin("ViewPort");
        
        //点击选中或悬停都会更新界面
        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered || !m_ViewportFocused);

        //Resize此处更新会闪屏
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize) && viewportPanelSize.x > 0 && viewportPanelSize.y > 0)
        {
            m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
        }//

        uint32_t m_TextureID = m_FrameBuffer->GetColorAttachment(0);
        ImGui::Image((void*)m_TextureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        //ViewportBounds ImGui从左上(0,0)到右下
        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset = ImGui::GetWindowPos();
        m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        Entity selectedEntity = m_SHP.GetSelectedEntity();
        //RunTime Camera
        //auto cameraEntity = m_ActiveScene->GetPrimaryCamera();
        //if (cameraEntity)
        //{
            //auto& cc = cameraEntity.GetComponent<CameraComponent>().camera;
            //glm::mat4 cameraProjection = cc.GetProjection();
            //glm::mat4 cameraView = glm::lookAt((cameraEntity.GetComponent<TransformComponent>().Translate),
            //    glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.f, 1.f, 0.f));*/

            //EditorCamera
            glm::mat4 cameraProjection = m_EditorCamera.GetProjection();
            glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

            if (selectedEntity)
            {
                auto& tc = selectedEntity.GetComponent<TransformComponent>();
                glm::mat4 transform = tc.GetTransform();

                //ImGuizmo::SetOrthographic(bool(cc.GetProjectionType()));
                ImGuizmo::SetOrthographic(false);
                ImGuizmo::BeginFrame();

                EditTransform(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), glm::value_ptr(transform), m_GizmoType);

                if (ImGuizmo::IsUsing())
                {
                    //glm::vec3 translation, rotation, scale;
                    //Math::DecomposeTransform(transform, translation, rotation, scale);

                    //glm::vec3 deltaRotation = rotation - tc.Rotation;
                    //tc.Translate = translation;
                    //tc.Rotation += deltaRotation;
                    //tc.Scale = scale;

                    glm::vec3 translation, scale, skew;
                    glm::quat rotation;
                    glm::vec4 perspective;
                    glm::decompose(transform, scale, rotation, translation, skew, perspective);
                    tc.Rotation = glm::eulerAngles(rotation);
                    tc.Translate = translation;
                    tc.Scale = scale;
                }

            }
        //}
        ImGui::End();
        ImGui::PopStyleVar();
        //----------------Viewport--------------------

        ImGui::End();
    }

    void EditorLayer::OnUpdate(TimeStep& ts)
    {
        //--------interval--------------
        if (time_temp >= 1.0) {
            TGE_TRACE("fps: {0} ({1}ms)", int(1 / ts.GetTimeSeconds()), ts.GetTimemillSeconds());
            time_temp = 0.0;
        }
        else {
            time_temp += ts.GetTimeSeconds();
        }
        //--------------------Camera----------------
        if (m_ViewportFocused)//没focuse则不更新相机
        {
            m_CameraController.OnUpdate(ts);    
        }
        m_EditorCamera.OnUpdate(ts);
        //------------------Resize-----------------
        FrameBufferSpecification spec = m_FrameBuffer->GetSpecification();
        if (m_ViewportSize.x > 0 && m_ViewportSize.y > 0 && (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
        {
            m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_CameraController.Resize(m_ViewportSize.x, m_ViewportSize.y);
            m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
            m_ActiveScene->OnViewportResize(uint32_t(m_ViewportSize.x), uint32_t(m_ViewportSize.y));
        }
        //------------------Renderer---------------
        Renderer2D::ResetStats();
        m_FrameBuffer->Bind();
        //以下是绘制到缓冲的部分
        RenderCommand::SetClearColor({ 0.1f, 0.2f, 0.3f, 1.0f });
        RenderCommand::Clear();
        
        m_FrameBuffer->ClearAttachment(1, -1);
        //--------------------Scene----------------
        /*Renderer2D::BeginScene(m_CameraController.GetCamera());*/
        //m_ActiveScene->OnUpdateRunTime(ts);
        m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
        //Renderer2D::EndScene();

        //-------------------MousePos-------------------
        auto [mx, my] = ImGui::GetMousePos();
        mx -= m_ViewportBounds[0].x;
        my -= m_ViewportBounds[0].y;
        glm::vec2 viewportSize = m_ViewportBounds[1] - m_ViewportBounds[0];
        my = viewportSize.y - my;//翻转y轴以匹配纹理坐标
        int mouseX = (int)mx;
        int mouseY = (int)my;

        if (mouseX >= 0 && mouseY >= 0 && mouseX < (int)m_ViewportSize.x && mouseY < (int)m_ViewportSize.y)
        {
            int pixel = m_FrameBuffer->ReadPixel(1, mouseX, mouseY);
            m_HoveredEntity = pixel == -1 ? Entity() : Entity((entt::entity)pixel, m_ActiveScene.get());
        }

        m_FrameBuffer->UnBind();
    }

    void EditorLayer::OnEvent(Event& event)
    {
        m_CameraController.OnEvent(event);
        m_EditorCamera.OnEvent(event);

        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(TGE_BIND_EVENT_FN(EditorLayer::OnKeyPressed));
        dispatcher.Dispatch<MouseButtonPressedEvent>(TGE_BIND_EVENT_FN(EditorLayer::OnMouseButtonPressed));

    }

    void EditorLayer::OnDetach()
    {
    }
    bool EditorLayer::OnMouseButtonPressed(MouseButtonPressedEvent& e)
    {
        if (e.GetMouseButton() == TGE_MOUSE_BUTTON_LEFT)
        {
            if (m_ViewportHovered && !ImGuizmo::IsOver())
            {
                m_SHP.SetSelectedEntity(m_HoveredEntity);
                return true;
            }
        }
        return false;
    }
    bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
    {
        //shortcuts
        if (e.GetRepeatCount() > 0)
            return false;
        bool control = Input::IsKeyPressed(TGE_KEY_LEFT_CONTROL) || Input::IsKeyPressed(TGE_KEY_RIGHT_CONTROL);
        bool shift = Input::IsKeyPressed(TGE_KEY_LEFT_SHIFT) || Input::IsKeyPressed(TGE_KEY_RIGHT_SHIFT);
        switch (e.GetKeyCode())
        {
        case TGE_KEY_N:
        {
            if (control)
            {
                NewScene();
            }
            break;
        }
        case TGE_KEY_O:
        {
            if (control)
            {
                OpenScene();
            }
            break;
        }
        case TGE_KEY_S:
        {
            if (control && shift)
            {
                SaveSceneAs();
            }
            break;
        }
        //Gizmos
        case TGE_KEY_Q:
        {
            m_GizmoType = ImGuizmo::OPERATION::BOUNDS;
            break;
        }
        case TGE_KEY_W:
        {
            m_GizmoType = ImGuizmo::OPERATION::TRANSLATE;
            break;
        }
        case TGE_KEY_E:
        {
            m_GizmoType = ImGuizmo::OPERATION::SCALE;
            break;
        }
        case TGE_KEY_R:
        {
            m_GizmoType = ImGuizmo::OPERATION::ROTATE;
            break;
        }
        }
        return true;
    }
    void EditorLayer::NewScene()
    {
        m_ActiveScene = std::make_shared<Scene>();
        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_SHP.SetContext(m_ActiveScene);
    }
    void EditorLayer::OpenScene()
    {
        std::string filepath = FileDialogs::OpenFile("TGE Scene (*.tge)\0*.tge\0");//传入filter
        if (!filepath.empty())
        {
            m_ActiveScene = std::make_shared<Scene>();
            m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_SHP.SetContext(m_ActiveScene);

            Serializer serializer(m_ActiveScene);
            serializer.DeSerialize(filepath);
        }
    }
    void EditorLayer::SaveSceneAs()
    {
        std::string filepath = FileDialogs::SaveFile("TGE Scene (*.tge)\0*.tge\0");
        if (!filepath.empty())
        {
            Serializer serializer(m_ActiveScene);
            serializer.Serialize(filepath);
        }
    }
}
