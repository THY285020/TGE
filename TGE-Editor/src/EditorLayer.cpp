#include "tgpch.h"

#include <glm/gtx/matrix_decompose.hpp>
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include "ImGuizmo.h"

#include "EditorLayer.h"
#include "TGE/Utils/PlatformUtils.h"
#include "TGE/Scene/Serializer.h"
#include "TGE/Math/Math.h"

#include <glad/glad.h>

namespace TGE
{

    static const glm::mat4 originGridMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1, 0));

    //extern const std::filesystem::path s_Assetpath;
    EditorLayer::EditorLayer() :Layer("EditorLayer"), m_CameraController(1280.f / 720.f, true)
    {
    }

    void EditorLayer::OnAttach()
    {
        ////Texture Init here
        //m_Texture = Texture2D::Create("assets/textures/wood.png");
        m_IconPlay = Texture2D::Create("assets/Icons/PlayButton.png");
        m_IconSimulate = Texture2D::Create("assets/Icons/SimulateButton.png");
        m_IconStop = Texture2D::Create("assets/Icons/StopButton.png");

        FrameBufferSpecification fbSpec;
        fbSpec.Attachments = { FramebufferTextureFormat::RGBA8, FramebufferTextureFormat::RED_INT, FramebufferTextureFormat::Depth };
        fbSpec.Width = Application::Get().GetWindow().GetWidth();//Application::Get().GetWindow().GetWidth();
        fbSpec.Height = Application::Get().GetWindow().GetHeight();//Application::Get().GetWindow().GetHeight();
        m_FrameBuffer = FrameBuffer::Create(fbSpec);

        //Entity
        m_EditorScene = std::make_shared<Scene>();
        m_ActiveScene = m_EditorScene;
        //m_ActiveScene = std::make_shared<Scene>(fbSpec.Width, fbSpec.Height);//创建registry
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
        m_CameraSpeed = m_EditorCamera.GetMoveSpeed();
        m_SHP.SetContext(m_ActiveScene);

        Grid::Init();
        m_skyRender.Init(fbSpec.Width, fbSpec.Height);
    }


    void EditorLayer::EditTransform(float* cameraView, float* cameraProjection, float* matrix, int& m_GizmoType)
    {
        static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::LOCAL);

            //SNAP
        bool snap = Input::IsKeyPressed(TGE_KEY_LEFT_CONTROL);
        float snapValue = 0.5f; // Snap to 0.5m for translation/scale
        // Snap to 45 degrees for rotation
        if (m_GizmoType == ImGuizmo::OPERATION::ROTATE)
            snapValue = 30.0f;

        float snapValues[3] = { snapValue, snapValue, snapValue };

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

        //ImGuizmo::DrawGrid(cameraView, cameraProjection, glm::value_ptr(originGridMatrix), 100.f);

        ImGuizmo::Manipulate(cameraView, cameraProjection, (ImGuizmo::OPERATION)m_GizmoType, mCurrentGizmoMode, matrix, NULL, snap ? snapValues : NULL);//matrix

        //ImGuizmo::ViewManipulate(cameraView, 8.f, ImVec2(viewManipulateRight - 128, viewManipulateTop), ImVec2(128, 128), 0x10101010);    
    }
    void EditorLayer::UI_Toolbar()
    {
        //运行，停止按钮
        //style
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 2));
        ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, ImVec2(0, 0));
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
        auto& colors = ImGui::GetStyle().Colors;
        const auto& buttonHovered = colors[ImGuiCol_ButtonHovered];
        const auto& buttonActive = colors[ImGuiCol_ButtonActive];
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(buttonHovered.x, buttonHovered.y, buttonHovered.z, 0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(buttonActive.x, buttonActive.y, buttonActive.z, 0.5f));

        ImGui::Begin("##toolbar", nullptr, ImGuiWindowFlags_NoDecoration| ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        float size = ImGui::GetWindowHeight()-4.0f;
        {
            Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate) ? m_IconPlay : m_IconStop;
            ImGui::SameLine(ImGui::GetWindowContentRegionMax().x * 0.5 - size * 0.5);
            if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0) )
            {
                if (m_SceneState == SceneState::Play)
                    OnSceneStop();
                else if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Simulate)
                    OnScenePlay();
            }
        }
        ImGui::SameLine();
        {
            Ref<Texture2D> icon = (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play) ? m_IconSimulate : m_IconStop;
            if (ImGui::ImageButton((ImTextureID)icon->GetRendererID(), ImVec2(size, size), ImVec2(0, 0), ImVec2(1, 1), 0) )
            {
                if (m_SceneState == SceneState::Simulate)
                    OnSceneStop();
                else if (m_SceneState == SceneState::Edit || m_SceneState == SceneState::Play)
                    OnSceneSimulate();
            }
        }
        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);
        ImGui::End();
    }

    void EditorLayer::OnScenePlay()
    {
        if (m_SceneState == SceneState::Simulate)
            m_SceneState = SceneState::Edit;
        m_SceneState = SceneState::Play;

        if(m_EditorScene != nullptr)
            m_ActiveScene = Scene::Copy(m_EditorScene);
        m_ActiveScene->OnRuntimeStart();//world2D物理
        m_SHP.SetContext(m_ActiveScene);
    }

    void EditorLayer::OnSceneSimulate()
    {
        if (m_SceneState == SceneState::Play)
            m_SceneState = SceneState::Edit;

        m_SceneState = SceneState::Simulate;

        if (m_EditorScene != nullptr)
            m_ActiveScene = Scene::Copy(m_EditorScene);
        m_ActiveScene->OnSimulationStart();//world2D物理
        m_SHP.SetContext(m_ActiveScene);
    }

    void EditorLayer::OnSceneStop()
    {
        if(m_SceneState == SceneState::Play)
            m_ActiveScene->OnRuntimeStop();
        else if(m_SceneState == SceneState::Simulate)
            m_ActiveScene->OnSimulationStop();

        m_SceneState = SceneState::Edit;

        m_ActiveScene = m_EditorScene;
        m_SHP.SetContext(m_ActiveScene);
    }


    void EditorLayer::DuplicateEntity()
    {
        if (m_SceneState != SceneState::Edit)
            return;
        Entity m_selectedEntity = m_SHP.GetSelectedEntity();
        if (m_selectedEntity)
        {
            m_selectedEntity = m_ActiveScene->DuplicateEntity(m_selectedEntity);
            m_SHP.SetSelectedEntity(m_selectedEntity);
        }
    }

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

        m_SHP.OnImGuiRenderer();//层级菜单
        m_CBP.OnImGuiRenderer();//资产菜单
        //--------------Statistics-------------------
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

        ImGui::Separator();

        auto stats3d = Renderer3D::GetStats();
        ImGui::Text("Renderer3D Stats:");
        ImGui::Text("Draw Calls: %d", stats3d.DrawCalls);
        ImGui::Text("Cubes : %d", stats3d.CubeCount);
        ImGui::Text("Sphere : %d", stats3d.SphereCount);
        ImGui::Text("Vertices : %d", stats3d.GetTotalVertexCount());
        ImGui::Text("Indices : %d", stats3d.GetTotalIndexCount());

        ImGui::End();

        //--------------Settings-------------------
        ImGui::Begin("Settings");

        //Moving Speed
        if (ImGui::SliderFloat("Move Speed", &m_CameraSpeed, 1, 5))
        {
            m_EditorCamera.SetMoveSpeed(m_CameraSpeed);
        }

        ImGui::Checkbox("Show Grid", &m_ShowGrid);

        //Physics Colliders
        ImGui::Checkbox("Show physics colliders", &m_ShowPhysicsColliders);

        static int model = 1;
        ImGui::RadioButton("MCF-STGCN", &model, 0);
        ImGui::SameLine();
        ImGui::RadioButton("DSSTGCN", &model, 1);


        ImGui::End();

        //----------------Viewport--------------------
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0, 0.0));//间隔缩小为0
        ImGui::Begin("ViewPort");
        
        //点击选中或悬停都会更新界面
        m_ViewportFocused = ImGui::IsWindowFocused();
        m_ViewportHovered = ImGui::IsWindowHovered();
        Application::Get().GetImGuiLayer()->BlockEvents(!m_ViewportHovered);

        //Resize此处更新会闪屏
        ImVec2 viewportPanelSize = ImGui::GetContentRegionAvail();
        if (m_ViewportSize != *((glm::vec2*)&viewportPanelSize) && viewportPanelSize.x > 0 && viewportPanelSize.y > 0)
        {
            m_ViewportSize = { viewportPanelSize.x, viewportPanelSize.y };
        }//

        uint32_t m_TextureID = m_FrameBuffer->GetColorAttachment(0);
        ImGui::Image((void*)m_TextureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        //接收拖动资源的载荷数据,此处是场景文件
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("CONTENT_BROWSER_ITEM"))
            {
                const wchar_t* path = (const wchar_t*)payload->Data;
                OpenScene(std::filesystem::path(path));//s_Assetpath/path
            }
            ImGui::EndDragDropTarget();
        }

        //ViewportBounds ImGui从左上(0,0)到右下
        auto viewportMinRegion = ImGui::GetWindowContentRegionMin();
        auto viewportMaxRegion = ImGui::GetWindowContentRegionMax();
        auto viewportOffset = ImGui::GetWindowPos();
        m_ViewportBounds[0] = { viewportMinRegion.x + viewportOffset.x, viewportMinRegion.y + viewportOffset.y };
        m_ViewportBounds[1] = { viewportMaxRegion.x + viewportOffset.x, viewportMaxRegion.y + viewportOffset.y };

        Entity selectedEntity = m_SHP.GetSelectedEntity();


        //EditorCamera
        if (m_SceneState == SceneState::Edit)
        {
            glm::mat4 cameraProjection = m_EditorCamera.GetProjection();
            glm::mat4 cameraView = m_EditorCamera.GetViewMatrix();

            if (selectedEntity)
            {
                auto& tc = selectedEntity.GetComponent<TransformComponent>();
                glm::mat4 transform = tc.GetTransform();

                //ImGuizmo::SetOrthographic(bool(cc.GetProjectionType()));
                ImGuizmo::SetOrthographic(false);
                ImGuizmo::BeginFrame();
                float* matrix = glm::value_ptr(transform);
                EditTransform(glm::value_ptr(cameraView), glm::value_ptr(cameraProjection), matrix, m_GizmoType);

                if (ImGuizmo::IsUsing())
                {
                    //old
                    //glm::vec3 translation, scale, skew;
                    //glm::quat rotation;
                    //glm::vec4 perspective;
                    //glm::decompose(transform, scale, rotation, translation, skew, perspective);
                    //tc.Rotation = glm::eulerAngles(rotation);
                    //tc.Translate = translation;
                    //tc.Scale = scale;
                    float matrixTranslation[3], matrixRotation[3], matrixScale[3];
                    ImGuizmo::DecomposeMatrixToComponents(matrix, matrixTranslation, matrixRotation, matrixScale);
                    tc.Translate = glm::vec3(matrixTranslation[0], matrixTranslation[1], matrixTranslation[2]);
                    tc.Rotation = glm::vec3(glm::radians(matrixRotation[0]), glm::radians(matrixRotation[1]), glm::radians(matrixRotation[2]));
                    tc.Scale = glm::vec3(matrixScale[0], matrixScale[1], matrixScale[2]);
                }
            }
        }

        ImGui::End();
        ImGui::PopStyleVar();
        //----------------Viewport--------------------

        UI_Toolbar();
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
        //双击跳转
        if (m_ActiveScene->CameraNeedUpdate)
        {
            m_EditorCamera.SetFocusPos(m_ActiveScene->m_UpdateTransform);
            m_ActiveScene->CameraNeedUpdate = false;
        }
        //------------------Resize-----------------
        FrameBufferSpecification spec = m_FrameBuffer->GetSpecification();
        if (m_ViewportSize.x > 0 && m_ViewportSize.y > 0 && (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
        {
            m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_CameraController.Resize(m_ViewportSize.x, m_ViewportSize.y);
            m_EditorCamera.SetViewportSize(m_ViewportSize.x, m_ViewportSize.y);
            m_ActiveScene->OnViewportResize(uint32_t(m_ViewportSize.x), uint32_t(m_ViewportSize.y));
            m_skyRender.Resize(m_ViewportSize.x, m_ViewportSize.y);
        }
        //------------------Renderer---------------
        Renderer2D::ResetStats();
        Renderer3D::ResetStats();
        m_FrameBuffer->Bind();
        //以下是绘制到缓冲的部分
        RenderCommand::SetClearColor({ 0.1f, 0.2f, 0.3f, 1.0f });
        RenderCommand::Clear();
        
        m_FrameBuffer->ClearAttachment(1, -1);//刷新帧缓冲1的slot为-1(entity)
        //--------------------Scene----------------
        /*Renderer2D::BeginScene(m_CameraController.GetCamera());*/
        
        //glStencilOp(GL_KEEP, GL_REPLACE, GL_REPLACE);//sfail,dpfail,allpass 
        //glStencilFunc(GL_ALWAYS, 1, 0xFF); // 所有的片段都应该更新模板缓冲 func,ref,mask
        //glStencilMask(0xFF); // 启用模板缓冲写入

        switch (m_SceneState)
        {
            case SceneState::Edit:
                if (m_ViewportFocused)//没focuse则不更新相机
                {
                    m_CameraController.OnUpdate(ts);
                }
                m_EditorCamera.OnUpdate(ts);//鼠标控制
                m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
                break;
            case SceneState::Simulate:
                m_EditorCamera.OnUpdate(ts);
                m_ActiveScene->OnUpdateSimulation(ts, m_EditorCamera);
                break;
            case SceneState::Play:
                m_ActiveScene->OnUpdateRunTime(ts);
                break;
        }

        if (m_ShowGrid)
        {
            Grid::Begin(m_EditorCamera);
            Grid::Draw();
        }


        //m_ActiveScene->OnUpdateRunTime(ts);
        //m_ActiveScene->OnUpdateEditor(ts, m_EditorCamera);
        //Renderer2D::EndScene();

        //glStencilFunc(GL_NOTEQUAL, 1, 0xFF);//只绘制不等于1的部分
        //glStencilMask(0x00); // 禁止模板缓冲的写入
        //glDisable(GL_DEPTH_TEST);

        OnOverlayRender();

        //glStencilMask(0xFF);
        //glStencilFunc(GL_ALWAYS, 0, 0xFF);
        //glEnable(GL_DEPTH_TEST);

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
            int pixel = m_FrameBuffer->ReadPixel(1, mouseX, mouseY);//读取鼠标处的缓冲值
            m_HoveredEntity = pixel == -1 ? Entity() : Entity((entt::entity)pixel, m_ActiveScene.get());
        }

        //防止在屏幕外操作
        if (!m_ViewportHovered)
        {
            m_EditorCamera.OnOutofScreen();
        }

        m_FrameBuffer->UnBind();

        m_skyRender.Render(m_EditorCamera);//渲染到球体
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
        if (e.GetRepeatCount() > 0 )
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
                if (control)
                {
                    if (shift)
                        SaveSceneAs();
                    else
                        SaveScene();
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
            case TGE_KEY_D:
            {
                if (control)
                {
                    DuplicateEntity();
                }
                break;
            }
        }
        return true;
    }
    void EditorLayer::OnOverlayRender()
    {
        //绘制圆圈组件
        if (m_SceneState == SceneState::Play)
        {
            auto camera = m_ActiveScene->GetPrimaryCamera();
            if (!camera)//空白场景不报错
                return;
            Renderer2D::BeginScene(camera.GetComponent<CameraComponent>().camera, camera.GetComponent<TransformComponent>().GetTransform()); 
        }
        else
        {
            Renderer2D::BeginScene(m_EditorCamera);
        }
        if (m_ShowPhysicsColliders)
        {
            //Box Colliders
            {
                auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, BoxCollider2DComponent>();
                for (auto entity : view)
                {
                    auto [tc, bc2d] = view.get<TransformComponent, BoxCollider2DComponent>(entity);
                    //glm::vec3 translation = tc.Translate + glm::vec3(bc2d.Offset, 0.001f);
                    //glm::vec2 bs = bc2d.Size * 2.0f;
                    //glm::vec3 scale = tc.Scale * glm::vec3(bs.x + 0.1f, bs.y+0.1f, 1.1f);
                    glm::vec3 scale = tc.Scale * glm::vec3(bc2d.Size * 2.0f+0.01f, 1.0f);

                    glm::mat4 transform = glm::translate(glm::mat4(1.0f), tc.Translate)
                        * glm::translate(glm::mat4(1.0f), glm::vec3(bc2d.Offset, 0.0f))//glm::vec3(bc2d.Offset, 0.001f
                        * glm::rotate(glm::mat4(1.0f), tc.Rotation.z, glm::vec3(0.0f, 0.0f, 1.0f))
                        * glm::scale(glm::mat4(1.0f), scale);

                    Renderer2D::DrawRect(transform, glm::vec4(0, 1, 0, 1));
                    //Renderer2D::DrawQuad(transform, glm::vec4(0, 1, 0, 1));
                }
            }
            //Circle Colliders
            {
                auto view = m_ActiveScene->GetAllEntitiesWith<TransformComponent, CircleCollider2DComponent>();
                for (auto entity : view)
                {
                    auto [tc, cc2d] = view.get<TransformComponent, CircleCollider2DComponent>(entity);
                    glm::vec3 translation = tc.Translate + glm::vec3(cc2d.Offset, 0.0f);
                    glm::vec3 scale = tc.Scale * glm::vec3(cc2d.Radius * 2.0f + 0.01f);

                    glm::mat4 transform = glm::translate(glm::mat4(1.0f), glm::vec3(translation))
                        * glm::scale(glm::mat4(1.0f), scale);
                    //Stencil
                    //Entity e = Entity(entity, m_ActiveScene.get());
                    //if (e.HasComponent<CircleRendererComponent>())
                    //{
                    //    float th = e.GetComponent<CircleRendererComponent>().Thickness;
                    //    Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), th + 0.01f);
                    //}
                    //else
                    //{
                    //    Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.01f);
                    //}
                    Renderer2D::DrawCircle(transform, glm::vec4(0, 1, 0, 1), 0.01f);
                   
                }
            }
        }

        Renderer2D::EndScene();
    }
    void EditorLayer::NewScene()
    {
        m_ActiveScene = std::make_shared<Scene>();
        m_ActiveScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        m_SHP.SetContext(m_ActiveScene);
        m_EditorScenePath = std::filesystem::path();
    }
    void EditorLayer::OpenScene()
    {
        std::string filepath = FileDialogs::OpenFile("TGE Scene (*.tge)\0*.tge\0");//传入filter
        if (!filepath.empty())
        {
            OpenScene(filepath);
            m_EditorScenePath = filepath;
        }
    }
    void EditorLayer::OpenScene(const std::filesystem::path& path)
    {
        if (m_SceneState != SceneState::Edit)
        {
            OnSceneStop();
        }

        Ref<Scene> newScene = std::make_shared<Scene>();
        Serializer serializer(newScene);
        //需要先调用OnViewportResize计算aspectRatio再计算glm::perspective
        m_EditorScene = newScene;
        m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
        if (serializer.DeSerialize(path.string()))
        {
            //m_EditorScene = newScene;
            //m_EditorScene->OnViewportResize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_ActiveScene = m_EditorScene;
            m_SHP.SetContext(m_ActiveScene);
            m_EditorScenePath = path;
        }
    }
    void EditorLayer::SaveSceneAs()
    {
        std::string filepath = FileDialogs::SaveFile("TGE Scene (*.tge)\0*.tge\0");
        if (!filepath.empty())
        {
            SceneSerialize(m_ActiveScene, filepath);
        }
    }
    void EditorLayer::SaveScene()
    {
        if (!m_EditorScenePath.empty())
        {
            SceneSerialize(m_ActiveScene, m_EditorScenePath);
        }
        else
            SaveSceneAs();
    }
    void EditorLayer::SceneSerialize(Ref<Scene> scene, std::filesystem::path path)
    {
        Serializer serializer(scene);
        serializer.Serialize(path.string());
    }
}
