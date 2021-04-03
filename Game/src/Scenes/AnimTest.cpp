#include "AnimTest.hpp"


AnimationScene::AnimationScene() {
    Name = "AnimationScene";
}

AnimationScene::~AnimationScene() {

}


void AnimationScene::OnAttach() {
    ENGINE_LOG_INFO("AnimationScene Attach");

    m_ViewportSize = {
        (float)Engine::Application::Get().GetWindow().GetWidth(),
        (float)Engine::Application::Get().GetWindow().GetHeight() };
}

void AnimationScene::OnDetach() {
    ENGINE_LOG_INFO("AnimationScene Detach");
}

void AnimationScene::OnUpdate(Engine::Timestep ts) {
    CheckViewportSize();

    // Setup Render
    Engine::RenderCommand::SetClearColor(math::vec4(.1, .1, .1, 1));
    Engine::RenderCommand::Clear();

    // Fill background
    int numX = 8;
    float ratio = m_ViewportSize.x / m_ViewportSize.y;
    Engine::SpriteRect src{ 0, 0, numX, numX / ratio };
    Engine::SpriteRect dst{ 0, 0, m_ViewportSize.x, m_ViewportSize.y };
    Engine::SpriteRenderer::SubmitSprite("Data/Images/grid/PNG/Dark/texture_09.png", &dst, &src);

    // draw title
    float x = m_ViewportSize.x / 2.0f;
    Engine::TextRenderer::SubmitText("font_medium", "Animation testing", x, 15.0f, math::vec3(.1f, .55f, .2f), TextAlignment::ALIGN_TOP_MID);

    /*
    // draw line
    for (int n = 0; n < m_joints.size(); n++) {
        const auto& joint = m_joints[n];

        u32 xpos = 600 + (60.0f * joint.position.x);
        u32 ypos = 600 - (60.0f * joint.position.z);
        math::vec4 v(0, 1, 0, 0);
        math::vec4 q ( joint.orientation.x,  joint.orientation.y,  joint.orientation.z, joint.orientation.w);
        math::vec4 qp(-joint.orientation.x, -joint.orientation.y, -joint.orientation.z, joint.orientation.w);
        math::vec3 vp = math::vec3(qp * v * q);
        //vp = math::vec3(100, 0, 0);
        math::vec3 newPos = joint.position + vp;
        u32 xpos2 = 600 + (60.0f * newPos.x);
        u32 ypos2 = 600 - (60.0f * newPos.z);

        Engine::TextRenderer::SubmitText("font_small", joint.name, xpos, ypos, math::vec3(.1f, .55f, .2f), TextAlignment::ALIGN_BOT_LEFT);
        Engine::SpriteRenderer::SubmitLine(xpos, ypos, xpos2, ypos2, math::vec4(1, 0, 1, 1));
    }
    */
}

void AnimationScene::OnEvent(Engine::Event& event) {
    // respond to input/game events
    Engine::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Engine::KeyPressedEvent>(BIND_EVENT_FN(AnimationScene::OnKeyPressedEvent));

    dispatcher.Dispatch<Engine::WindowResizeEvent>([&](Engine::WindowResizeEvent& e) {
        m_ViewportSize = {
            (float)e.GetWidth(),
            (float)e.GetHeight()
        };

        return false;
    });
}

bool AnimationScene::OnKeyPressedEvent(Engine::KeyPressedEvent& e) {
    // quit game
    if (e.GetKeyCode() == KEY_CODE_ESCAPE) {
        Engine::Application::Get().Close();
    }

    return false;
}

void AnimationScene::OnGuiRender() {
    // render debug ui
}

void AnimationScene::CheckViewportSize() {
    static u32 specWidth = -1;
    static u32 specHeight = -1;

    if (m_ViewportSize.x > 0.0f && m_ViewportSize.y > 0.0f &&
        (specWidth != m_ViewportSize.x || specHeight != m_ViewportSize.y)) {
        specWidth = (u32)m_ViewportSize.x;
        specHeight = (u32)m_ViewportSize.y;
        ENGINE_LOG_INFO("Render resolution: {0} x {1}", specWidth, specHeight);
        Engine::TextRenderer::OnWindowResize(specWidth, specHeight);
        Engine::SpriteRenderer::OnWindowResize(specWidth, specHeight);
    }
}