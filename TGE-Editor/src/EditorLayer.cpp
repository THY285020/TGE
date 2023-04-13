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

    //template <typename T, typename ... U> Array(T, U...)->Array<T, 1 + sizeof...(U)>;
    //
    //struct GraphEditorDelegate : public GraphEditor::Delegate
    //{
    //    bool AllowedLink(GraphEditor::NodeIndex from, GraphEditor::NodeIndex to) override
    //    {
    //        return true;
    //    }
    //
    //    void SelectNode(GraphEditor::NodeIndex nodeIndex, bool selected) override
    //    {
    //        mNodes[nodeIndex].mSelected = selected;
    //    }
    //
    //    void MoveSelectedNodes(const ImVec2 delta) override
    //    {
    //        for (auto& node : mNodes)
    //        {
    //            if (!node.mSelected)
    //            {
    //                continue;
    //            }
    //            node.x += delta.x;
    //            node.y += delta.y;
    //        }
    //    }
    //
    //    virtual void RightClick(GraphEditor::NodeIndex nodeIndex, GraphEditor::SlotIndex slotIndexInput, GraphEditor::SlotIndex slotIndexOutput) override
    //    {
    //    }
    //
    //    void AddLink(GraphEditor::NodeIndex inputNodeIndex, GraphEditor::SlotIndex inputSlotIndex, GraphEditor::NodeIndex outputNodeIndex, GraphEditor::SlotIndex outputSlotIndex) override
    //    {
    //        mLinks.push_back({ inputNodeIndex, inputSlotIndex, outputNodeIndex, outputSlotIndex });
    //    }
    //
    //    void DelLink(GraphEditor::LinkIndex linkIndex) override
    //    {
    //        mLinks.erase(mLinks.begin() + linkIndex);
    //    }
    //
    //    void CustomDraw(ImDrawList* drawList, ImRect rectangle, GraphEditor::NodeIndex nodeIndex) override
    //    {
    //        drawList->AddLine(rectangle.Min, rectangle.Max, IM_COL32(0, 0, 0, 255));
    //        drawList->AddText(rectangle.Min, IM_COL32(255, 128, 64, 255), "Draw");
    //    }
    //
    //    const size_t GetTemplateCount() override
    //    {
    //        return sizeof(mTemplates) / sizeof(GraphEditor::Template);
    //    }
    //
    //    const GraphEditor::Template GetTemplate(GraphEditor::TemplateIndex index) override
    //    {
    //        return mTemplates[index];
    //    }
    //
    //    const size_t GetNodeCount() override
    //    {
    //        return mNodes.size();
    //    }
    //
    //    const GraphEditor::Node GetNode(GraphEditor::NodeIndex index) override
    //    {
    //        const auto& myNode = mNodes[index];
    //        return GraphEditor::Node
    //        {
    //            myNode.name,
    //            myNode.templateIndex,
    //            ImRect(ImVec2(myNode.x, myNode.y), ImVec2(myNode.x + 200, myNode.y + 200)),
    //            myNode.mSelected
    //        };
    //    }
    //
    //    const size_t GetLinkCount() override
    //    {
    //        return mLinks.size();
    //    }
    //
    //    const GraphEditor::Link GetLink(GraphEditor::LinkIndex index) override
    //    {
    //        return mLinks[index];
    //    }
    //
    //    // Graph datas
    //    static const inline GraphEditor::Template mTemplates[] = {
    //        {
    //            IM_COL32(160, 160, 180, 255),
    //            IM_COL32(100, 100, 140, 255),
    //            IM_COL32(110, 110, 150, 255),
    //            1,
    //            Array{"MyInput"},
    //            nullptr,
    //            2,
    //            Array{"MyOutput0", "MyOuput1"},
    //            nullptr
    //        },
    //
    //        {
    //            IM_COL32(180, 160, 160, 255),
    //            IM_COL32(140, 100, 100, 255),
    //            IM_COL32(150, 110, 110, 255),
    //            3,
    //            nullptr,
    //            Array{ IM_COL32(200,100,100,255), IM_COL32(100,200,100,255), IM_COL32(100,100,200,255) },
    //            1,
    //            Array{"MyOutput0"},
    //            Array{ IM_COL32(200,200,200,255)}
    //        }
    //    };
    //
    //    struct Node
    //    {
    //        const char* name;
    //        GraphEditor::TemplateIndex templateIndex;
    //        float x, y;
    //        bool mSelected;
    //    };
    //
    //    std::vector<Node> mNodes = {
    //        {
    //            "My Node 0",
    //            0,
    //            0, 0,
    //            false
    //        },
    //
    //        {
    //            "My Node 1",
    //            0,
    //            400, 0,
    //            false
    //        },
    //
    //        {
    //            "My Node 2",
    //            1,
    //            400, 400,
    //            false
    //        }
    //    };
    //
    //    std::vector<GraphEditor::Link> mLinks = { {0, 0, 1, 0} };
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

        ImGui::Begin("Statistics");
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

        uint32_t m_TextureID = m_FrameBuffer->GetColorAttachment();
        ImGui::Image((void*)m_TextureID, ImVec2{ m_ViewportSize.x, m_ViewportSize.y }, ImVec2{ 0, 1 }, ImVec2{ 1, 0 });

        Entity selectedEntity = m_SHP.GetSelectedEntity();

        //auto cameraEntity = m_Camera;
        auto cameraEntity = m_ActiveScene->GetPrimaryCamera();
        if (cameraEntity)
        {
            auto& cc = cameraEntity.GetComponent<CameraComponent>().camera;
            glm::mat4 cameraProjection = cc.GetProjection();
            glm::mat4 cameraView = glm::lookAt((cameraEntity.GetComponent<TransformComponent>().Translate),
                glm::vec3(0.0, 0.0, -1.0), glm::vec3(0.f, 1.f, 0.f));

            if (selectedEntity)
            {
                auto& tc = selectedEntity.GetComponent<TransformComponent>();
                glm::mat4 transform = tc.GetTransform();

                ImGuizmo::SetOrthographic(bool(cc.GetProjectionType()));
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
        }
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
            m_CameraController.OnUpdate(ts);
        //------------------Resize-----------------
        FrameBufferSpecification spec = m_FrameBuffer->GetSpecification();
        if (m_ViewportSize.x > 0 && m_ViewportSize.y > 0 && (spec.Width != m_ViewportSize.x || spec.Height != m_ViewportSize.y))
        {
            m_FrameBuffer->Resize((uint32_t)m_ViewportSize.x, (uint32_t)m_ViewportSize.y);
            m_CameraController.Resize(m_ViewportSize.x, m_ViewportSize.y);
            m_ActiveScene->OnViewportResize(uint32_t(m_ViewportSize.x), uint32_t(m_ViewportSize.y));
        }
        //------------------Renderer---------------
        Renderer2D::ResetStats();
        m_FrameBuffer->Bind();
        //以下是绘制到缓冲的部分
        RenderCommand::SetClearColor({ 0.1f, 0.2f, 0.3f, 1.0f });
        RenderCommand::Clear();

        //--------------------Scene----------------
        /*Renderer2D::BeginScene(m_CameraController.GetCamera());*/
        m_ActiveScene->OnUpdate(ts);
        //Renderer2D::EndScene();

        m_FrameBuffer->UnBind();
    }

    void EditorLayer::OnEvent(Event& event)
    {
        m_CameraController.OnEvent(event);

        EventDispatcher dispatcher(event);
        dispatcher.Dispatch<KeyPressedEvent>(TGE_BIND_EVENT_FN(EditorLayer::OnKeyPressed));

    }

    void EditorLayer::OnDetach()
    {
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
